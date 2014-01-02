//
//  interpreter.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "interpreter.h"
#include "word.h"
#include "str.h"
#include "number.h"
#include "expression.h"
#include "annotation.h"
#include "null.h"
#include "threading.h"

#include "stackframe.h"
#include "function.h"
#include "corefunctions.h"
#include "parser.h"

#include "file.h"

#if GFX_Include_GraphicsStack
#   include "graphics.h"
#   include "context.h"
#endif /* GFX_Include_GraphicsStack */

namespace gfx {
    
    bool Interpreter::IsTrue(Base *value)
    {
        if(!value)
            return false;
        
        if(value == Null::shared()) {
            return false;
        } else if(value->isKindOfClass<Number>()) {
            return (static_cast<Number *>(value)->value() != 0.0);
        } else {
            return true;
        }
    }
    
#pragma mark - Lifecycle
    
    Interpreter::Interpreter() :
        Base(),
        mRootFrame(retained(CoreFunctions::createCoreFunctionFrame(this))),
        mSearchPaths(new Array<const String>()),
        mImportAllowed(true),
        mWordHandlers(),
        AnnotationFoundSignal(str("gfx::Interpreter::AnnotationFoundSignal"))
    {
        ///Handles words that begin with `'` by stripping the `'` off
        ///and pushing the resulting `gfx::Word` onto the stack. Useful
        ///for functions related to the interpreter.
        this->appendWordHandler([this](StackFrame *currentFrame, Word *word) {
            if(word->string()->hasPrefix(str("'"))) {
                auto rawWord = word->string()->substring(Range(1, word->string()->length() - 1));
                currentFrame->push(make<Word>(rawWord, word->offset()));
                return true;
            } else {
                return false;
            }
        });
        
        
        ///Handles words that begin with `=>` by creating a new variable
        ///binding with the text after the arrow. The (exact) word `=>`
        ///is ignored and will be handled by another word-handler.
        this->appendWordHandler([](StackFrame *currentFrame, Word *word) {
            auto wordString = word->string();
            if(wordString->length() > 2 && wordString->hasPrefix(str("=>"))) {
                auto bindingName = wordString->substring(Range(2, wordString->length() - 2));
                currentFrame->setBindingToValue(bindingName, currentFrame->pop());
                return true;
            }
            
            return false;
        });
        
        
        ///Handles words that contain a dot (`.`). The dot is used to access
        ///data within a hash without invoking the `hash/get` functor.
        this->appendWordHandler([this](StackFrame *currentFrame, Word *word) {
            auto wordString = word->string();
            if(wordString->find(str("."), Range(0, wordString->length()))) {
                auto pieces = SplitString(wordString, str("."));
                Base *result = currentFrame->bindingValue(pieces->first()) ?: Null::shared();
                if(pieces->count() > 1) {
                    pieces->iterate(Range(1, pieces->count() - 1), [this, word, currentFrame, &result](String *subword, Index index, bool *stop) {
                        if(result->isKindOfClass<Dictionary<Base, Base>>()) {
                            auto dictionary = static_cast<Dictionary<Base, Base> *>(result);
                            result = dictionary->get(subword);
                        } else if(result == Null::shared()) {
                            *stop = true;
                            return;
                        } else {
                            this->fail((String::Builder() << "Non-dictionary object used in dot-lookup: " << result), word->offset());
                        }
                    });
                }
                
                currentFrame->push(result);
                
                return true;
            } else {
                return false;
            }
        });
        
        ///Looks up the word in the current frame, pushing the value if one is found.
        ///
        ///This handler should always be last.
        this->appendWordHandler([this](StackFrame *currentFrame, Word *word) {
            auto value = currentFrame->bindingValue(word->string());
            if(value) {
                currentFrame->push(value);
                return true;
            } else {
                return false;
            }
        });
        
        
        
        
#if GFX_Include_GraphicsStack
        Graphics::AttachTo(this);
#endif /* GFX_Include_GraphicsStack */
        
        this->addSearchPath(String::Empty);
        this->addSearchPath(str("./"));
    }
    
    Interpreter::~Interpreter()
    {
        released(mRootFrame);
        mRootFrame = nullptr;
        
        released(mSearchPaths);
        mSearchPaths = nullptr;
    }
    
#pragma mark - Thread Local Storage
    
