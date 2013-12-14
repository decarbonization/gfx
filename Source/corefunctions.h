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
    class Interpreter;
    
    ///The CoreFunctions class encapsulates all of the
    ///core language functions of the Gfx language.
    class CoreFunctions
    {
        ///Returns the shared core function frame, creating it if it does not already exist.
        ///
        ///__Important:__ The frame returned by this method __is not__ usable directly. It
        ///__must__ be used as the parent of a child stack frame. The returned frame is
        ///frozen, and won't take new stack or binding values. Additionally, it has no
        ///interpreter associated with it, and as such will cause crashes if used.
        static StackFrame *sharedCoreFunctionFrame();
        
    public:
        
        ///Adds all of the core language functions to a given frame.
        ///
        /// \param  frame   The frame to add the functions to. Required.
        static void addTo(StackFrame *frame);
        
        ///Creates a new core function frame for use with a new instance of `gfx::Interpreter`.
        static StackFrame *createCoreFunctionFrame(Interpreter *interpreter);
    };
}

#endif /* defined(__gfx__corefunctions__) */
