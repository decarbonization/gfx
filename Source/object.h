//
//  object.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/11/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__object__
#define __gfx__object__

#include "function.h"
#include "dictionary.h"

namespace gfx {
    class String;
    
    class Object : public Function
    {
        const String *mName;
        Dictionary<const String, Base> *mStorage;
        
    public:
        
        Object(const String *name);
        Object(const Object *object);
        ~Object();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        const String *description() const override;
        bool isEqual(const Object *other) const;
        bool isEqual(const Base *other) const override;
        
#pragma mark - Introspection
        
        virtual const String *name() const { return retained_autoreleased(mName); }
        
#pragma mark - Cloning
        
        virtual void apply(StackFrame *stack) const override;
        
#pragma mark - Slots
        
        virtual void set(const String *name, Base *value);
        virtual Base *get(const String *name) const;
    };
}

#endif /* defined(__gfx__object__) */
