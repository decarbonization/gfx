//
//  graphics.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__graphics__
#define __gfx__graphics__

#if GFX_Include_GraphicsStack

#include "base.h"
#include "array.h"
#include <CoreGraphics/CoreGraphics.h>

namespace gfx {
    class StackFrame;
    class Interpreter;
    
#pragma mark - Utility Functions
    
    extern Array<Base> *VectorFromSize(Size size);
    extern Size VectorToSize(const Array<Base> *sizeVector);
    extern Array<Base> *VectorFromPoint(Point point);
    extern Point VectorToPoint(const Array<Base> *pointVector);
    extern Array<Base> *VectorFromRect(Rect rect);
    extern Rect VectorToRect(const Array<Base> *rectVector);
    
    class Graphics : public Base
    {
        static void addTo(StackFrame *frame);
        
    public:
        
        static void AttachTo(Interpreter *interpreter);
    };
}

#endif /* GFX_Include_GraphicsStack */

#endif /* defined(__gfx__graphics__) */
