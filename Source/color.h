//
//  color.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__color__
#define __gfx__color__

#if GFX_Include_GraphicsStack

#include "base.h"
#include "types.h"
#include <CoreGraphics/CoreGraphics.h>

namespace gfx {
    class StackFrame;
    
    ///The Color class encapsulates color and alpha information.
    ///
    ///The underlying type of Color is platform dependent. On OS X and iOS, it is CGColor.
    class Color : public Base
    {
    public:
        
        ///The native type of the Color class.
        typedef CGColorRef NativeType;
        
    protected:
        
        ///The native object backing the instance.
        CGColorRef mColor;
        
    public:
        
        ///Returns a new autoreleased white Color object.
        static Color *white()   { return make<Color>(1.0, 1.0, 1.0, 1.0); }
        
        ///Returns a new autoreleased black Color object.
        static Color *black()   { return make<Color>(0.0, 0.0, 0.0, 1.0); }
        
        ///Returns a new autoreleased translucent Color object.
        static Color *clear()   { return make<Color>(0.0, 0.0, 0.0, 0.0); }
        
        ///Returns a new autoreleased red Color object.
        static Color *red()     { return make<Color>(1.0, 0.0, 0.0, 1.0); }
        
        ///Returns a new autoreleased green Color object.
        static Color *green()   { return make<Color>(0.0, 1.0, 0.0, 1.0); }
        
        ///Returns a new autoreleased blue Color object.
        static Color *blue()    { return make<Color>(0.0, 0.0, 1.0, 1.0); }
        
        ///Returns a new autoreleased orange Color object.
        static Color *orange()  { return make<Color>(1.0, 0.5, 0.0, 1.0); }
        
        ///Returns a new autoreleased purple Color object.
        static Color *purple()  { return make<Color>(0.5, 0.0, 0.5, 1.0); }
        
        ///Returns a new autoreleased pink Color object.
        static Color *pink()    { return make<Color>(1.0, 0.0, 1.0, 1.0); }
        
        ///Returns a new autoreleased brown Color object.
        static Color *brown()   { return make<Color>(0.6, 0.4, 0.2, 1.0); }
        
        ///Returns a new autoreleased yellow Color object.
        static Color *yellow()  { return make<Color>(1.0, 1.0, 0.0, 1.0); }
        
#pragma mark - Lifecycle
        
        ///Constructs the Color using a native object.
        ///
        /// \param  color   The native color object. Should not be null.
        ///
        Color(NativeType color);
        
        ///Constructs the Color using given red, green, blue, and alpha values.
        ///
        /// \param  red     The red value of the color. Range is {0.0, 1.0}.
        /// \param  green   The green value of the color. Range is {0.0, 1.0}.
        /// \param  blue    The blue value of the color. Range is {0.0, 1.0}.
        /// \param  alpha   The alpha value of the color. Range is {0.0, 1.0}.
        ///
        Color(Float red, Float green, Float blue, Float alpha);
        
        ///Constructs the Color from a String instance containing an HTML style color.
        ///
        /// \param  inColorString   The color string. Should not be null.
        ///
        ///The color string should be of the form `#ffffff`. Currently only six character
        ///HTML color strings are supported. Any other value will raise an exception.
        Color(const String *inColorString);
        
        ///The destructor.
        ~Color();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
#pragma mark - Setting
        
        ///Sets the receiver as the current stroke and
        ///fill color in the current `gfx::Context`.
        void set();
        
        ///Sets the receiver as the current fill color.
        void setFill();
        
        ///Sets the receiver as the current stroke color.
        void setStroke();
        
#pragma mark - Introspection
        
        ///Returns the number of components that make up the Color instance.
        ///
        ///The number does not include the alpha value of the color.
        Index numberOfComponents() const;
        
        ///Returns the components that make up the Color instance.
        ///
        ///Does not include the alpha value of the color.
        const Float *getComponents() const;
        
        ///Returns the alpha value of the color.
        Float alpha() const;
        
        ///Returns the native object backing the Color.
        NativeType get() const;
        
#pragma mark - Functions
        
        ///Adds the color function suite to a given stack frame.
        static void AddTo(StackFrame *frame);
    };
}

#endif /* GFX_Include_GraphicsStack */

#endif /* defined(__gfx__color__) */
