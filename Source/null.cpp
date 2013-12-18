//
//  null.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/17/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "null.h"
#include "exception.h"

namespace gfx {
    Null::Null() :
        Base()
    {
    }
    
    Null::~Null()
    {
        throw Exception(str("Null instance destroyed. There is only one null instance. This is a fatal error."), nullptr);
    }
    
    Null *Null::shared()
    {
        static Null *sharedNull = new Null();
        return retained_autoreleased(sharedNull);
    }
    
#pragma mark - Identity
    
    HashCode Null::hash() const
    {
        return 'null';
    }
    
    bool Null::isEqual(const Base *other) const
    {
        return (other == this);
    }
    
    const String *Null::description() const
    {
        return str("null");
    }
}
