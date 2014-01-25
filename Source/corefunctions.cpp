//
//  corefunctions.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "corefunctions.h"

#include "interpreter.h"
#include "stackframe.h"
#include "function.h"
#include "word.h"
#include "str.h"
#include "number.h"
#include "null.h"
#include "papertape.h"
#include "filepolicy.h"
#include "type.h"

#include "gfx_defines.h"

namespace gfx {
    
#pragma mark - Math Operations
    
#   define SYNTHESIZE_MATH_WRAPPER_1_PARAM(FunctionName) static void FunctionName##Wrapper(StackFrame *frame) { \
    Number *input = frame->popNumber(); \
    Number *result = make<Number>(FunctionName(input->value())); \
    frame->push(result); \
}
#   define SYNTHESIZE_MATH_WRAPPER_2_PARAM(FunctionName) static void FunctionName##Wrapper(StackFrame *frame) { \
    Number *input1 = frame->popNumber(); \
    Number *input2 = frame->popNumber(); \
    Number *result = make<Number>(FunctionName(input1->value(), input2->value())); \
    frame->push(result); \
}
    
    static void opPlus(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() + right->value());
        frame->push(result);
    }
    
    static void opMinus(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() - right->value());
        frame->push(result);
    }
    
    static void opTimes(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() * right->value());
        frame->push(result);
    }
    
    static void opDivide(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() / right->value());
        frame->push(result);
    }
    
    static void opPow(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(pow(left->value(), right->value()));
        frame->push(result);
    }
    
    
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(cos)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(sin)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(tan)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(acos)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(asin)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(atan)
    SYNTHESIZE_MATH_WRAPPER_2_PARAM(atan2)
    
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(cosh)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(sinh)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(tanh)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(acosh)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(asinh)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(atanh)
    
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(exp)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(log)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(log10)
    
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(sqrt)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(cbrt)
    SYNTHESIZE_MATH_WRAPPER_2_PARAM(hypot)
    
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(ceil)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(floor)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(round)
    SYNTHESIZE_MATH_WRAPPER_1_PARAM(abs)
    
#pragma mark - Boolean Operations
    
    static void opAnd(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() && right->value());
        frame->push(result);
    }
    
    static void opOr(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() || right->value());
        frame->push(result);
    }
    
    static void opNot(StackFrame *frame)
    {
        Number *value = frame->popNumber();
        Number *result = make<Number>(!value->value());
        frame->push(result);
    }
    
#pragma mark -
    
    static void opEqual(StackFrame *frame)
    {
        Base *left = frame->pop();
        Base *right = frame->pop();
        Number *result = make<Number>(left->isEqual(right));
        frame->push(result);
    }
    
    static void opNotEqual(StackFrame *frame)
    {
        Base *left = frame->pop();
        Base *right = frame->pop();
        Number *result = make<Number>(!left->isEqual(right));
        frame->push(result);
    }
    
    static void opLessThan(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() < right->value());
        frame->push(result);
    }
    
    static void opLessThanOrEqual(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() <= right->value());
        frame->push(result);
    }
    
    static void opGreaterThan(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() > right->value());
        frame->push(result);
    }
    
    static void opGreaterThanOrEqual(StackFrame *frame)
    {
        Number *right = frame->popNumber();
        Number *left = frame->popNumber();
        Number *result = make<Number>(left->value() >= right->value());
        frame->push(result);
    }
    
#pragma mark - Stack Operations
    
    static void dup(StackFrame *frame)
    {
        frame->push(frame->peak());
    }
    
    static void swap(StackFrame *frame)
    {
        Base *a = frame->pop();
        Base *b = frame->pop();
        
        frame->push(b);
        frame->push(a);
    }
    
    static void drop(StackFrame *frame)
    {
        frame->pop();
    }
    
    static void clear(StackFrame *frame)
    {
        while (!frame->empty())
            frame->pop();
    }
    
    static void showstack(StackFrame *frame)
    {
        String::Builder stackDescription;
        stackDescription << "-- stack has " << frame->depth() << " items --\n";
        
        frame->iterate([&stackDescription](Base *value, Index index, bool *stop) {
            stackDescription << value;
            stackDescription << "\n";
        });
        
        stackDescription << "-- end stack --";
        
        PaperTape::WriteLine(stackDescription);
    }
    