    Dictionary<const Base, Base> *Interpreter::threadStorage() const
    {
        auto commonStorage = threading::threadStorage();
        auto key = threading::StorageDictionary::weakKeyForObject(this);
        auto interpreterStorage = static_cast<threading::StorageDictionary *>(commonStorage->get(key));
        if(!interpreterStorage) {
            interpreterStorage = make<threading::StorageDictionary>();
            commonStorage->set(key, interpreterStorage);
        }
        
        return interpreterStorage;
    }
    
#pragma mark - Interpretation
    
    void Interpreter::fail(const String *reason, Offset source)
    {
        const String *extendedReason = String::Builder() << "From " << source.line << ":" << source.column << ": " << reason;
        auto userInfo = autoreleased(new Dictionary<const String, Base>{
            { kUserInfoKeyOffsetLine, make<Number>(source.line) },
            { kUserInfoKeyOffsetLine, make<Number>(source.column) },
        });
        
        throw Exception(extendedReason, userInfo);
    }
    
    void Interpreter::evalExpression(StackFrame *currentFrame, Base *part, EvalContext context)
    {
        gfx_assert_param(currentFrame);
        
        if(part->isKindOfClass<Word>()) {
            auto word = static_cast<Word *>(part);
            if(word->string()->hasPrefix(str("&"))) {
                //Looking up functions without applying them is a special case for now.
                auto rawWord = word->string()->substring(Range(1, word->string()->length() - 1));
                this->evalExpression(currentFrame, make<Word>(rawWord, word->offset()), EvalContext::Vector);
            } else {
                if(!this->handleWord(currentFrame, word))
                    failForUnboundWord(word);
                
                if(context != EvalContext::Vector &&
                   !currentFrame->empty() &&
                   currentFrame->peak()->isKindOfClass<Function>()) {
                    auto function = currentFrame->popFunction();
                    function->apply(currentFrame);
                }
            }
        } else if(part->isKindOfClass<String>() || part->isKindOfClass<Number>()) {
            currentFrame->push(part);
        } else if(part->isKindOfClass<Expression>()) {
            Expression *expression = static_cast<Expression *>(part);
            
            switch (expression->type()) {
                case Expression::Type::Vector: {
                    auto vector = make<Array<Base>>();
                    for (Base *subexpression : expression->subexpressions()) {
                        this->evalExpression(currentFrame, subexpression, EvalContext::Vector);
                        vector->append(currentFrame->pop());
                    }
                    
                    currentFrame->push(vector);
                    
                    break;
                }
                    
                case Expression::Type::Hash: {
                    auto subexpressions = expression->subexpressions();
                    auto count = subexpressions->count();
                    if((count % 2) != 0) {
                        fail(str("Malformed hash literal"), expression->offset());
                    }
                    
                    auto dictionary = make<Dictionary<Base, Base>>();
                    for (Index i = 0; i < count; i += 2) {
                        this->evalExpression(currentFrame, subexpressions->at(i), EvalContext::Vector);
                        auto key = currentFrame->pop();
                        
                        this->evalExpression(currentFrame, subexpressions->at(i + 1), EvalContext::Vector);
                        auto value = currentFrame->pop();
                        
                        dictionary->set(key, value);
                    }
                    
                    currentFrame->push(dictionary);
                    
                    break;
                }
                    
                case Expression::Type::Function: {
                    currentFrame->push(make<InterpretedFunction>(expression));
                    
                    break;
                }
            }
        } else if(part->isKindOfClass<Annotation>() && context == EvalContext::Normal) {
            AnnotationFoundSignal(static_cast<Annotation *>(part));
        }
    }
    
    void Interpreter::eval(StackFrame *currentFrame, const Array<Base> *expressions, EvalContext context)
    {
        gfx_assert_param(currentFrame);
        
        if(!expressions)
            return;
        
        try {
            for (Base *expression : expressions) {
                this->evalExpression(currentFrame, expression, context);
            }
        } catch (Exception &e) {
            resetFunctionStack(e);
            throw;
        }
    }
    
#pragma mark - Word Handling
    
