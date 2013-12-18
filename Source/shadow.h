//
//  shadow.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__shadow__
#define __gfx__shadow__

#include "base.h"
#include "types.h"

namespace gfx {
    class Color;
    class StackFrame;
    
    ///The Shadow class encapsulates the attributes necessary to
    ///apply a shadow to one or more drawing operations.
    class Shadow : public Base
    {
        ///The offset.
        Size mOffset;
        
        ///The blur radius.
        Float mBlurRadius;
        
        ///The color.
        Color *mColor;
        
    public:
        
        ///Constructs a shadow with the necessary attributes to render it.
        ///
        /// \param  offset      The offset of the shadow.
        /// \param  blurRadius  The blur radius of the shadow.
        /// \param  color       The color of the shadow.
        ///
        Shadow(Size offset, Float blurRadius, Color *color);
        
        ///The destructor.
        ~Shadow();
        
#pragma mark - Identity
        
        bool isEqual(const Shadow *other) const;
        bool isEqual(const Base *other) const override;
        
#pragma mark - Introspection
        
        ///Returns the offset of the shadow.
        Size offset() const;
        
        ///Returns the blur radius of the shadow.
        Float blurRadius() const;
        
        ///Returns the color of the shadow.
        Color *color() const;
        
#pragma mark - Using
        
        ///Sets the shadow of subsequent drawing operations to that described by the receiver.
        void set();
        
        ///Removes any shadow applied to the current context.
        static void unset();
        
#pragma mark - Functions
        
        ///Adds the functions in the shadow suite to a given stack frame.
        static void addTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__shadow__) */