#pragma mark - Core Functions
    
    ///A wrapper around the `typeid` core construct that special cases
    ///subtypes of `gfx::Function` to always refer to the root type.
#define BASE__TYPEID(obj) (obj->isKindOfClass<Function>()? typeid(Function) : typeid(*obj))
    
    static void type_of(StackFrame *frame)
    {
        /* val -- type */
        
        auto value = frame->pop();
        
        auto typeMap = frame->interpreter()->typeResolutionMap();
        auto valueType = typeMap->lookupType(BASE__TYPEID(value));
        frame->push(const_cast<Type *>(valueType));
    }
    
    static void is_a(StackFrame *frame)
    {
        /* val type -- bool */
        
        auto type = frame->popType<Type>();
        auto value = frame->pop();
        
        auto typeMap = frame->interpreter()->typeResolutionMap();
        auto valueType = typeMap->lookupType(BASE__TYPEID(value));
        if(valueType && valueType->isKindOf(type)) {
            frame->push(Number::True());
        } else {
            frame->push(Number::False());
        }
    }
    
#undef BASE__TYPEID
    
#if GFX_Language_SupportsImport
    static void import(StackFrame *frame)
    {
        String *filename = frame->popString();
        if(frame->interpreter()->import(frame, filename))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
#endif /* GFX_Language_SupportsImport */
    
#pragma mark -
    
    static void print(StackFrame *frame)
    {
        PaperTape::WriteLine(frame->pop()->description());
    }
    
    static void read(StackFrame *frame)
    {
        frame->push(const_cast<String *>(PaperTape::ReadLine()));
    }
    
    static void toString(StackFrame *frame)
    {
        /* val -- str */
        frame->push(const_cast<String *>(frame->pop()->description()));
    }
    
#pragma mark -
    
    static void _if(StackFrame *frame)
    {
        Function *trueFunction = static_cast<Function *>(frame->pop());
        Base *condition = frame->pop();
        if(Interpreter::IsTrue(condition)) {
            trueFunction->apply(frame);
        }
    }
    
    static void ifelse(StackFrame *frame)
    {
        Function *falseFunction = frame->popFunction();
        Function *trueFunction = frame->popFunction();
        Base *condition = frame->pop();
        if(Interpreter::IsTrue(condition)) {
            trueFunction->apply(frame);
        } else {
            falseFunction->apply(frame);
        }
    }
    
    static void _while(StackFrame *frame)
    {
        Function *body = frame->popFunction();
        Function *condition = frame->popFunction();
        
        condition->apply(frame);
        while (Interpreter::IsTrue(frame->pop())) {
            body->apply(frame);
            condition->apply(frame);
        }
    }
    
    static void times(StackFrame *frame)
    {
        Number *count = frame->popNumber();
        Function *function = frame->popFunction();
        for (long i = 0, c = round(count->value()); i < c; i++) {
            frame->push(make<Number>(i));
            function->apply(frame);
            frame->safeDrop();
        }
    }
    
    static void apply(StackFrame *frame)
    {
        frame->popFunction()->apply(frame);
    }
    
    static void recurse(StackFrame *frame)
    {
        /* -- */
        
        throw RecursionMarkerException();
    }
    
#pragma mark -
    
    static void _throw(StackFrame *frame)
    {
        String *reason = frame->popString();
        throw Exception(reason, nullptr);
    }
    
    static void rescue(StackFrame *frame)
    {
        /* func func -- */
        Function *catchFunction = frame->popFunction();
        Function *tryFunction = frame->popFunction();
        try {
            tryFunction->apply(frame);
        } catch (gfx::Exception e) {
            frame->setBindingToValue(str("__exception"), const_cast<String *>(e.reason()));
            catchFunction->apply(frame);
        }
    }
    
#pragma mark -
    
    static void bind(StackFrame *frame)
    {
        Base *wordOrWords = frame->pop();
        if(wordOrWords && wordOrWords->isKindOfClass<Array<Base>>()) {
            auto words = static_cast<Array<Base> *>(wordOrWords);
            for (Index index = words->count() - 1; index >= 0; index--) {
                Word *word = dynamic_cast_or_throw<Word *>(words->at(index));
                frame->setBindingToValue(word->string(), frame->pop(), false);
            }
        } else if(wordOrWords && wordOrWords->isKindOfClass<Word>()) {
            auto wordString = static_cast<Word *>(wordOrWords)->string();
            if(wordString->find(str("."), Range(0, wordString->length()))) {
                throw Exception(str("Dot-syntax is only supported for lookup, cannot use with let."), nullptr);
            }
            
            Base *value = frame->pop();
            frame->setBindingToValue(wordString, value, false);
        } else {
            gfx_assert(false, str("bind is being used incorrectly"));
        }
    }
    
    static void set(StackFrame *frame)
    {
        auto word = frame->popType<Word>();
        auto wordString = word->string();
        if(wordString->find(str("."), Range(0, wordString->length()))) {
            throw Exception(str("Dot-syntax is only supported for lookup, cannot use with set."), nullptr);
        }
        
        auto value = frame->pop();
        frame->setBindingToValue(word->string(), value);
    }
    
    static void define(StackFrame *frame)
    {
        auto value = frame->pop();
        auto word = frame->popType<Word>();
        auto wordString = word->string();
        if(wordString->find(str("."), Range(0, wordString->length()))) {
            throw Exception(str("Dot-syntax is only supported for lookup, cannot use with set."), nullptr);
        }
        
        frame->setBindingToValue(word->string(), value);
    }
    
    static void destructure(StackFrame *frame)
    {
        auto vector = frame->popType<Array<Base>>();
        for (Base *value : vector)
            frame->push(value);
    }
    
#pragma mark - String Functions
    
    static void str_eq(StackFrame *frame)
    {
        /* str str -- bool */
        String *right = frame->popString();
        String *left = frame->popString();
        if(left->isEqual(right))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
    static void str_compare(StackFrame *frame)
    {
        /* str str -- num */
        String *right = frame->popString();
        String *left = frame->popString();
        CFComparisonResult result = left->compare(right, Range(0, left->length()));
        frame->push(make<Number>(result));
    }
    
    static void str_contains(StackFrame *frame)
    {
        /* str haystack str needle -- bool */
        String *needle = frame->popString();
        String *haystack = frame->popString();
        Range rangeOfString = haystack->find(needle, Range(0, haystack->length()));
        if(rangeOfString.location != kCFNotFound)
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
    static void str_startsWith(StackFrame *frame)
    {
        /* str haystack str needle -- bool */
        String *needle = frame->popString();
        String *haystack = frame->popString();
        if(haystack->hasPrefix(needle))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
    static void str_endsWith(StackFrame *frame)
    {
        /* str haystack str needle -- bool */
        String *needle = frame->popString();
        String *haystack = frame->popString();
        if(haystack->hasSuffix(needle))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
#pragma mark -
    
    static void str_charAt(StackFrame *frame)
    {
        /* str num -- num */
        Number *offset = frame->popNumber();
        String *string = frame->popString();
        Number *charCode = make<Number>(string->at(offset->value()));
        frame->push(charCode);
    }
    
    static void str_indexOf(StackFrame *frame)
    {
        /* str haystack str needle -- int */
        String *needle = frame->popString();
        String *haystack = frame->popString();
        Range rangeOfString = haystack->find(needle, Range(0, haystack->length()));
        frame->push(make<Number>(rangeOfString.location));
    }
    
#pragma mark -
    
    static void str_concat(StackFrame *frame)
    {
        /* str1 str2 -- str */
        String *string2 = frame->popString();
        String *string1 = frame->popString();
        frame->push((String::Builder() << string1 << string2));
    }
    
    static void str_replace(StackFrame *frame)
    {
        /* str target str toFind str toReplace -- bool */
        String *toReplace = frame->popString();
        String *toFind = frame->popString();
        String *target = frame->popString();
        String *targetCopy = make<String>(target);
        targetCopy->findAndReplace(toFind, toReplace, Range(0, targetCopy->length()));
        frame->push(targetCopy);
    }
    
    static void str_substr(StackFrame *frame)
    {
        /* str num num -- str */
        Number *length = frame->popNumber();
        Number *location = frame->popNumber();
        String *haystack = frame->popString();
        String *substring = haystack->substring(Range(location->value(), length->value()));
        frame->push(substring);
    }
    
    static void str_split(StackFrame *frame)
    {
        /* str target str deliminator -- bool */
        String *deliminator = frame->popString();
        String *target = frame->popString();
        frame->push(SplitString(target, deliminator));
    }
    
    static void str_lowerCase(StackFrame *frame)
    {
        /* str -- str */
        String *string = frame->popString();
        frame->push(string->lowercasedString());
    }
    
    static void str_upperCase(StackFrame *frame)
    {
        /* str -- str */
        String *string = frame->popString();
        frame->push(string->uppercasedString());
    }
    
    static void str_capitalCase(StackFrame *frame)
    {
        /* str -- str */
        String *string = frame->popString();
        frame->push(string->capitalizedString());
    }
    
#pragma mark - Vector Functions
    
    static void vec_at(StackFrame *frame)
    {
        /* vec num -- val */
        Number *index = frame->popNumber();
        Array<Base> *vector = frame->popType<Array<Base>>();
        Base *value = vector->at(index->value());
        frame->push(value);
    }
    
    static void vec_concat(StackFrame *frame)
    {
        /* vec vec -- vec */
        Array<Base> *vector2 = frame->popType<Array<Base>>();
        Array<Base> *vector1 = frame->popType<Array<Base>>();
        Array<Base> *newVector = make<Array<Base>>();
        newVector->appendArray(vector1);
        newVector->appendArray(vector2);
        frame->push(newVector);
    }
    
    static void vec_indexOf(StackFrame *frame)
    {
        /* vec val -- num */
        Base *value = frame->pop();
        Array<Base> *vector = frame->popType<Array<Base>>();
        frame->push(make<Number>(vector->firstIndexOf(Range(0, vector->count()), value)));
    }
    
    static void vec_lastIndexOf(StackFrame *frame)
    {
        /* vec val -- num */
        Base *value = frame->pop();
        Array<Base> *vector = frame->popType<Array<Base>>();
        frame->push(make<Number>(vector->lastIndexOf(Range(0, vector->count()), value)));
    }
    
    static void vec_join(StackFrame *frame)
    {
        /* vec str -- str */
        String *deliminator = frame->popString();
        Array<Base> *vector = frame->popType<Array<Base>>();
        frame->push(JoinArray(vector, deliminator));
    }
    
    static void vec_subset(StackFrame *frame)
    {
        /* vec num num -- vec */
        Number *length = frame->popNumber();
        Number *location = frame->popNumber();
        Array<Base> *vector = frame->popType<Array<Base>>();
        Array<Base> *subvector = vector->subarray(Range(location->value(), length->value()));
        frame->push(subvector);
    }
    
    static void vec_sort(StackFrame *frame)
    {
        /* vec func -- vec */
        Function *function = frame->popFunction();
        Array<Base> *vector = frame->popType<Array<Base>>();
        Array<Base> *newVector = copy(vector);
        newVector->sort(Range(0, vector->count()), [frame, function](Base *left, Base *right) -> CFComparisonResult {
            frame->push(left);
            frame->push(right);
            
            function->apply(frame);
            
            return (CFComparisonResult)frame->popNumber()->value();
        });
        frame->push(newVector);
    }
    
    static void vec_forEach(StackFrame *frame)
    {
        /* vec func -- */
        Function *function = frame->popFunction();
        Array<Base> *vector = frame->popType<Array<Base>>();
        vector->iterate(vector->all(), [frame, function](Base *value, Index index, bool *stop) {
            frame->push(value);
            function->apply(frame);
            frame->safeDrop();
        });
    }
    
    static void vec_filter(StackFrame *frame)
    {
        /* vec func -- vec */
        Function *function = frame->popFunction();
        Array<Base> *vector = frame->popType<Array<Base>>();
        const Array<Base> *result = vector->filter(vector->all(), [frame, function](Base *value, Index index, bool *stop) {
            frame->push(value);
            function->apply(frame);
            return (bool)frame->popNumber()->value();
        });
        frame->push(const_cast<Array<Base> *>(result));
    }
    
    static void vec_map(StackFrame *frame)
    {
        /* vec func -- vec */
        Function *function = frame->popFunction();
        Array<Base> *vector = frame->popType<Array<Base>>();
        const Array<Base> *result = vector->map(vector->all(), [frame, function](Base *value, Index index, bool *stop) -> Base * {
            frame->push(value);
            function->apply(frame);
            return frame->pop();
        });
        frame->push(const_cast<Array<Base> *>(result));
    }
    
#pragma mark - Hash Functions
    
    static void hash_get(StackFrame *frame)
    {
        /* hash val -- val */
        Base *key = frame->pop();
        Dictionary<Base, Base> *hash = frame->popType<Dictionary<Base, Base>>();
        frame->push(hash->get(key));
    }
    
    static void hash_concat(StackFrame *frame)
    {
        /* hash hash -- hash */
        Dictionary<Base, Base> *hash2 = frame->popType<Dictionary<Base, Base>>();
        Dictionary<Base, Base> *hash1 = frame->popType<Dictionary<Base, Base>>();
        Dictionary<Base, Base> *newHash = make<Dictionary<Base, Base>>();
        newHash->takeValuesFrom(hash1);
        newHash->takeValuesFrom(hash2);
        frame->push(newHash);
    }
    
    static void hash_without(StackFrame *frame)
    {
        /* hash vec|val -- hash */
        
        Base *vectorOrValue = frame->pop();
        Dictionary<Base, Base> *newHash = copy(frame->popType<Dictionary<Base, Base>>());
        
        if(vectorOrValue->isKindOfClass<Array<Base>>()) {
            auto keysToRemove = static_cast<Array<Base> *>(vectorOrValue);
            for (Base *key : keysToRemove)
                newHash->remove(key);
        } else {
            newHash->remove(vectorOrValue);
        }
        
        frame->push(newHash);
    }
    
    static void hash_eachPair(StackFrame *frame)
    {
        /* hash func -- */
        Function *function = frame->popFunction();
        Dictionary<Base, Base> *hash = frame->popType<Dictionary<Base, Base>>();
        hash->iterate([frame, function](Base *key, Base *value) {
            frame->push(key);
            frame->push(value);
            
            function->apply(frame);
            
            frame->safeDrop();
        });
    }
    
#pragma mark - File Operations
    
    static void file_exists(StackFrame *frame)
    {
        /* str -- bool */
        String *path = frame->popString();
        if(FilePolicy::ActiveFilePolicy()->pathExists(path))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
    static void file_isDirectory(StackFrame *frame)
    {
        /* str -- bool */
        String *path = frame->popString();
        if(FilePolicy::ActiveFilePolicy()->isPathDirectory(path))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
    static void file_open(StackFrame *frame)
    {
        /* str -- file */
        String *path = frame->popString();
        frame->push(FilePolicy::ActiveFilePolicy()->openFileAtPath(path, File::Mode::ReadWrite));
    }
    
    static void file_close(StackFrame *frame)
    {
        /* file -- */
        File *file = frame->popType<File>();
        file->close();
    }
    
    static void file_size(StackFrame *frame)
    {
        /* file -- num */
        File *file = frame->popType<File>();
        size_t length = file->length();
        frame->push(make<Number>(length));
    }
    
    static void file_seek(StackFrame *frame)
    {
        /* file -- */
        Number *location = frame->popNumber();
        File *file = frame->popType<File>();
        file->setPosition(location->value());
    }
    
    static void file_tell(StackFrame *frame)
    {
        /* file -- num */
        File *file = frame->popType<File>();
        frame->push(make<Number>(file->position()));
    }
    
    static void file_read(StackFrame *frame)
    {
        /* file num -- str */
        Number *amount = frame->popNumber();
        File *file = frame->popType<File>();
        frame->push(file->readString(amount->value()));
    }
    
    static void file_readLine(StackFrame *frame)
    {
        /* file -- str */
        File *file = frame->popType<File>();
        frame->push(file->readLine());
    }
    
    static void file_write(StackFrame *frame)
    {
        /* file str -- num */
        String *string = frame->popString();
        File *file = frame->popType<File>();
        size_t amountWritten = file->writeString(string);
        frame->push(make<Number>(amountWritten));
    }
    
    static void file_writeLine(StackFrame *frame)
    {
        /* file str -- num */
        String *string = frame->popString();
        File *file = frame->popType<File>();
        size_t amountWritten = file->writeLine(string);
        frame->push(make<Number>(amountWritten));
    }
    
#pragma mark - Public API
    
    void CoreFunctions::addTo(StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        AutoreleasePool pool;
        
        //Core Constants
        frame->createVariableBinding(str("true"), Number::True());
        frame->createVariableBinding(str("false"), Number::False());
        frame->createVariableBinding(str("null"), Null::shared());
        
        //Math Constants
        frame->createVariableBinding(str("num/min"), Number::Minimum());
        frame->createVariableBinding(str("num/max"), Number::Maximum());
        
        frame->createVariableBinding(str("math/E"), make<Number>(M_E));
        frame->createVariableBinding(str("math/LOG2E"), make<Number>(M_LOG2E));
        frame->createVariableBinding(str("math/LOG10E"), make<Number>(M_LOG10E));
        frame->createVariableBinding(str("math/LN2"), make<Number>(M_LN2));
        frame->createVariableBinding(str("math/LN10"), make<Number>(M_LN10));
        frame->createVariableBinding(str("math/PI"), make<Number>(M_PI));
        frame->createVariableBinding(str("math/PI2"), make<Number>(M_PI_2));
        frame->createVariableBinding(str("math/1PI"), make<Number>(M_1_PI));
        frame->createVariableBinding(str("math/2PI"), make<Number>(M_2_PI));
        frame->createVariableBinding(str("math/2SQRTPI"), make<Number>(M_2_SQRTPI));
        frame->createVariableBinding(str("math/SQRT2"), make<Number>(M_SQRT2));
        frame->createVariableBinding(str("math/SQRT1_2"), make<Number>(M_SQRT1_2));
        
        //File Constants
        frame->createVariableBinding(str("file/out"), File::consoleOut());
        frame->createVariableBinding(str("file/in"), File::consoleIn());
        frame->createVariableBinding(str("file/err"), File::consoleError());
        
        
        //Math Operations
        frame->createFunctionBinding(str("+"), &opPlus);
        frame->createFunctionBinding(str("-"), &opMinus);
        frame->createFunctionBinding(str("*"), &opTimes);
        frame->createFunctionBinding(str("/"), &opDivide);
        frame->createFunctionBinding(str("^"), &opPow);
        
        frame->createFunctionBinding(str("math/cos"), &cosWrapper);
        frame->createFunctionBinding(str("math/sin"), &sinWrapper);
        frame->createFunctionBinding(str("math/tan"), &tanWrapper);
        frame->createFunctionBinding(str("math/acos"), &acosWrapper);
        frame->createFunctionBinding(str("math/asin"), &asinWrapper);
        frame->createFunctionBinding(str("math/atan"), &atanWrapper);
        frame->createFunctionBinding(str("math/atan2"), &atan2Wrapper);
        
        frame->createFunctionBinding(str("math/cosh"), &coshWrapper);
        frame->createFunctionBinding(str("math/sinh"), &sinhWrapper);
        frame->createFunctionBinding(str("math/tanh"), &tanhWrapper);
        frame->createFunctionBinding(str("math/acosh"), &acoshWrapper);
        frame->createFunctionBinding(str("math/asinh"), &asinhWrapper);
        frame->createFunctionBinding(str("math/atanh"), &atanhWrapper);
        
        frame->createFunctionBinding(str("math/exp"), &expWrapper);
        frame->createFunctionBinding(str("math/log"), &logWrapper);
        frame->createFunctionBinding(str("math/log10"), &log10Wrapper);
        
        frame->createFunctionBinding(str("math/sqrt"), &sqrtWrapper);
        frame->createFunctionBinding(str("math/cbrt"), &cbrtWrapper);
        frame->createFunctionBinding(str("math/hypot"), &hypotWrapper);
        
        frame->createFunctionBinding(str("math/abs"), &absWrapper);
        frame->createFunctionBinding(str("math/ceil"), &ceilWrapper);
        frame->createFunctionBinding(str("math/floor"), &floorWrapper);
        frame->createFunctionBinding(str("math/round"), &roundWrapper);
        
        
        //Boolean Operations
        frame->createFunctionBinding(str("and"), &opAnd);
        frame->createFunctionBinding(str("or"), &opOr);
        frame->createFunctionBinding(str("not"), &opNot);
        
        frame->createFunctionBinding(str("="), &opEqual);
        frame->createFunctionBinding(str("!="), &opNotEqual);
        frame->createFunctionBinding(str("<"), &opLessThan);
        frame->createFunctionBinding(str("<="), &opLessThanOrEqual);
        frame->createFunctionBinding(str(">"), &opGreaterThan);
        frame->createFunctionBinding(str(">="), &opGreaterThanOrEqual);
        
        
        //Stack Operations
        frame->createFunctionBinding(str("__dup"), &dup);
        frame->createFunctionBinding(str("__swap"), &swap);
        frame->createFunctionBinding(str("__drop"), &drop);
        frame->createFunctionBinding(str("__clear"), &clear);
        frame->createFunctionBinding(str("__showstack"), &showstack);
        
        
        //Core Functions
        frame->createFunctionBinding(str("type-of"), &type_of);
        frame->createFunctionBinding(str("is-a?"), &is_a);
        
#if GFX_Language_SupportsImport
        frame->createFunctionBinding(str("import"), &import);
#endif /* GFX_Language_SupportsImport */
        
        frame->createFunctionBinding(str("print"), &print);
        frame->createFunctionBinding(str("read"), &read);
        frame->createFunctionBinding(str("->str"), &toString);
        
        frame->createFunctionBinding(str("if"), &_if);
        frame->createFunctionBinding(str("ifelse"), &ifelse);
        frame->createFunctionBinding(str("while"), &_while);
        frame->createFunctionBinding(str("times"), &times);
        frame->createFunctionBinding(str("fn/apply"), &apply);
        frame->createFunctionBinding(str("__recurse"), &recurse);
        
        frame->createFunctionBinding(str("throw"), &_throw);
        frame->createFunctionBinding(str("rescue"), &rescue);
        
        frame->createFunctionBinding(str("->void"), &drop);
        frame->createFunctionBinding(str("=>"), &bind);
        frame->createFunctionBinding(str("set!"), &set);
        frame->createFunctionBinding(str("def"), &define);
        frame->createFunctionBinding(str("destruct!"), &destructure);
        
        
        //String Functions
        frame->createFunctionBinding(str("str/eq"), &str_eq);
        frame->createFunctionBinding(str("str/compare"), &str_compare);
        frame->createFunctionBinding(str("str/contains"), &str_contains);
        frame->createFunctionBinding(str("str/starts-with"), &str_startsWith);
        frame->createFunctionBinding(str("str/ends-with"), &str_endsWith);
        
        frame->createFunctionBinding(str("str/char-at"), &str_charAt);
        frame->createFunctionBinding(str("str/index-of"), &str_indexOf);
        
        frame->createFunctionBinding(str("str/concat"), &str_concat);
        frame->createFunctionBinding(str("str/replace"), &str_replace);
        frame->createFunctionBinding(str("str/substr"), &str_substr);
        frame->createFunctionBinding(str("str/split"), &str_split);
        frame->createFunctionBinding(str("str/lower-case"), &str_lowerCase);
        frame->createFunctionBinding(str("str/upper-case"), &str_upperCase);
        frame->createFunctionBinding(str("str/capital-case"), &str_capitalCase);
        
        
        //Vector Functions
        frame->createFunctionBinding(str("vec/at"), &vec_at);
        frame->createFunctionBinding(str("vec/concat"), &vec_concat);
        frame->createFunctionBinding(str("vec/index-of"), &vec_indexOf);
        frame->createFunctionBinding(str("vec/last-index-of"), &vec_lastIndexOf);
        frame->createFunctionBinding(str("vec/join"), &vec_join);
        frame->createFunctionBinding(str("vec/subset"), &vec_subset);
        frame->createFunctionBinding(str("vec/sort"), &vec_sort);
        frame->createFunctionBinding(str("vec/for-each"), &vec_forEach);
        frame->createFunctionBinding(str("vec/filter"), &vec_filter);
        frame->createFunctionBinding(str("vec/map"), &vec_map);
        
        
        //Hash Functions
        frame->createFunctionBinding(str("hash/get"), &hash_get);
        frame->createFunctionBinding(str("hash/concat"), &hash_concat);
        frame->createFunctionBinding(str("hash/without"), &hash_without);
        frame->createFunctionBinding(str("hash/each-pair"), &hash_eachPair);
        
        
        //File Functions
        frame->createFunctionBinding(str("file/exists?"), &file_exists);
        frame->createFunctionBinding(str("file/dir?"), &file_isDirectory);
        frame->createFunctionBinding(str("file/open"), &file_open);
        frame->createFunctionBinding(str("file/close"), &file_close);
        frame->createFunctionBinding(str("file/size"), &file_size);
        frame->createFunctionBinding(str("file/seek"), &file_seek);
        frame->createFunctionBinding(str("file/tell"), &file_tell);
        frame->createFunctionBinding(str("file/read"), &file_read);
        frame->createFunctionBinding(str("file/read-line"), &file_readLine);
        frame->createFunctionBinding(str("file/write"), &file_write);
        frame->createFunctionBinding(str("file/write-line"), &file_writeLine);
    }
    
    StackFrame *CoreFunctions::sharedCoreFunctionFrame()
    {
        static StackFrame *sharedCoreFunctionFrame = nullptr;
        if(!sharedCoreFunctionFrame) {
            sharedCoreFunctionFrame = new StackFrame(nullptr, nullptr);
            CoreFunctions::addTo(sharedCoreFunctionFrame);
            sharedCoreFunctionFrame->freeze();
        }
        
        return sharedCoreFunctionFrame;
    }
    
    StackFrame *CoreFunctions::createCoreFunctionFrame(Interpreter *interpreter)
    {
        return make<StackFrame>(CoreFunctions::sharedCoreFunctionFrame(), interpreter);
    }
}