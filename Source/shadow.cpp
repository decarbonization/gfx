//
//  shadow.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "shadow.h"

#include "context.h"
#include "color.h"

#include "stackframe.h"
#include "number.h"
#include "graphics.h"

namespace gfx {
    Shadow::Shadow(Size offset, Float blurRadius, Color *color) :
        mOffset(offset),
        mBlurRadius(blurRadius),
        mColor(retained(color))
    {
    }
    
    Shadow::~Shadow()
    {
        released(mColor);
        mColor = nullptr;
    }
    
#pragma mark - Identity
    
    bool Shadow::isEqual(const Shadow *other) const
    {
        if(!other)
            return false;
        
        return (mOffset.width == other->mOffset.width &&
                mOffset.height == other->mOffset.height &&
                mBlurRadius == other->mBlurRadius &&
                mColor->isEqual(other->mColor));
    }
    
    bool Shadow::isEqual(const Base *other) const
    {
        if(other && other->isKindOfClass<Shadow>())
            return this->isEqual((const Shadow *)other);
        
        return false;
    }
    
#pragma mark - Introspection
    
    Size Shadow::offset() const
    {
        return mOffset;
    }
    
    Float Shadow::blurRadius() const
    {
        return mBlurRadius;
    }
    
    Color *Shadow::color() const
    {
        return retained_autoreleased(mColor);
    }
    
#pragma mark - Using
    
    void Shadow::set()
    {
        CGContextSetShadowWithColor(Context::currentContext()->get(), offset(), blurRadius(), color()->get());
    }
    
    void Shadow::unset()
    {
        CGContextSetShadowWithColor(Context::currentContext()->get(), CGSizeZero, 0.0, NULL);
    }
    
#pragma mark - Functions
    
    static void shadow_make(StackFrame *stack)
    {
        /* vec num color -- shadow */
        
        auto color = stack->popType<Color>();
        auto blurRadius = stack->popNumber();
        auto offset = VectorToSize(stack->popType<Array<Base>>());
        
        stack->push(make<Shadow>(offset, blurRadius->value(), color));
    }
    
    static void shadow_set(StackFrame *stack)
    {
        /* shadow -- */
        
        auto shadow = stack->popType<Shadow>();
        shadow->set();
    }
    
    static void shadow_unset(StackFrame *stack)
    {
        /* -- */
        
        Shadow::unset();
    }
    
#pragma mark -
    
    void Shadow::addTo(StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        frame->createFunctionBinding(str("shadow"), &shadow_make);
        
        frame->createFunctionBinding(str("shadow/set"), &shadow_set);
        frame->createFunctionBinding(str("shadow/unset"), &shadow_unset);
    }
}
