//
//  stackframe.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "stackframe.h"
#include "dictionary.h"
#include "exception.h"
#include "number.h"
#include "function.h"

namespace gfx {
    
    StackFrame::StackFrame(StackFrame *parent, Interpreter *interpreter) :
        mStorage(new Array<Base>),
        mParent(parent),
        mBindings(new Dictionary<const String, Base>),
        mInterpreter(interpreter),
        DestroySignal("gfx::StackFrame::DestroySignal"_gfx)
    {
        if(mParent)
            mParent->DestroySignal += [this](Nothing) { autoreleased(this); };
    }
    
    StackFrame::~StackFrame()
    {
        DestroySignal();
        
        released(mStorage);
        released(mBindings);
    }
    
#pragma mark - Stack Methods
    
    void StackFrame::push(Base *value)
    {
        if(!value)
            mStorage->append(make<Number>(0));
        else
            mStorage->append(value);
    }
    
    Base *StackFrame::pop()
    {
        if(empty()) {
            if(!mParent)
                gfx_assert(false, "stack underflow"_gfx);
            else
                return mParent->pop();
        }
            
        Base *value = peak();
        mStorage->removeLast();
        return value;
    }
    
    
    Number *StackFrame::popNumber()
    {
        return popType<Number>();
    }
    
    String *StackFrame::popString()
    {
        return popType<String>();
    }
    
    Function *StackFrame::popFunction()
    {
        return popType<Function>();
    }
    
#pragma mark -
    
    void StackFrame::safeDrop()
    {
        if(!empty())
            pop();
    }
    
    void StackFrame::reset()
    {
        mStorage->removeAll();
    }
    
#pragma mark -
    
    Base *StackFrame::peak() const
    {
        if(empty()) {
            if(!mParent)
                gfx_assert(false, "stack underflow"_gfx);
            else
                return mParent->peak();
        }
        
        return mStorage->last();
    }
    
    size_t StackFrame::depth() const
    {
        return mStorage->count();
    }
    
    bool StackFrame::empty() const
    {
        return (mStorage->count() == 0);
    }
    
    void StackFrame::iterate(std::function<void(Base *value, Index index, bool *stop)> function) const
    {
        mStorage->iterate(function);
    }
    
#pragma mark - Parent
    
    StackFrame *StackFrame::parent() const
    {
        return retained_autoreleased(mParent);
    }
    
    Interpreter *StackFrame::interpreter() const
    {
        return mInterpreter;
    }
    
#pragma mark - Bindings
    
    void StackFrame::createBindingWithValue(const String *key, Base *value)
    {
        gfx_assert_param(key);
        
        mBindings->set(key, value);
    }
    
    void StackFrame::setBindingToValue(const String *key, Base *value, bool searchParentScopes)
    {
        gfx_assert_param(key);
        
        auto bindings = mBindings;
        if(searchParentScopes) {
            auto parentScope = this;
            
            do {
                if(parentScope->bindingValue(key, false)) {
                    bindings = parentScope->mBindings;
                    break;
                }
                
                parentScope = parentScope->parent();
            } while (parentScope != nullptr);
        }
        
        bindings->set(key, value);
    }
    
    Base *StackFrame::bindingValue(const String *key, bool searchParentScopes) const
    {
        gfx_assert_param(key);
        
        Base *value = mBindings->get(key);
        if(value)
            return value;
        else if(searchParentScopes && mParent)
            return mParent->bindingValue(key);
        else
            return nullptr;
    }
}
