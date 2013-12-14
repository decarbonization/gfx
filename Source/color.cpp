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
#include "str.h"
#include "exception.h"

#include "stackframe.h"
#include "number.h"

namespace gfx {
    
#pragma mark - Lifecycle
    
    Color::Color(NativeType color) :
        Base(),
        mColor(color)
    {
        CFRetain(color);
    }
    
    Color::Color(Float red, Float green, Float blue, Float alpha) :
        Base(),
        mColor(NULL)
    {
        CGColorRef color = CGColorCreateGenericRGB(red, green, blue, alpha);
        mColor = color;
    }
    
    Color::Color(const String *inColorString) :
        Base(),
        mColor(NULL)
    {
        auto colorString = autoreleased(copy(inColorString));
        
        if(colorString->hasPrefix(str("#")))
            colorString->deleteRange(Range(0, 1));
        
        if(colorString->length() == 6) {
            unsigned long colorCode = strtoul(colorString->getCString(), NULL, 16);
            auto red = (unsigned char)(colorCode >> 16);
            auto green = (unsigned char)(colorCode >> 8);
            auto blue = (unsigned char)(colorCode);
            mColor = CGColorCreateGenericRGB(red / 255.0, green / 255.0, blue / 255.0, 1.0);
        } else {
            throw Exception(str("malformed HTML color string given"), nullptr);
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
    
    HashCode Color::hash() const
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
        
        const Float *components = getComponents();
        for (Index i = 0, count = numberOfComponents(); i < count; i++) {
            description << components[i] << ", ";
        }
        
        description << alpha() << "}>";
        
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
        CGContextSetFillColorWithColor(Context::currentContext()->get(), mColor);
    }
    
    void Color::setStroke()
    {
        CGContextSetStrokeColorWithColor(Context::currentContext()->get(), mColor);
    }
    
#pragma mark - Introspection
    
    Index Color::numberOfComponents() const
    {
        return CGColorGetNumberOfComponents(mColor);
    }
    
    const Float *Color::getComponents() const
    {
        return CGColorGetComponents(mColor);
    }
    
    Float Color::alpha() const
    {
        return CGColorGetAlpha(mColor);
    }
    
    Color::NativeType Color::get() const
    {
        return mColor;
    }
    
#pragma mark - Color Functions
    
    static void rgb(StackFrame *stack)
    {
        auto blue = stack->popNumber();
        auto green = stack->popNumber();
        auto red = stack->popNumber();
        
        stack->push(make<Color>(red->value() / 255.0, green->value() / 255.0, blue->value() / 255.0, 1.0));
    }
    
    static void rgba(StackFrame *stack)
    {
        auto alpha = stack->popNumber();
        auto blue = stack->popNumber();
        auto green = stack->popNumber();
        auto red = stack->popNumber();
        
        stack->push(make<Color>(red->value() / 255.0, green->value() / 255.0, blue->value() / 255.0, alpha->value()));
    }
    
    static void set_fill(StackFrame *stack)
    {
        auto color = stack->popType<Color>();
        color->setFill();
    }
    
    static void set_stroke(StackFrame *stack)
    {
        auto color = stack->popType<Color>();
        color->setStroke();
    }
    
#pragma mark -
    
    void Color::AddTo(StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        frame->createVariableBinding(str("white"), Color::white());
        frame->createVariableBinding(str("black"), Color::black());
        frame->createVariableBinding(str("translucent"), Color::clear());
        frame->createVariableBinding(str("red"), Color::red());
        frame->createVariableBinding(str("green"), Color::green());
        frame->createVariableBinding(str("blue"), Color::blue());
        frame->createVariableBinding(str("orange"), Color::orange());
        frame->createVariableBinding(str("purple"), Color::purple());
        frame->createVariableBinding(str("pink"), Color::pink());
        frame->createVariableBinding(str("brown"), Color::brown());
        frame->createVariableBinding(str("yellow"), Color::yellow());
        
        frame->createFunctionBinding(str("rgb"), &rgb);
        frame->createFunctionBinding(str("rgba"), &rgba);
        frame->createFunctionBinding(str("set-fill"), &set_fill);
        frame->createFunctionBinding(str("set-stroke"), &set_stroke);
    }
}

#endif /* GFX_Include_GraphicsStack */