    bool Interpreter::handleWord(StackFrame *currentFrame, Word *word)
    {
        gfx_assert_param(currentFrame);
        gfx_assert_param(word);
        
        for (WordHandler &handler : mWordHandlers) {
            if(handler(currentFrame, word))
                return true;
        }
        
        return false;
    }
    
    void Interpreter::prependWordHandler(const WordHandler &wordHandler)
    {
        gfx_assert_param(wordHandler);
        
        mWordHandlers.push_front(wordHandler);
    }
    
    void Interpreter::appendWordHandler(const WordHandler &wordHandler)
    {
        gfx_assert_param(wordHandler);
        
        mWordHandlers.push_back(wordHandler);
    }
    
    void Interpreter::failForUnboundWord(const Word *word)
    {
        fail((String::Builder() << "unbound word '" << word << "'"), word->offset());
    }
    
#pragma mark - Stack Frames
    
    StackFrame *Interpreter::rootFrame() const
    {
        return retained_autoreleased(mRootFrame);
    }
    
#pragma mark - Backtrace Tracking
    
    Array<const Function> *Interpreter::threadLocalFunctionStack() const
    {
        auto storage = threadStorage();
        auto stack = static_cast<Array<const Function> *>(storage->get(str("threadLocalFunctionStack")));
        if(!stack) {
            stack = make<Array<const Function>>();
            storage->set(str("threadLocalFunctionStack"), stack);
        }
        
        return stack;
    }
    
    void Interpreter::resetFunctionStack(Exception &e) noexcept
    {
        if(auto backtrace = const_cast<String *>(this->backtrace()))
            e.userInfo()->set(kUserInfoKeyBacktraceString, backtrace);
            
        threadLocalFunctionStack()->removeAll();
    }
    
#pragma mark -
    
    void Interpreter::enteredFunction(const Function *function)
    {
        threadLocalFunctionStack()->append(function);
    }
    
    void Interpreter::exitedFunction(const Function *function)
    {
        threadLocalFunctionStack()->remove(function);
    }
    
    const String *Interpreter::backtrace() const
    {
        auto stack = threadLocalFunctionStack();
        if(stack->count() == 0)
            return nullptr;
        
        String::Builder backtrace;
        
        backtrace << "backtrace:";
        stack->iterate(stack->all(), [&backtrace](const Function *function, Index index, bool *stop) {
            backtrace << "\n" << (index + 1) << "  " << (void *)function << ": " << function;
        });
        
        return backtrace;
    }
    
#pragma mark - Import Support
    
    void Interpreter::setImportAllowed(bool importAllowed)
    {
        mImportAllowed = importAllowed;
    }
    
    bool Interpreter::isImportAllowed() const
    {
        return mImportAllowed;
    }
    
#pragma mark -
    
    const Array<const String> *Interpreter::searchPaths() const
    {
        return retained_autoreleased(mSearchPaths);
    }
    
    void Interpreter::addSearchPath(const String *searchPath)
    {
        gfx_assert_param(searchPath);
        
        mSearchPaths->append(searchPath);
    }
    
    void Interpreter::removeSearchPath(const String *searchPath)
    {
        gfx_assert_param(searchPath);
        gfx_assert(mSearchPaths->contains(Range(0, mSearchPaths->count()), searchPath),
                str("cannot remove search path that was never in search paths array"));
        
        mSearchPaths->removeAt(mSearchPaths->firstIndexOf(Range(0, mSearchPaths->count()), searchPath));
    }
    
#pragma mark -
    
    bool Interpreter::import(StackFrame *frame, const String *filename)
    {
        gfx_assert_param(filename);
        
        if(!this->isImportAllowed())
            throw Exception(str("illegal import"), nullptr);
        
        if(FilePaths::pathExtension(filename)->length() == 0)
            filename = String::Builder() << filename << ".gfx";
        
        AutoreleasePool pool;
        for (const String *searchPath : this->searchPaths()) {
            const String *path = FilePaths::combinePaths(searchPath, filename);
            if(File::exists(path)) {
                try {
                    const String *source = File::readFileAtPath(filename);
                    this->eval(frame, Parser(source).parse());
                } catch (Exception e) {
                    return false;
                }
                
                return true;
            }
        }
        
        return false;
    }
}
