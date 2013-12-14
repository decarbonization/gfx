//
//  object.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/11/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "object.h"
#include "str.h"
#include "interpreter.h"
#include "stackframe.h"

namespace gfx {
    Object::Object(const String *name) :
        mName(copy(name)),
        mStorage(new Dictionary<const String, Base>()),
        mConstructor(nullptr)
    {
    }
    
    Object::Object(const Object *object) :
        mName(copy(object->mName)),
        mStorage(copy(object->mStorage)),
        mConstructor(retained(object->constructor()))
    {
    }
    
    Object::~Object()
    {
        released(mName);
        mName = nullptr;
        
        released(mStorage);
        mStorage = nullptr;
        
        released(mConstructor);
        mConstructor = nullptr;
    }
    
#pragma mark - Identity
    
    HashCode Object::hash() const
    {
        return mName->hash() ^ mStorage->hash();
    }
    
    const String *Object::description() const
    {
        return Base::description();
    }
    
    bool Object::isEqual(const Object *other) const
    {
        if(!other)
            return false;
        
        return (mName->isEqual(other->name()) &&
                mStorage->isEqual(other->mStorage));
    }
    
    bool Object::isEqual(const Base *other) const
    {
        if(other && other->isKindOfClass<Object>())
            return this->isEqual(static_cast<const Object *>(other));
        else
            return false;
    }
    
#pragma mark - Cloning
    
    void Object::setConstructor(Function *constructor)
    {
        autoreleased(mConstructor);
        mConstructor = retained(constructor);
    }
    
    Function *Object::constructor() const
    {
        return retained_autoreleased(mConstructor);
    }
    
    void Object::apply(StackFrame *stack) const
    {
        auto clone = copy(this);
        
        if(auto constructor = clone->constructor()) {
            constructor->apply(stack);
        }
        
        stack->push(clone);
    }
    
#pragma mark - Slots
    
    void Object::set(const String *name, Base *value)
    {
        mStorage->set(name, value);
    }
    
    Base *Object::get(const String *name) const
    {
        return mStorage->get(name);
    }
}
