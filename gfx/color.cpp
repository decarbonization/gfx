//
//  color.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Include_GraphicsStack

#include "color.h"
#include "context.h"
#include "string.h"
#include "exception.h"
#include "word.h"

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
        auto colorString = autoreleased(const_cast<String *>(copy(word->string())));
        
        if(colorString->hasPrefix("#"_gfx))
            colorString->deleteRange(CFRangeMake(0, 1));
        
        if(colorString->length() == 6) {
            unsigned long colorCode = strtoul(colorString->getCString(), NULL, 16);
            auto red = (unsigned char)(colorCode >> 16);
            auto green = (unsigned char)(colorCode >> 8);
            auto blue = (unsigned char)(colorCode);
            mColor = CGColorCreateGenericRGB(red / 255.0, green / 255.0, blue / 255.0, 1.0);
        } else {
            throw Exception("malformed HTML color string given"_gfx, nullptr);
        }
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
        CGContextSetFillColorWithColor(Context::currentContext()->getContext(), mColor);
    }
    
    void Color::setStroke()
    {
        CGContextSetStrokeColorWithColor(Context::currentContext()->getContext(), mColor);
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

#endif /* GFX_Include_GraphicsStack */
