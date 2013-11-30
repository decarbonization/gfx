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
#include <CoreGraphics/CoreGraphics.h>

namespace gfx {
    class Word;
    
    class Color : public Base
    {
    public:
        
        typedef CGColorRef NativeType;
        
    protected:
        
        CGColorRef mColor;
        
    public:
        
        Color(CGColorRef color);
        Color(Float red, Float green, Float blue, Float alpha);
        Color(const Word *word);
        ~Color();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
#pragma mark - Setting
        
        void set();
        void setFill();
        void setStroke();
        
#pragma mark - Introspection
        
        Index numberOfComponents() const;
        const Float *getComponents() const;
        NativeType get() const;
        CGColorSpaceRef getColorSpace() const;
        Float getAlpha() const;
    };
}

#endif /* GFX_Include_GraphicsStack */

#endif /* defined(__gfx__color__) */
