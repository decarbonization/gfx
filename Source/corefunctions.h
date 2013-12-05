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
    
    ///The CoreFunctions class encapsulates all of the
    ///core language functions of the Gfx language.
    class CoreFunctions
    {
        ///Creates a new variable binding with a given name and value in a given frame.
        static void createVariableBinding(StackFrame *frame, const String *name, Base *value);
        
        ///Creates a new `gfx::NativeFunction` with a given implementation,
        ///and adds it to a given frame with a given name.
        static void createFunctionBinding(StackFrame *frame, const String *name, std::function<void(StackFrame *stack)> implementation);
        
    public:
        
        ///Adds all of the core language functions to a given frame.
        ///
        /// \param  frame   The frame to add the functions to. Required.
        static void addTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__corefunctions__) */
