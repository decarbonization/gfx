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
        PaperTape::WriteLine(str("Resetting..."));
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
        frame->popFunction()->apply(frame);
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
                Word *word = static_cast<Word *>(words->at(index));
                frame->setBindingToValue(word->string(), frame->pop(), false);
            }
        } else if(wordOrWords && wordOrWords->isKindOfClass<Word>()) {
            Base *value = frame->pop();
            frame->setBindingToValue(static_cast<Word *>(wordOrWords)->string(), value, false);
        } else {
            gfx_assert(false, str("bind is being used incorrectly"));
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
            gfx_assert(key != kHashThunk, str("odd number keys in hash"));
            
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
        gfx_assert_param(frame);
        
        AutoreleasePool pool;
        
        //Core Constants
        CoreFunctions::createVariableBinding(frame, str("true"), make<Number>(1));
        CoreFunctions::createVariableBinding(frame, str("false"), make<Number>(0));
        CoreFunctions::createVariableBinding(frame, str("null"), make<Number>(0));
        
        //Math Constants
        CoreFunctions::createVariableBinding(frame, str("num/min"), Number::Minimum());
        CoreFunctions::createVariableBinding(frame, str("num/max"), Number::Maximum());
        
        CoreFunctions::createVariableBinding(frame, str("math/E"), make<Number>(M_E));
        CoreFunctions::createVariableBinding(frame, str("math/LOG2E"), make<Number>(M_LOG2E));
        CoreFunctions::createVariableBinding(frame, str("math/LOG10E"), make<Number>(M_LOG10E));
        CoreFunctions::createVariableBinding(frame, str("math/LN2"), make<Number>(M_LN2));
        CoreFunctions::createVariableBinding(frame, str("math/LN10"), make<Number>(M_LN10));
        CoreFunctions::createVariableBinding(frame, str("math/PI"), make<Number>(M_PI));
        CoreFunctions::createVariableBinding(frame, str("math/PI2"), make<Number>(M_PI_2));
        CoreFunctions::createVariableBinding(frame, str("math/1PI"), make<Number>(M_1_PI));
        CoreFunctions::createVariableBinding(frame, str("math/2PI"), make<Number>(M_2_PI));
        CoreFunctions::createVariableBinding(frame, str("math/2SQRTPI"), make<Number>(M_2_SQRTPI));
        CoreFunctions::createVariableBinding(frame, str("math/SQRT2"), make<Number>(M_SQRT2));
        CoreFunctions::createVariableBinding(frame, str("math/SQRT1_2"), make<Number>(M_SQRT1_2));
        
        //File Constants
        CoreFunctions::createVariableBinding(frame, str("file/out"), File::consoleOut());
        CoreFunctions::createVariableBinding(frame, str("file/in"), File::consoleIn());
        CoreFunctions::createVariableBinding(frame, str("file/err"), File::consoleError());
        
        
        //Math Operations
        CoreFunctions::createFunctionBinding(frame, str("+"), &opPlus);
        CoreFunctions::createFunctionBinding(frame, str("-"), &opMinus);
        CoreFunctions::createFunctionBinding(frame, str("*"), &opTimes);
        CoreFunctions::createFunctionBinding(frame, str("/"), &opDivide);
        CoreFunctions::createFunctionBinding(frame, str("^"), &opPow);
        
        CoreFunctions::createFunctionBinding(frame, str("math/cos"), &cosWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/sin"), &sinWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/tan"), &tanWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/acos"), &acosWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/asin"), &asinWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/atan"), &atanWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/atan2"), &atan2Wrapper);
        
        CoreFunctions::createFunctionBinding(frame, str("math/cosh"), &coshWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/sinh"), &sinhWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/tanh"), &tanhWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/acosh"), &acoshWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/asinh"), &asinhWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/atanh"), &atanhWrapper);
        
        CoreFunctions::createFunctionBinding(frame, str("math/exp"), &expWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/log"), &logWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/log10"), &log10Wrapper);
        
        CoreFunctions::createFunctionBinding(frame, str("math/sqrt"), &sqrtWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/cbrt"), &cbrtWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/hypot"), &hypotWrapper);
        
        CoreFunctions::createFunctionBinding(frame, str("math/abs"), &absWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/ceil"), &ceilWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/floor"), &floorWrapper);
        CoreFunctions::createFunctionBinding(frame, str("math/round"), &roundWrapper);
        
        
        //Boolean Operations
        CoreFunctions::createFunctionBinding(frame, str("and"), &opAnd);
        CoreFunctions::createFunctionBinding(frame, str("or"), &opOr);
        CoreFunctions::createFunctionBinding(frame, str("not"), &opNot);
        
        CoreFunctions::createFunctionBinding(frame, str("="), &opEqual);
        CoreFunctions::createFunctionBinding(frame, str("!="), &opNotEqual);
        CoreFunctions::createFunctionBinding(frame, str("<"), &opLessThan);
        CoreFunctions::createFunctionBinding(frame, str("<="), &opLessThanOrEqual);
        CoreFunctions::createFunctionBinding(frame, str(">"), &opGreaterThan);
        CoreFunctions::createFunctionBinding(frame, str(">="), &opGreaterThanOrEqual);
        
        
        //Stack Operations
        CoreFunctions::createFunctionBinding(frame, str("rt/dup"), &dup);
        CoreFunctions::createFunctionBinding(frame, str("rt/swap"), &swap);
        CoreFunctions::createFunctionBinding(frame, str("rt/drop"), &drop);
        CoreFunctions::createFunctionBinding(frame, str("rt/clear"), &clear);
        CoreFunctions::createFunctionBinding(frame, str("rt/showstack"), &showstack);
        CoreFunctions::createFunctionBinding(frame, str("rt/backtrace"), &backtrace);
        CoreFunctions::createFunctionBinding(frame, str("rt/reset"), &reset);
        
        
        //Core Functions
