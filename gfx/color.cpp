//
//  color.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "color.h"
#include "graphics.h"

#include "string.h"

namespace gfx {
    Color::Color(CGColorRef color) :
        Base(),
        mColor(color)
    {
        CFRetain(color);
    }
    
    Color::Color(CGFloat red, CGFloat green, CGFloat blue, CGFloat alpha) :
        Base(),
        mColor(NULL)
    {
        CGColorRef color = CGColorCreateGenericRGB(red, green, blue, alpha);
        mColor = color;
    }
    
    Color::Color(const Word *word) :
        Base(),
        mColor(NULL)
    {
        
    }
    
    Color::~Color()
    {
        if(mColor) {
            CFRelease(mColor);
            mColor = NULL;
        }
    }
    
#pragma mark - Identity
    
    CFHashCode Color::hash() const
    {
        return CFHash(mColor);
    }
    
    bool Color::isEqual(const Base *other) const
    {
        if(other->isKindOfClass<Color>()) {
            const Color *otherColor = (const Color *)other;
            return CFEqual(mColor, otherColor->mColor);
        }
        
        return false;
    }
    
    const String *Color::description() const
    {
        String::Builder description;
        
        description << "<" << this->className() << ":" << (void *)this << " {";
        
        const CGFloat *components = getComponents();
        for (CFIndex i = 0, count = numberOfComponents(); i < count; i++) {
            description << components[i] << ", ";
        }
        
        description << getAlpha() << "}>";
        
        return description;
    }
    
#pragma mark - Setting
    
    void Color::set()
    {
        setFill();
        setStroke();
    }
    
    void Color::setFill()
    {
        CGContextSetFillColorWithColor(Graphics::getCurrentContext(), mColor);
    }
    
    void Color::setStroke()
    {
        CGContextSetStrokeColorWithColor(Graphics::getCurrentContext(), mColor);
    }
    
#pragma mark - Introspection
    
    CFIndex Color::numberOfComponents() const
    {
        return CGColorGetNumberOfComponents(mColor);
    }
    
    const CGFloat *Color::getComponents() const
    {
        return CGColorGetComponents(mColor);
    }
    
    CGColorRef Color::getColor() const
    {
        return mColor;
    }
    
    CGColorSpaceRef Color::getColorSpace() const
    {
        return CGColorGetColorSpace(mColor);
    }
    
    CGFloat Color::getAlpha() const
    {
        return CGColorGetAlpha(mColor);
    }
}
