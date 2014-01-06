//
//  gradient.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "gradient.h"
#include "color.h"
#include "context.h"
#include "path.h"

#include "graphics.h"
#include "exception.h"
#include "stackframe.h"
#include "number.h"

namespace gfx {
    Gradient::Gradient(const Array<Color> *colors, const std::vector<Float> &locations, const DrawingInformation &info) :
        Base(),
        mStorage(),
        mDrawingInformation(info)
    {
        gfx_assert(colors->count() == locations.size(), str("colors and locations must be the same size"));
        
        cf::AutoRef<CGColorSpaceRef> colorSpace = CGColorSpaceCreateDeviceRGB();
        cf::MutableArrayAutoRef nativeColors = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        colors->iterate(colors->all(), [nativeColors](Color *color, Index index, bool *stop) {
            CFArrayAppendValue(nativeColors, color->get());
        });
        mStorage = CGGradientCreateWithColors(colorSpace, nativeColors, &locations[0]);
    }
    
    Gradient::~Gradient()
    {
        if(mStorage) {
            CFRelease(mStorage);
            mStorage = NULL;
        }
    }
    
#pragma mark - Identity
    
    HashCode Gradient::hash() const
    {
        return CFHash(get());
    }
    
    bool Gradient::isEqual(const Gradient *other) const
    {
        if(!other)
            return false;
        
        return CFEqual(get(), other->get());
    }
    
    bool Gradient::isEqual(const Base *other) const
    {
        if(other && other->isKindOfClass<Gradient>())
            return this->isEqual((const Gradient *)other);
        
        return false;
    }
    
#pragma mark - Introspection
    
    Gradient::NativeType Gradient::get() const
    {
        return mStorage;
    }
    
#pragma mark - Drawing
    
    /* Derived from <https://github.com/larcus94/LBGradient/blob/master/LBGradient/LBGradient.m> */
    
    void Gradient::drawLinearInRect(Rect rect, Float angle)
    {
        Context::currentContext()->transaction([this, rect, angle](Context *context) {
            CGContextClipToRect(context->get(), rect);
            CGContextTranslateCTM(context->get(), rect.getMidX(), rect.getMidY());
            CGContextRotateCTM(context->get(), -(M_PI * angle / 180.0));
            CGContextTranslateCTM(context->get(), -rect.getMidX(), -rect.getMidY());
            
            CGPoint startPoint = { rect.getMinX(), rect.getMidY() };
            CGPoint endPoint = { rect.getMaxX(), rect.getMidY() };
            
            CGContextDrawLinearGradient(context->get(),
                                        this->get(),
                                        startPoint,
                                        endPoint,
                                        kCGGradientDrawsBeforeStartLocation | kCGGradientDrawsAfterEndLocation);
        });
    }
    
    void Gradient::drawLinearInPath(const Path *path, Float angle)
    {
        Context::currentContext()->transaction([this, path, angle](Context *context) {
            path->set();
            auto rect = path->boundingBox();
            
            CGContextTranslateCTM(context->get(), rect.getMidX(), rect.getMidY());
            CGContextRotateCTM(context->get(), -(M_PI * angle / 180.0));
            CGContextTranslateCTM(context->get(), -rect.getMidX(), -rect.getMidY());
            
            CGPoint startPoint{ rect.getMinX(), rect.getMidY() };
            CGPoint endPoint{ rect.getMaxX(), rect.getMidY() };
            
            CGContextDrawLinearGradient(context->get(),
                                        this->get(),
                                        startPoint,
                                        endPoint,
                                        kCGGradientDrawsBeforeStartLocation | kCGGradientDrawsAfterEndLocation);
        });
    }
    
#pragma mark -
    
    void Gradient::drawRadialInRect(Rect rect, Point relativeCenterPoint)
    {
        Context::currentContext()->transaction([this, rect, relativeCenterPoint](Context *context) {
            CGContextClipToRect(context->get(), rect);
            
            Float radius = sqrt(pow(rect.getWidth() / 2.0, 2.0) + pow(rect.getHeight() / 2.0, 2.0));
            Point startCenter{
                (rect.getWidth() * relativeCenterPoint.x),
                (rect.getHeight() * relativeCenterPoint.y)
            };
            Point endCenter{
                rect.getWidth() / 2.0,
                rect.getHeight() / 2.0
            };
            
            CGContextDrawRadialGradient(context->get(),
                                        this->get(),
                                        startCenter,
                                        0.0,
                                        endCenter,
                                        radius,
                                        kCGGradientDrawsBeforeStartLocation | kCGGradientDrawsAfterEndLocation);
        });
    }
    