#if GFX_Language_SupportsImport
        CoreFunctions::createFunctionBinding(frame, str("import"), &import);
#endif /* GFX_Language_SupportsImport */
        
        CoreFunctions::createFunctionBinding(frame, str("print"), &print);
        CoreFunctions::createFunctionBinding(frame, str("read"), &read);
        CoreFunctions::createFunctionBinding(frame, str("->str"), &toString);
        
        CoreFunctions::createFunctionBinding(frame, str("if"), &_if);
        CoreFunctions::createFunctionBinding(frame, str("ifelse"), &ifelse);
        CoreFunctions::createFunctionBinding(frame, str("while"), &_while);
        CoreFunctions::createFunctionBinding(frame, str("times"), &times);
        CoreFunctions::createFunctionBinding(frame, str("fn/apply"), &apply);
        
        CoreFunctions::createFunctionBinding(frame, str("throw"), &_throw);
        CoreFunctions::createFunctionBinding(frame, str("rescue"), &rescue);
        
        CoreFunctions::createFunctionBinding(frame, str("->void"), &drop);
        CoreFunctions::createFunctionBinding(frame, str("let"), &bind);
        CoreFunctions::createFunctionBinding(frame, str("set!"), &set);
        CoreFunctions::createFunctionBinding(frame, str("destruct!"), &destructure);
        
        
        //String Functions
        CoreFunctions::createFunctionBinding(frame, str("str/eq"), &str_eq);
        CoreFunctions::createFunctionBinding(frame, str("str/compare"), &str_compare);
        CoreFunctions::createFunctionBinding(frame, str("str/contains"), &str_contains);
        CoreFunctions::createFunctionBinding(frame, str("str/starts-with"), &str_startsWith);
        CoreFunctions::createFunctionBinding(frame, str("str/ends-with"), &str_endsWith);
        
        CoreFunctions::createFunctionBinding(frame, str("str/char-at"), &str_charAt);
        CoreFunctions::createFunctionBinding(frame, str("str/index-of"), &str_indexOf);
        
        CoreFunctions::createFunctionBinding(frame, str("str/concat"), &str_concat);
        CoreFunctions::createFunctionBinding(frame, str("str/replace"), &str_replace);
        CoreFunctions::createFunctionBinding(frame, str("str/substr"), &str_substr);
        CoreFunctions::createFunctionBinding(frame, str("str/split"), &str_split);
        CoreFunctions::createFunctionBinding(frame, str("str/lower-case"), &str_lowerCase);
        CoreFunctions::createFunctionBinding(frame, str("str/upper-case"), &str_upperCase);
        CoreFunctions::createFunctionBinding(frame, str("str/capital-case"), &str_capitalCase);
        
        
        //Vector Functions
        CoreFunctions::createFunctionBinding(frame, str("vec/at"), &vec_at);
        CoreFunctions::createFunctionBinding(frame, str("vec/concat"), &vec_concat);
        CoreFunctions::createFunctionBinding(frame, str("vec/index-of"), &vec_indexOf);
        CoreFunctions::createFunctionBinding(frame, str("vec/last-index-of"), &vec_lastIndexOf);
        CoreFunctions::createFunctionBinding(frame, str("vec/join"), &vec_join);
        CoreFunctions::createFunctionBinding(frame, str("vec/subset"), &vec_subset);
        CoreFunctions::createFunctionBinding(frame, str("vec/sort"), &vec_sort);
        CoreFunctions::createFunctionBinding(frame, str("vec/for-each"), &vec_forEach);
        CoreFunctions::createFunctionBinding(frame, str("vec/filter"), &vec_filter);
        CoreFunctions::createFunctionBinding(frame, str("vec/map"), &vec_map);
        
        
        //Hash Functions
        CoreFunctions::createFunctionBinding(frame, str("hash/begin"), &hash_begin);
        CoreFunctions::createFunctionBinding(frame, str("hash/end"), &hash_end);
        CoreFunctions::createFunctionBinding(frame, str("hash/get"), &hash_get);
        CoreFunctions::createFunctionBinding(frame, str("hash/concat"), &hash_concat);
        CoreFunctions::createFunctionBinding(frame, str("hash/each-pair"), &hash_eachPair);
        
        
#if GFX_Language_SupportsFiles
        //File Functions
        CoreFunctions::createFunctionBinding(frame, str("file/exists?"), &file_exists);
        CoreFunctions::createFunctionBinding(frame, str("file/dir?"), &file_isDirectory);
        CoreFunctions::createFunctionBinding(frame, str("file/open"), &file_open);
        CoreFunctions::createFunctionBinding(frame, str("file/close"), &file_close);
        CoreFunctions::createFunctionBinding(frame, str("file/size"), &file_size);
        CoreFunctions::createFunctionBinding(frame, str("file/seek"), &file_seek);
        CoreFunctions::createFunctionBinding(frame, str("file/tell"), &file_tell);
        CoreFunctions::createFunctionBinding(frame, str("file/read"), &file_read);
        CoreFunctions::createFunctionBinding(frame, str("file/read-line"), &file_readLine);
        CoreFunctions::createFunctionBinding(frame, str("file/write"), &file_write);
        CoreFunctions::createFunctionBinding(frame, str("file/write-line"), &file_writeLine);
#endif /* GFX_Language_SupportsFiles */
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