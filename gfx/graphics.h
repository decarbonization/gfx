//
//  graphics.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__graphics__
#define __gfx__graphics__

#include "base.h"
#include <CoreGraphics/CoreGraphics.h>

namespace gfx {
    class StackFrame;
    
    class Graphics : public Base
    {
    public:
        
#pragma mark - Public Interface
        
        static void createVariableBinding(StackFrame *frame, const String *name, Base *value);
        static void createFunctionBinding(StackFrame *frame, const String *name, std::function<void(StackFrame *stack)> implementation);
        static void addTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__graphics__) */
