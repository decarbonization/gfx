//
//  offset.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/16/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "offset.h"
#include "str.h"

namespace gfx {
    const Offset Offset::Invalid = { -1, 0 };
    
    bool Offset::isInvalid() const
    {
        return (line == -1);
    }
    
    const String *Offset::description() const
    {
        if(this->isInvalid())
            return str("gfx::Offset::Invalid");
        else
            return String::Builder() << "gfx::Offset{" << line << ", " << column << "}";
    }
}