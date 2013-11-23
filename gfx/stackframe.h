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
#include <stack>

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
}

#endif /* defined(__gfx__stackframe__) */
