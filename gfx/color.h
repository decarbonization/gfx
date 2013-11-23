//
//  color.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__color__
#define __gfx__color__

#include "base.h"
#include "cf.h"
#include <CoreGraphics/CoreGraphics.h>

namespace gfx {
    class Word;
    
    class Color : public Base
    {
        CGColorRef mColor;
        
    public:
        
        Color(CGColorRef color);
        Color(CGFloat red, CGFloat green, CGFloat blue, CGFloat alpha);
        Color(const Word *word);
        ~Color();
        
#pragma mark - Identity
        
        CFHashCode hash() const override;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
#pragma mark - Setting
        
        void set();
        void setFill();
        void setStroke();
        
#pragma mark - Introspection
        
        CFIndex numberOfComponents() const;
        const CGFloat *getComponents() const;
        CGColorRef getColor() const;
        CGColorSpaceRef getColorSpace() const;
        CGFloat getAlpha() const;
    };
}

#endif /* defined(__gfx__color__) */
