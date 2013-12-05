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
        
        if(value->isKindOfClass<Number>()) {
            return (static_cast<Number *>(value)->value() != 0.0);
        } else {
            return true;
        }
    }
    
#pragma mark - Lifecycle
    
    Interpreter::Interpreter() :
        mRootFrame(),
        mFrames(),
        mRunningFunctions(new Array<const Function>),
        mSearchPaths(make<Array<const String>>()),
        mImportAllowed(true),
        mUnboundWordHandler(),
        ResetSignal("gfx::Interpreter::ResetSignal"_gfx)
    {
        mRootFrame = CoreFunctions::createCoreFunctionFrame(this);
        this->pushFrame(mRootFrame);
        
#if GFX_Include_GraphicsStack
        Graphics::attachTo(this);
#else
        mUnboundWordHandler = [this](Word *word) {
            failForUnboundWord(word);
            return nullptr;
        };
#endif /* GFX_Include_GraphicsStack */
        
        this->addSearchPath(""_gfx);
        this->addSearchPath("./"_gfx);
    }
    
    Interpreter::~Interpreter()
    {
        while (!mFrames.empty())
            this->popFrame();
    }
    
#pragma mark - Interpretation
    
    void Interpreter::fail(const String *reason, Offset source)
    {
        const String *extendedReason = String::Builder() << "From " << source.line << ":" << source.offset << ": " << reason;
        Dictionary<const String, Base> *userInfo = make<Dictionary<const String, Base>>();
        userInfo->set("line"_gfx, make<Number>(source.line));
        userInfo->set("offset"_gfx, make<Number>(source.offset));
        
        throw Exception(extendedReason, userInfo);
    }
    
    void Interpreter::evalExpression(Base *part, EvalContext context)
    {
        StackFrame *currentFrame = this->currentFrame();
        gfx_assert(currentFrame != nullptr, "stack frame is required"_gfx);
        
        if(part->isKindOfClass<Word>()) {
            auto word = static_cast<Word *>(part);
            if(word->string()->hasPrefix("'"_gfx)) {
                auto rawWord = word->string()->substring(Range(1, word->string()->length() - 1));
                currentFrame->push(make<Word>(rawWord, word->offset()));
            } else if(word->string()->hasPrefix("&"_gfx)) {
                auto rawWord = word->string()->substring(Range(1, word->string()->length() - 1));
                this->evalExpression(make<Word>(rawWord, word->offset()), EvalContext::Vector);
            } else {
                auto value = currentFrame->bindingValue(word->string());
                if(!value) value = mUnboundWordHandler(word);
                if(!value) failForUnboundWord(word);
                
                if(context != EvalContext::Vector && value->isKindOfClass<Function>())
                    static_cast<Function *>(value)->apply(currentFrame);
                else
                    currentFrame->push(value);
            }
        } else if(part->isKindOfClass<String>() || part->isKindOfClass<Number>()) {
            currentFrame->push(part);
        } else if(part->isKindOfClass<Expression>()) {
            Expression *expression = static_cast<Expression *>(part);
            
            if(expression->type() == Expression::Type::Vector) {
                auto vector = make<Array<Base>>();
                for (Base *subexpression : expression->subexpressions()) {
                    this->evalExpression(subexpression, EvalContext::Vector);
                    vector->append(currentFrame->pop());
                }
                
                currentFrame->push(vector);
            } else if(expression->type() == Expression::Type::Function) {
                currentFrame->push(make<InterpretedFunction>(expression));
            }
        }
    }
    
    void Interpreter::eval(const Array<Base> *expressions, EvalContext context)
    {
        if(!expressions)
            return;
        
        for (Base *expression : expressions) {
            this->evalExpression(expression, context);
        }
    }
    
#pragma mark - Resetting
    
    void Interpreter::reset()
    {
#if GFX_Include_GraphicsStack
        Context::emptyContextStack();
#endif /* GFX_Include_GraphicsStack */
        
        released(mRootFrame);
        mRootFrame = CoreFunctions::createCoreFunctionFrame(this);
        this->pushFrame(mRootFrame);
        
        mRunningFunctions->removeAll();
        
        ResetSignal(this);
    }
    
#pragma mark - Unbound Word Handling
    
    void Interpreter::setUnboundWordHandler(UnboundWordHandler handler)
    {
        mUnboundWordHandler = handler;
    }
    
    Interpreter::UnboundWordHandler Interpreter::unboundWordHandler() const
    {
        return mUnboundWordHandler;
    }
    
    void Interpreter::failForUnboundWord(const Word *word)
    {
        fail((String::Builder() << "unbound word '" << word << "'"), word->offset());
    }
    
#pragma mark - Stack Frames
    
    void Interpreter::pushFrame(StackFrame *frame)
    {
        mFrames.push(retained(frame));
    }
    
    void Interpreter::popFrame()
    {
        if(mFrames.empty())
            throw Exception("Frame stack underflow."_gfx, nullptr);
        
        mFrames.top()->autorelease();
        mFrames.pop();
    }
    
    StackFrame *Interpreter::currentFrame() const
    {
        return mFrames.top();
    }
    
    Base *Interpreter::lastValue() const
    {
        if(this->currentFrame()->empty())
            return nullptr;
        else
            return this->currentFrame()->peak();
    }
    
#pragma mark - Backtrace Tracking
    
    void Interpreter::enteredFunction(const Function *function)
    {
        mRunningFunctions->append(function);
    }
    
    void Interpreter::exitedFunction(const Function *function)
    {
        mRunningFunctions->removeLast();
    }
    const String *Interpreter::backtrace() const
    {
        String::Builder backtrace;
        
        backtrace << "-- backtrace for " << this << " --\n";
        for (Index index = mRunningFunctions->count() - 1; index >= 0; index--) {
            backtrace << mRunningFunctions->at(index) << "\n";
        }
        backtrace << "-- end backtrace --";
        
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
                "cannot remove search path that was never in search paths array"_gfx);
        
        mSearchPaths->removeAt(mSearchPaths->firstIndexOf(Range(0, mSearchPaths->count()), searchPath));
    }
    
#pragma mark -
    
    bool Interpreter::import(const String *filename)
    {
        gfx_assert_param(filename);
        
        if(!this->isImportAllowed())
            throw Exception("illegal import"_gfx, nullptr);
        
        if(FilePaths::pathExtension(filename)->length() == 0)
            filename = String::Builder() << filename << ".gfx";
        
        AutoreleasePool pool;
        for (const String *searchPath : this->searchPaths()) {
            const String *path = FilePaths::combinePaths(searchPath, filename);
            if(File::exists(path)) {
                try {
                    const String *source = File::readFileAtPath(filename);
                    this->eval(Parser(source).parse());
                } catch (Exception e) {
                    return false;
                }
                
                return true;
            }
        }
        
        return false;
    }
}
