//
//  stackframe.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__stackframe__
#define __gfx__stackframe__

#include "base.h"
#include "array.h"
#include "dictionary.h"
#include <tuple>

namespace gfx {
    class Interpreter;
    class Number;
    class Function;
    
    class StackFrame : public Base
    {
        Array<Base> *mStorage;
        Dictionary<const String, Base> *mBindings;
        StackFrame *mParent;
        /* weak */ Interpreter *mInterpreter;
        
    public:
        
        explicit StackFrame(StackFrame *parent, Interpreter *interpreter);
        virtual ~StackFrame();
        
#pragma mark - Stack Methods
        
        void push(Base *value);
        Base *pop();
        Number *popNumber();
        String *popString();
        Function *popFunction();
        
        template<typename T> T *popType()
        {
            Base *value = pop();
            if(!value->isKindOfClass<T>())
                throw Exception("wrong type on stack"_gfx, nullptr);
            
            return (T *)value;
        }
        
#pragma mark -
        
        void reset();
        void safeDrop();
        
#pragma mark -
        
        Base *peak() const;
        size_t depth() const;
        bool empty() const;
        void iterate(std::function<void(Base *value, CFIndex index, bool *stop)> function) const;
        
#pragma mark - Parent
        
        StackFrame *parent() const;
        Interpreter *interpreter() const;
        
#pragma mark - Bindings
        
        void createBindingWithValue(const String *key, Base *value);
        void setBindingToValue(const String *key, Base *value, bool searchParentScopes = true);
        Base *bindingValue(const String *key, bool searchParentScopes = true) const;
    };
    
#pragma mark - Unpacking Pops
    
    template<typename T1> std::tuple<T1> popMany(StackFrame *frame)
    {
        T1 *val1 = frame->popType<T1>();
        return std::make_tuple(val1);
    }
    
    template<typename T1, typename T2> std::tuple<T1, T2> popMany(StackFrame *frame)
    {
        T2 *val2 = frame->popType<T2>();
        T1 *val1 = frame->popType<T1>();
        return std::make_tuple(val1, val2);
    }
    
    template<typename T1, typename T2, typename T3> std::tuple<T1, T2, T3> popMany(StackFrame *frame)
    {
        T3 *val3 = frame->popType<T3>();
        T2 *val2 = frame->popType<T2>();
        T1 *val1 = frame->popType<T1>();
        return std::make_tuple(val1, val2, val3);
    }
    
    template<typename T1, typename T2, typename T3, typename T4> std::tuple<T1, T2, T3, T4> popMany(StackFrame *frame)
    {
        T4 *val4 = frame->popType<T4>();
        T3 *val3 = frame->popType<T3>();
        T2 *val2 = frame->popType<T2>();
        T1 *val1 = frame->popType<T1>();
        return std::make_tuple(val1, val2, val3, val4);
    }
}

#endif /* defined(__gfx__stackframe__) */
