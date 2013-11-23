//
//  offset.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/16/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_offset_h
#define gfx_offset_h

#include <CoreFoundation/CoreFoundation.h>
#include "string.h"

namespace gfx {
    class String;
    
    ///The Offset struct describes the originating location
    //of Expressions and Words from the `gfx::Parser` class.
    struct Offset {
        ///The line of origination.
        CFIndex line;
        
        ///The line-offset of origination.
        CFIndex offset;
        
        
        ///The Invalid offset. Used as a default placeholder, like null.
        static const Offset Invalid;
        
        ///Returns a bool indicating whether or not the offset is invalid.
        bool isInvalid() const;
        
        ///Returns a human-readable description of the offset.
        const String *description() const;
    };
}

#endif