    void Gradient::drawRadialInPath(const Path *path, Point relativeCenterPoint)
    {
        Context::currentContext()->transaction([this, path, relativeCenterPoint](Context *context) {
            path->set();
            auto rect = path->boundingBox();
            
            Float radius = sqrt(pow(rect.getWidth() / 2.0, 2.0) + pow(rect.getHeight() / 2.0, 2.0));
            Point startCenter{
                (rect.getWidth() * relativeCenterPoint.x),
                (rect.getHeight() * relativeCenterPoint.y)
            };
            Point endCenter{
                rect.getWidth() / 2.0,
                rect.getHeight() / 2.0
            };
            
            CGContextDrawRadialGradient(context->get(),
                                        this->get(),
                                        startCenter,
                                        0.0,
                                        endCenter,
                                        radius,
                                        kCGGradientDrawsBeforeStartLocation | kCGGradientDrawsAfterEndLocation);
        });
    }
    
#pragma mark -
    
    const Gradient::DrawingInformation &Gradient::drawingInformation() const
    {
        return mDrawingInformation;
    }
    
    void Gradient::drawInRect(Rect rect)
    {
        switch (mDrawingInformation.type()) {
            case DrawingInformation::Type::kLinearFloat: {
                this->drawLinearInRect(rect, mDrawingInformation.angle());
                break;
            }
                
            case DrawingInformation::Type::kRadialPoint: {
                this->drawRadialInRect(rect, mDrawingInformation.relativeCenterLocation());
                break;
            }
        }
    }
    
    void Gradient::drawInPath(const Path *path)
    {
        switch (mDrawingInformation.type()) {
            case DrawingInformation::Type::kLinearFloat: {
                this->drawLinearInPath(path, mDrawingInformation.angle());
                break;
            }
                
            case DrawingInformation::Type::kRadialPoint: {
                this->drawRadialInPath(path, mDrawingInformation.relativeCenterLocation());
                break;
            }
        }
    }
    
#pragma mark - Functions
    
    static void gradient_makeLinear(StackFrame *stack)
    {
        /* vec angle -- gradient */
        auto angle = stack->popNumber();
        auto info = stack->popType<Array<Base>>();
        
        auto colors = make<Array<Color>>();
        std::vector<Float> locations{};
        
        bool lookingForColor = true;
        for (Base *value : info) {
            if(lookingForColor) {
                auto color = dynamic_cast<Color *>(value);
                gfx_assert(color != nullptr, str("malformed input to `gradient`, expected color not number."));
                
                colors->append(color);
                
                lookingForColor = false;
            } else {
                auto location = dynamic_cast<Number *>(value);
                gfx_assert(location != nullptr, str("malformed input to `gradient`, expected number not color."));
                
                locations.push_back(location->value());
                
                lookingForColor = true;
            }
        }
        
        stack->push(make<Gradient>(colors, locations, angle->value()));
    }
    
    static void gradient_makeRadial(StackFrame *stack)
    {
        /* vec vec -- gradient */
        auto relativeCenterLocation = VectorToPoint(stack->popType<Array<Base>>());
        auto info = stack->popType<Array<Base>>();
        
        auto colors = make<Array<Color>>();
        std::vector<Float> locations{};
        
        bool lookingForColor = true;
        for (Base *value : info) {
            if(lookingForColor) {
                auto color = dynamic_cast<Color *>(value);
                gfx_assert(color != nullptr, str("malformed input to `gradient`, expected color not number."));
                
                colors->append(color);
                
                lookingForColor = false;
            } else {
                auto location = dynamic_cast<Number *>(value);
                gfx_assert(location != nullptr, str("malformed input to `gradient`, expected number not color."));
                
                locations.push_back(location->value());
                
                lookingForColor = true;
            }
        }
        
        stack->push(make<Gradient>(colors, locations, relativeCenterLocation));
    }
    
    static void gradient_draw(StackFrame *stack)
    {
        /* gradient vec|path -- */
        
        auto rectOrPath = stack->pop();
        auto gradient = stack->popType<Gradient>();
        if(rectOrPath->isKindOfClass<Array<Base>>()) {
            auto rect = VectorToRect(static_cast<Array<Base> *>(rectOrPath));
            gradient->drawInRect(rect);
        } else if(rectOrPath->isKindOfClass<Path>()) {
            auto path = static_cast<Path *>(rectOrPath);
            gradient->drawInPath(path);
        } else {
            throw Exception(str("unexpected type to gradient/draw"), nullptr);
        }
    }
    
#pragma mark -
    
    void Gradient::addTo(StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        frame->createVariableBinding(str("<gradient>"), str("gfx::Gradient"));
        
        frame->createFunctionBinding(str("linear-gradient"), &gradient_makeLinear);
        frame->createFunctionBinding(str("radial-gradient"), &gradient_makeRadial);
        
        frame->createFunctionBinding(str("gradient/draw"), &gradient_draw);
    }
}
