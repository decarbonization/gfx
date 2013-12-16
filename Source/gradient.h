//
//  gradient.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__gradient__
#define __gfx__gradient__

#include "base.h"
#include "array.h"

#include <vector>

namespace gfx {
    class Color;
    class Path;
    class StackFrame;
    
    ///The Gradient class encapsulates creation and drawing of gradients in the graphics stack.
    ///Gradients may be draw in a linear or radial fashion, and may be drawn into simple rectangles,
    ///or complex paths.
    class Gradient : public Base
    {
    public:
        
        ///The underlying native type of the gradient class.
        typedef CGGradientRef NativeType;
        
    protected:
        
        ///The underlying native type of the gradient.
        NativeType mStorage;
        
    public:
        
        ///Constructs a new gradient with an array of colors, and their locations.
        ///
        /// \param  colors  The colors to place into the gradient. Should not be null.
        /// \param  locations   The corresponding locations for each color. Locations are in the range of {0.0, 1.0}.
        ///
        Gradient(const Array<Color> *colors, const std::vector<Float> &locations);
        
        ///The destructor.
        ~Gradient();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const Gradient *other) const;
        bool isEqual(const Base *other) const override;
        
#pragma mark - Introspection
        
        ///Returns the underlying native value backing the receiver.
        NativeType get() const;
        
#pragma mark - Drawing
        
        ///Draws the receiver as a linear gradient within a given rectangle, using a given angle.
        ///
        /// \param  rect    The rectangle to draw the gradient within.
        /// \param  angle   The angle to draw the gradient at.
        ///
        void drawLinearInRect(Rect rect, Float angle);
        
        ///Draws the receiver as a linear gradient within a given path, using a given angle.
        ///
        /// \param  path    The path to draw the gradient within.
        /// \param  angle   The angle to draw the gradient at.
        ///
        void drawLinearInPath(const Path *path, Float angle);
        
#pragma mark -
        
        ///Draws the receiver as a radial gradient within a given rectangle, using given relative center locations.
        ///
        /// \param  rect                The rectangle to draw the gradient within.
        /// \param  relativeCenterPoint The offset to place the center of the radial gradient.
        ///                             Both x and y are in the range of {0.0, 1.0}.
        ///
        void drawRadialInRect(Rect rect, Point relativeCenterPoint);
        
        ///Draws the receiver as a radial gradient within a given path, using given relative center locations.
        ///
        /// \param  rect                The path to draw the gradient within.
        /// \param  relativeCenterPoint The offset to place the center of the radial gradient.
        ///                             Both x and y are in the range of {0.0, 1.0}.
        ///
        void drawRadialInPath(const Path *path, Point relativeCenterPoint);
        
#pragma mark - Functions
        
        ///Adds the functions in the gradient suite to a given frame.
        static void AddTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__gradient__) */
