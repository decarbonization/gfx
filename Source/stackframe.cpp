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
#include "null.h"
#include "function.h"

/* These macros exist to make it easier to introduce more nuanced locking later. */

///Introduces a scoped synchronization point for read operations.
///
///Only one of these macros may be used per scope.
#define SCOPED_READ_GUARD  std::lock_guard<std::recursive_mutex> scopeReadLock(mReadWriteMutex)

///Introduces a scoped synchronization point for write operations.
///
///Only one of these macros may be used per scope.
#define SCOPED_WRITE_GUARD std::lock_guard<std::recursive_mutex> scopeWriteLock(mReadWriteMutex)

namespace gfx {
    
    StackFrame::StackFrame(StackFrame *parent, Interpreter *interpreter) :
        mReadWriteMutex(),
        mStorage(new Array<Base>),
        mParent(parent),
        mBindings(new Dictionary<const String, Base>),
        mInterpreter(interpreter),
        mIsFrozen(false),
        mDestroySignalReference(),
        DestroySignal(str("gfx::StackFrame::DestroySignal"))
    {
        if(mParent)
            mDestroySignalReference = mParent->DestroySignal.add([this](Nothing) {
                mParent = nullptr;
                autoreleased(this);
            });
    }
    
    StackFrame::~StackFrame()
    {
        DestroySignal();
        
        if(mParent) {
            mParent->DestroySignal.remove(mDestroySignalReference);
        }
        
        released(mStorage);
        released(mBindings);
    }
    
#pragma mark - Stack Methods
    
    void StackFrame::push(Base *value)
    {
        SCOPED_WRITE_GUARD;
        
        assertMutationPossible();
        
        if(!value)
            mStorage->append(Null::shared());
        else
            mStorage->append(value);
    }
    
    Base *StackFrame::pop()
    {
        SCOPED_WRITE_GUARD;
        
        assertMutationPossible();
        
        if(empty()) {
            if(!mParent || mParent->isFrozen())
                gfx_assert(false, str("stack underflow"));
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
        SCOPED_WRITE_GUARD;
        
        assertMutationPossible();
        
        if(!empty())
            pop();
    }
    
    void StackFrame::dropAll()
    {
        SCOPED_WRITE_GUARD;
        
        assertMutationPossible();
        
        mStorage->removeAll();
    }
    
#pragma mark -
    
    Base *StackFrame::peak() const
    {
        SCOPED_READ_GUARD;
        
        if(empty()) {
            if(!mParent || mParent->isFrozen())
                gfx_assert(false, str("stack underflow"));
            else
                return mParent->peak();
        }
        
        return mStorage->last();
    }
    
    size_t StackFrame::depth() const
    {
        SCOPED_READ_GUARD;
        
        return mStorage->count();
    }
    
    bool StackFrame::empty() const
    {
        SCOPED_READ_GUARD;
        
        return (mStorage->count() == 0);
    }
    
    void StackFrame::iterate(std::function<void(Base *value, Index index, bool *stop)> function) const
    {
        SCOPED_READ_GUARD;
        
        mStorage->iterate(mStorage->all(), function);
    }
    
#pragma mark - Parent
    
    StackFrame *StackFrame::parent() const
    {
        SCOPED_READ_GUARD;
        
        return retained_autoreleased(mParent);
    }
    
    Interpreter *StackFrame::interpreter() const
    {
        SCOPED_READ_GUARD;
        
        return mInterpreter;
    }
    
#pragma mark - Bindings
    
    void StackFrame::setBindingToValue(const String *key, Base *value, bool searchParentScopes)
    {
        gfx_assert_param(key);
        
        SCOPED_WRITE_GUARD;
        
        assertMutationPossible(String::Builder() << "Cannot change value of binding '" << key << "'.");
        
        auto bindings = mBindings;
        if(searchParentScopes && parent()) {
            auto parentScope = parent();
            
            do {
                if(parentScope->bindingValue(key, false)) {
                    parentScope->assertMutationPossible(String::Builder() << "Cannot change value of binding '" << key << "'.");
                    
                    bindings = parentScope->mBindings;
                    break;
                }
                
                parentScope = parentScope->parent();
            } while (parentScope != nullptr);
            
            if(!bindings)
                bindings = mBindings;
        }
        
        bindings->set(key, value);
    }
    
    Base *StackFrame::bindingValue(const String *key, bool searchParentScopes) const
    {
        gfx_assert_param(key);
        
        SCOPED_READ_GUARD;
        
        Base *value = mBindings->get(key);
        if(value)
            return value;
        else if(searchParentScopes && parent())
            return parent()->bindingValue(key);
        else
            return nullptr;
    }
    
#pragma mark -
    
    void StackFrame::createVariableBinding(const String *name, Base *value)
    {
        this->setBindingToValue(name, value, false);
    }
    
    void StackFrame::createFunctionBinding(const String *name, std::function<void(StackFrame *stack)> implementation)
    {
        this->setBindingToValue(name, make<NativeFunction>(name, implementation), false);
    }
    
#pragma mark - Freezing
    
    void StackFrame::assertMutationPossible(const String *message, const String *affectedBindingKey) const
    {
        if(mIsFrozen) {
            Dictionary<const String, Base> *userInfo = nullptr;
            if(affectedBindingKey) {
                userInfo = autoreleased(new Dictionary<const String, Base>{
                    {str("AffectedBindingKey"), (Base *)affectedBindingKey},
                });
            }
            throw StackFrame::AccessViolationException(message, userInfo);
        }
    }
    
    void StackFrame::freeze()
    {
        SCOPED_WRITE_GUARD;
        
        mIsFrozen = true;
    }
    
    void StackFrame::unfreeze()
    {
        SCOPED_WRITE_GUARD;
        
        mIsFrozen = false;
    }
    
    bool StackFrame::isFrozen() const
    {
        SCOPED_READ_GUARD;
        
        return mIsFrozen;
    }
}
