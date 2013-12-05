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
#include "papertape.h"
#include "file.h"

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
    
    static void backtrace(StackFrame *frame)
    {
        PaperTape::WriteLine(frame->interpreter()->backtrace());
    }
    
    static void reset(StackFrame *frame)
    {
        PaperTape::WriteLine("Resetting..."_gfx);
        frame->interpreter()->reset();
    }
    
#pragma mark - Core Functions
    
#if GFX_Language_SupportsImport
    static void import(StackFrame *frame)
    {
        String *filename = frame->popString();
        if(frame->interpreter()->import(filename))
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
        static_cast<Function *>(frame->pop())->apply(frame);
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
            frame->setBindingToValue("__exception"_gfx, const_cast<String *>(e.reason()));
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
                Word *word = static_cast<Word *>(words->at(index));
                frame->setBindingToValue(word->string(), frame->pop(), false);
            }
        } else if(wordOrWords && wordOrWords->isKindOfClass<Word>()) {
            Base *value = frame->pop();
            frame->setBindingToValue(static_cast<Word *>(wordOrWords)->string(), value, false);
        } else {
            gfx_assert(false, "bind is being used incorrectly"_gfx);
        }
    }
    
    static void set(StackFrame *frame)
    {
        Word *word = frame->popType<Word>();
        Base *value = frame->pop();
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
        vector->iterate([frame, function](Base *value, Index index, bool *stop) {
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
        const Array<Base> *result = vector->filter([frame, function](Base *value, Index index, bool *stop) {
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
        const Array<Base> *result = vector->map([frame, function](Base *value, Index index, bool *stop) -> Base * {
            frame->push(value);
            function->apply(frame);
            return frame->pop();
        });
        frame->push(const_cast<Array<Base> *>(result));
    }
    
#pragma mark - Hash Functions
    
    
    static Base *const kHashThunk = new Base();
    static void hash_begin(StackFrame *frame)
    {
        /* -- HashThunk */
        frame->push(kHashThunk);
    }
    
    static void hash_end(StackFrame *frame)
    {
        /* HashThunk ... -- hash */
        Dictionary<Base, Base> *hash = make<Dictionary<Base, Base>>();
        for (;;) {
            Base *value = frame->pop();
            if(value == kHashThunk)
                break;
            
            Base *key = frame->pop();
            gfx_assert(key != kHashThunk, "odd number keys in hash"_gfx);
            
            hash->set(key, value);
        }
        
        frame->push(hash);
    }
    
    static void hash_get(StackFrame *frame)
    {
        /* hash val -- val */
        Base *key = frame->pop();
        Dictionary<Base, Base> *hash = frame->popType<Dictionary<Base, Base>>();
        frame->push(hash->get(key));
    }
    
    static void hash_concat(StackFrame *frame)
    {
        Dictionary<Base, Base> *hash2 = frame->popType<Dictionary<Base, Base>>();
        Dictionary<Base, Base> *hash1 = frame->popType<Dictionary<Base, Base>>();
        Dictionary<Base, Base> *newHash = make<Dictionary<Base, Base>>();
        newHash->takeValuesFrom(hash1);
        newHash->takeValuesFrom(hash2);
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
    
#if GFX_Language_SupportsFiles
#pragma mark - File Operations
    
    static void file_exists(StackFrame *frame)
    {
        /* str -- bool */
        String *path = frame->popString();
        if(File::exists(path))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
    static void file_isDirectory(StackFrame *frame)
    {
        /* str -- bool */
        String *path = frame->popString();
        if(File::isDirectory(path))
            frame->push(Number::True());
        else
            frame->push(Number::False());
    }
    
    static void file_open(StackFrame *frame)
    {
        /* str -- file */
        String *path = frame->popString();
        frame->push(make<File>(path, File::Mode::ReadWrite));
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
    
#endif /* GFX_Language_SupportsFiles */
    
#pragma mark - Public API
    
    void CoreFunctions::createVariableBinding(StackFrame *frame, const String *name, Base *value)
    {
        frame->setBindingToValue(name, value, false);
    }
    
    void CoreFunctions::createFunctionBinding(StackFrame *frame, const String *name, std::function<void(StackFrame *stack)> implementation)
    {
        frame->setBindingToValue(name, make<NativeFunction>(name, implementation), false);
    }
    
    void CoreFunctions::addTo(StackFrame *frame)
    {
        //Core Constants
        CoreFunctions::createVariableBinding(frame, "true"_gfx, make<Number>(1));
        CoreFunctions::createVariableBinding(frame, "false"_gfx, make<Number>(0));
        CoreFunctions::createVariableBinding(frame, "null"_gfx, make<Number>(0));
        
        //Math Constants
        CoreFunctions::createVariableBinding(frame, "num/min"_gfx, Number::Minimum());
        CoreFunctions::createVariableBinding(frame, "num/max"_gfx, Number::Maximum());
        
        CoreFunctions::createVariableBinding(frame, "math/E"_gfx, make<Number>(M_E));
        CoreFunctions::createVariableBinding(frame, "math/LOG2E"_gfx, make<Number>(M_LOG2E));
        CoreFunctions::createVariableBinding(frame, "math/LOG10E"_gfx, make<Number>(M_LOG10E));
        CoreFunctions::createVariableBinding(frame, "math/LN2"_gfx, make<Number>(M_LN2));
        CoreFunctions::createVariableBinding(frame, "math/LN10"_gfx, make<Number>(M_LN10));
        CoreFunctions::createVariableBinding(frame, "math/PI"_gfx, make<Number>(M_PI));
        CoreFunctions::createVariableBinding(frame, "math/PI2"_gfx, make<Number>(M_PI_2));
        CoreFunctions::createVariableBinding(frame, "math/1PI"_gfx, make<Number>(M_1_PI));
        CoreFunctions::createVariableBinding(frame, "math/2PI"_gfx, make<Number>(M_2_PI));
        CoreFunctions::createVariableBinding(frame, "math/2SQRTPI"_gfx, make<Number>(M_2_SQRTPI));
        CoreFunctions::createVariableBinding(frame, "math/SQRT2"_gfx, make<Number>(M_SQRT2));
        CoreFunctions::createVariableBinding(frame, "math/SQRT1_2"_gfx, make<Number>(M_SQRT1_2));
        
        //File Constants
        CoreFunctions::createVariableBinding(frame, "file/out"_gfx, File::consoleOut());
        CoreFunctions::createVariableBinding(frame, "file/in"_gfx, File::consoleIn());
        CoreFunctions::createVariableBinding(frame, "file/err"_gfx, File::consoleError());
        
        
        //Math Operations
        CoreFunctions::createFunctionBinding(frame, "+"_gfx, &opPlus);
        CoreFunctions::createFunctionBinding(frame, "-"_gfx, &opMinus);
        CoreFunctions::createFunctionBinding(frame, "*"_gfx, &opTimes);
        CoreFunctions::createFunctionBinding(frame, "/"_gfx, &opDivide);
        CoreFunctions::createFunctionBinding(frame, "^"_gfx, &opPow);
        
        CoreFunctions::createFunctionBinding(frame, "math/cos"_gfx, &cosWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/sin"_gfx, &sinWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/tan"_gfx, &tanWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/acos"_gfx, &acosWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/asin"_gfx, &asinWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/atan"_gfx, &atanWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/atan2"_gfx, &atan2Wrapper);
        
        CoreFunctions::createFunctionBinding(frame, "math/cosh"_gfx, &coshWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/sinh"_gfx, &sinhWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/tanh"_gfx, &tanhWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/acosh"_gfx, &acoshWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/asinh"_gfx, &asinhWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/atanh"_gfx, &atanhWrapper);
        
        CoreFunctions::createFunctionBinding(frame, "math/exp"_gfx, &expWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/log"_gfx, &logWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/log10"_gfx, &log10Wrapper);
        
        CoreFunctions::createFunctionBinding(frame, "math/sqrt"_gfx, &sqrtWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/cbrt"_gfx, &cbrtWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/hypot"_gfx, &hypotWrapper);
        
        CoreFunctions::createFunctionBinding(frame, "math/abs"_gfx, &absWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/ceil"_gfx, &ceilWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/floor"_gfx, &floorWrapper);
        CoreFunctions::createFunctionBinding(frame, "math/round"_gfx, &roundWrapper);
        
        
        //Boolean Operations
        CoreFunctions::createFunctionBinding(frame, "&"_gfx, &opAnd);
        CoreFunctions::createFunctionBinding(frame, "|"_gfx, &opOr);
        CoreFunctions::createFunctionBinding(frame, "!"_gfx, &opNot);
        
        CoreFunctions::createFunctionBinding(frame, "="_gfx, &opEqual);
        CoreFunctions::createFunctionBinding(frame, "!="_gfx, &opNotEqual);
        CoreFunctions::createFunctionBinding(frame, "<"_gfx, &opLessThan);
        CoreFunctions::createFunctionBinding(frame, "<="_gfx, &opLessThanOrEqual);
        CoreFunctions::createFunctionBinding(frame, ">"_gfx, &opGreaterThan);
        CoreFunctions::createFunctionBinding(frame, ">="_gfx, &opGreaterThanOrEqual);
        
        
        //Stack Operations
        CoreFunctions::createFunctionBinding(frame, "rt/dup"_gfx, &dup);
        CoreFunctions::createFunctionBinding(frame, "rt/swap"_gfx, &swap);
        CoreFunctions::createFunctionBinding(frame, "rt/drop"_gfx, &drop);
        CoreFunctions::createFunctionBinding(frame, "rt/clear"_gfx, &clear);
        CoreFunctions::createFunctionBinding(frame, "rt/showstack"_gfx, &showstack);
        CoreFunctions::createFunctionBinding(frame, "rt/backtrace"_gfx, &backtrace);
        CoreFunctions::createFunctionBinding(frame, "rt/reset"_gfx, &reset);
        
        
        //Core Functions
#if GFX_Language_SupportsImport
        CoreFunctions::createFunctionBinding(frame, "import"_gfx, &import);
#endif /* GFX_Language_SupportsImport */
        
        CoreFunctions::createFunctionBinding(frame, "print"_gfx, &print);
        CoreFunctions::createFunctionBinding(frame, "read"_gfx, &read);
        CoreFunctions::createFunctionBinding(frame, "->str"_gfx, &toString);
        
        CoreFunctions::createFunctionBinding(frame, "if"_gfx, &_if);
        CoreFunctions::createFunctionBinding(frame, "ifelse"_gfx, &ifelse);
        CoreFunctions::createFunctionBinding(frame, "while"_gfx, &_while);
        CoreFunctions::createFunctionBinding(frame, "times"_gfx, &times);
        CoreFunctions::createFunctionBinding(frame, "fn/apply"_gfx, &apply);
        
        CoreFunctions::createFunctionBinding(frame, "throw"_gfx, &_throw);
        CoreFunctions::createFunctionBinding(frame, "rescue"_gfx, &rescue);
        
        CoreFunctions::createFunctionBinding(frame, "->void"_gfx, &drop);
        CoreFunctions::createFunctionBinding(frame, "let"_gfx, &bind);
        CoreFunctions::createFunctionBinding(frame, "set!"_gfx, &set);
        CoreFunctions::createFunctionBinding(frame, "destruct!"_gfx, &destructure);
        
        
        //String Functions
        CoreFunctions::createFunctionBinding(frame, "str/eq"_gfx, &str_eq);
        CoreFunctions::createFunctionBinding(frame, "str/compare"_gfx, &str_compare);
        CoreFunctions::createFunctionBinding(frame, "str/contains"_gfx, &str_contains);
        CoreFunctions::createFunctionBinding(frame, "str/starts-with"_gfx, &str_startsWith);
        CoreFunctions::createFunctionBinding(frame, "str/ends-with"_gfx, &str_endsWith);
        
        CoreFunctions::createFunctionBinding(frame, "str/char-at"_gfx, &str_charAt);
        CoreFunctions::createFunctionBinding(frame, "str/index-of"_gfx, &str_indexOf);
        
        CoreFunctions::createFunctionBinding(frame, "str/concat"_gfx, &str_concat);
        CoreFunctions::createFunctionBinding(frame, "str/replace"_gfx, &str_replace);
        CoreFunctions::createFunctionBinding(frame, "str/substr"_gfx, &str_substr);
        CoreFunctions::createFunctionBinding(frame, "str/split"_gfx, &str_split);
        CoreFunctions::createFunctionBinding(frame, "str/lower-case"_gfx, &str_lowerCase);
        CoreFunctions::createFunctionBinding(frame, "str/upper-case"_gfx, &str_upperCase);
        CoreFunctions::createFunctionBinding(frame, "str/capital-case"_gfx, &str_capitalCase);
        
        
        //Vector Functions
        CoreFunctions::createFunctionBinding(frame, "vec/at"_gfx, &vec_at);
        CoreFunctions::createFunctionBinding(frame, "vec/concat"_gfx, &vec_concat);
        CoreFunctions::createFunctionBinding(frame, "vec/index-of"_gfx, &vec_indexOf);
        CoreFunctions::createFunctionBinding(frame, "vec/last-index-of"_gfx, &vec_lastIndexOf);
        CoreFunctions::createFunctionBinding(frame, "vec/join"_gfx, &vec_join);
        CoreFunctions::createFunctionBinding(frame, "vec/subset"_gfx, &vec_subset);
        CoreFunctions::createFunctionBinding(frame, "vec/sort"_gfx, &vec_sort);
        CoreFunctions::createFunctionBinding(frame, "vec/for-each"_gfx, &vec_forEach);
        CoreFunctions::createFunctionBinding(frame, "vec/filter"_gfx, &vec_filter);
        CoreFunctions::createFunctionBinding(frame, "vec/map"_gfx, &vec_map);
        
        
        //Hash Functions
        CoreFunctions::createFunctionBinding(frame, "hash/begin"_gfx, &hash_begin);
        CoreFunctions::createFunctionBinding(frame, "hash/end"_gfx, &hash_end);
        CoreFunctions::createFunctionBinding(frame, "hash/get"_gfx, &hash_get);
        CoreFunctions::createFunctionBinding(frame, "hash/concat"_gfx, &hash_concat);
        CoreFunctions::createFunctionBinding(frame, "hash/each-pair"_gfx, &hash_eachPair);
        
        
#if GFX_Language_SupportsFiles
        //File Functions
        CoreFunctions::createFunctionBinding(frame, "file/exists?"_gfx, &file_exists);
        CoreFunctions::createFunctionBinding(frame, "file/dir?"_gfx, &file_isDirectory);
        CoreFunctions::createFunctionBinding(frame, "file/open"_gfx, &file_open);
        CoreFunctions::createFunctionBinding(frame, "file/close"_gfx, &file_close);
        CoreFunctions::createFunctionBinding(frame, "file/size"_gfx, &file_size);
        CoreFunctions::createFunctionBinding(frame, "file/seek"_gfx, &file_seek);
        CoreFunctions::createFunctionBinding(frame, "file/tell"_gfx, &file_tell);
        CoreFunctions::createFunctionBinding(frame, "file/read"_gfx, &file_read);
        CoreFunctions::createFunctionBinding(frame, "file/read-line"_gfx, &file_readLine);
        CoreFunctions::createFunctionBinding(frame, "file/write"_gfx, &file_write);
        CoreFunctions::createFunctionBinding(frame, "file/write-line"_gfx, &file_writeLine);
#endif /* GFX_Language_SupportsFiles */
    }
    
}