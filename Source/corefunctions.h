//
//  corefunctions.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__corefunctions__
#define __gfx__corefunctions__

#include "base.h"

namespace gfx {
    class StackFrame;
    
    class CoreFunctions {
        
        static void createVariableBinding(StackFrame *frame, const String *name, Base *value);
        static void createFunctionBinding(StackFrame *frame, const String *name, std::function<void(StackFrame *stack)> implementation);
        
    public:
        
        static void addTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__corefunctions__) */
