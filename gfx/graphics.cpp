//
//  graphics.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "graphics.h"
#include "interpreter.h"
#include "stackframe.h"
#include "function.h"
#include "array.h"
#include "number.h"
#include "file.h"

#include "color.h"
#include "path.h"
#include <ImageIO/ImageIO.h>

namespace gfx {
    
#pragma mark - Context Stack
    
    static CFMutableArrayRef GetContextStack()
    {
        static CFMutableArrayRef array;
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            array = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        });
        
        return array;
    }
    
    void Graphics::pushContext(CGContextRef context)
    {
        gfx_assert_param(context);
        
        CFArrayAppendValue(GetContextStack(), context);
    }
    
    void Graphics::popContext()
    {
        CFArrayRemoveValueAtIndex(GetContextStack(), CFArrayGetCount(GetContextStack()) - 1);
    }
    
    CGContextRef Graphics::getCurrentContext()
    {
        return (CGContextRef)CFArrayGetValueAtIndex(GetContextStack(), CFArrayGetCount(GetContextStack()) - 1);
    }
    
#pragma mark -
    
    void Graphics::beginContext(CGSize size, CGFloat scale)
    {
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef imageContext = CGBitmapContextCreate(/* in data */ NULL,
                                                          /* in width */ size.width * scale,
                                                          /* in height */ size.height * scale,
                                                          /* in bitsPerComponent */ 8,
                                                          /* in bytesPerRow */ 0,
                                                          /* in colorSpace */ colorSpace,
                                                          /* in bitmapInfo */ (CGBitmapInfo)kCGImageAlphaPremultipliedLast);
        
        CGContextTranslateCTM(imageContext, 0.0, size.height * scale);
        CGContextScaleCTM(imageContext, scale, -scale);
        
        pushContext(imageContext);
        
        CGColorSpaceRelease(colorSpace);
        CGContextRelease(imageContext);
    }
    
    void Graphics::endContext()
    {
        popContext();
    }
    
    CGImageRef Graphics::createImageFromCurrentContext()
    {
        return CGBitmapContextCreateImage(Graphics::getCurrentContext());
    }
    
#pragma mark - Utility Functions
    
    namespace {
        CGSize vectorToCGSize(const Array<Base> *sizeVector)
        {
            gfx_assert_param(sizeVector);
            gfx_assert(sizeVector->count() == 2, "wrong number of numbers in vector"_gfx);
            
            return CGSizeMake(dynamic_cast_or_throw<Number *>(sizeVector->at(0))->value(),
                              dynamic_cast_or_throw<Number *>(sizeVector->at(1))->value());
        }
        
        CGPoint vectorToCGPoint(const Array<Base> *pointVector)
        {
            gfx_assert_param(pointVector);
            gfx_assert(pointVector->count() == 2, "wrong number of numbers in vector"_gfx);
            
            return CGPointMake(dynamic_cast_or_throw<Number *>(pointVector->at(0))->value(),
                               dynamic_cast_or_throw<Number *>(pointVector->at(1))->value());
        }
        
        CGRect vectorToCGRect(const Array<Base> *rectVector)
        {
            gfx_assert_param(rectVector);
            gfx_assert(rectVector->count() == 4, "wrong number of numbers in vector"_gfx);
            
            return CGRectMake(dynamic_cast_or_throw<Number *>(rectVector->at(0))->value(),
                              dynamic_cast_or_throw<Number *>(rectVector->at(1))->value(),
                              dynamic_cast_or_throw<Number *>(rectVector->at(2))->value(),
                              dynamic_cast_or_throw<Number *>(rectVector->at(3))->value());
        }
    }
    
#pragma mark - Canvas Functions
    
    static void canvas_begin(StackFrame *stack)
    {
        auto sizeVector = stack->popType<Array<Base>>();
        CGSize size = vectorToCGSize(sizeVector);
        Graphics::beginContext(size);
    }
    
    static void canvas_end(StackFrame *stack)
    {
        Graphics::endContext();
    }
    
    static void canvas_save(StackFrame *stack)
    {
        String *path = stack->popString();
        
        cf::AutoRef<CGImageRef> image = Graphics::createImageFromCurrentContext();
        
        cf::AutoRef<CFMutableDataRef> data = CFDataCreateMutable(kCFAllocatorDefault, 0);
        cf::AutoRef<CGImageDestinationRef> destination = CGImageDestinationCreateWithData(data, CFSTR("public.png"), 1, NULL);
        CGImageDestinationAddImage(destination, image, NULL);
        
        gfx_assert(CGImageDestinationFinalize(destination), "could not rasterize canvas to save it"_gfx);
        
        auto file = make<File>(path, "w");
        file->write(CFDataGetBytePtr(data), CFDataGetLength(data));
    }
    
#pragma mark - Color Functions
    
    static void rgb(StackFrame *stack)
    {
        Number *blue = stack->popNumber();
        Number *green = stack->popNumber();
        Number *red = stack->popNumber();
        
        stack->push(make<Color>(red->value(), green->value(), blue->value(), 1.0));
    }
    
    static void rgba(StackFrame *stack)
    {
        Number *alpha = stack->popNumber();
        Number *blue = stack->popNumber();
        Number *green = stack->popNumber();
        Number *red = stack->popNumber();
        
        stack->push(make<Color>(red->value(), green->value(), blue->value(), alpha->value()));
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
    
#pragma mark - Core Operations
    
    static void fill(StackFrame *stack)
    {
        auto rectVector = stack->popType<Array<Base>>();
        CGRect rect = vectorToCGRect(rectVector);
        CGContextFillRect(Graphics::getCurrentContext(), rect);
    }
    
    static void stroke(StackFrame *stack)
    {
        auto rectVector = stack->popType<Array<Base>>();
        CGRect rect = vectorToCGRect(rectVector);
        CGContextStrokeRect(Graphics::getCurrentContext(), rect);
    }
    
#pragma mark - Paths
    
    static void path_make(StackFrame *stack)
    {
        /* -- path */
        stack->push(make<Path>());
    }
    
    static void path_rect(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        CGRect rect = vectorToCGRect(rectVector);
        stack->push(Path::withRect(rect));
    }
    
    static void path_roundRect(StackFrame *stack)
    {
        /* vec num -- path */
        auto radius = stack->popNumber();
        auto rectVector = stack->popType<Array<Base>>();
        CGRect rect = vectorToCGRect(rectVector);
        stack->push(Path::withRoundedRect(rect, radius->value(), radius->value()));
    }
    
    static void path_oval(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        CGRect rect = vectorToCGRect(rectVector);
        stack->push(Path::withOval(rect));
    }
    
#pragma mark -
    
    static void path_move(StackFrame *stack)
    {
        /* path vec -- path */
        auto point = vectorToCGPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->moveToPoint(point);
        stack->push(path);
    }
    
    static void path_line(StackFrame *stack)
    {
        /* path vec -- path */
        auto point = vectorToCGPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->lineToPoint(point);
        stack->push(path);
    }
    
    static void path_arc(StackFrame *stack)
    {
        /* path vec1 vec2 num -- path */
        auto radius = stack->popNumber();
        auto point2 = vectorToCGPoint(stack->popType<Array<Base>>());
        auto point1 = vectorToCGPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->arcToPoint(point1, point2, radius->value());
        stack->push(path);
    }
    
    static void path_curve(StackFrame *stack)
    {
        auto radius = stack->popNumber();
        auto controlPoint2 = vectorToCGPoint(stack->popType<Array<Base>>());
        auto controlPoint1 = vectorToCGPoint(stack->popType<Array<Base>>());
        auto point = vectorToCGPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->curveToPoint(point, controlPoint1, controlPoint2, radius->value());
        stack->push(path);
    }
    
#pragma mark -
    
    static void path_boundingBox(StackFrame *stack)
    {
        /* path -- vec */
        auto path = stack->popType<Path>();
        auto boundingBox = path->boundingBox();
        auto array = autoreleased(new Array<Base>{
            make<Number>(CGRectGetMinX(boundingBox)),
            make<Number>(CGRectGetMinY(boundingBox)),
            make<Number>(CGRectGetWidth(boundingBox)),
            make<Number>(CGRectGetHeight(boundingBox)),
        });
        stack->push(array);
    }
    
    static void path_currentPoint(StackFrame *stack)
    {
        /* path -- vec */
        auto path = stack->popType<Path>();
        auto currentPoint = path->currentPoint();
        auto array = autoreleased(new Array<Base>{
            make<Number>(currentPoint.x),
            make<Number>(currentPoint.y),
        });
        stack->push(array);
    }
    
    static void path_isEmpty(StackFrame *stack)
    {
        /* path -- bool */
        auto path = stack->popType<Path>();
        if(path->isEmpty())
            stack->push(Number::True());
        else
            stack->push(Number::False());
    }
    
    static void path_containsPoint(StackFrame *stack)
    {
        /* path vec -- bool */
        auto point = vectorToCGPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        if(path->containsPoint(point))
            stack->push(Number::True());
        else
            stack->push(Number::False());
    }
    
#pragma mark -
    
    static void path_fill(StackFrame *stack)
    {
        /* path -- */
        auto path = stack->popType<Path>();
        path->fill();
    }
    
    static void path_stroke(StackFrame *stack)
    {
        /* path -- */
        auto path = stack->popType<Path>();
        path->stroke();
    }
    
#pragma mark - Public Interface
    
    void Graphics::createVariableBinding(StackFrame *frame, const String *name, Base *value)
    {
        frame->createBindingWithValue(name, value);
    }
    
    void Graphics::createFunctionBinding(StackFrame *frame, const String *name, std::function<void(StackFrame *stack)> implementation)
    {
        frame->createBindingWithValue(name, make<NativeFunction>(name, implementation));
    }
    
    void Graphics::addTo(StackFrame *frame)
    {
        //Canvas Functions
        Graphics::createFunctionBinding(frame, "canvas/begin"_gfx, &canvas_begin);
        Graphics::createFunctionBinding(frame, "canvas/end"_gfx, &canvas_end);
        Graphics::createFunctionBinding(frame, "canvas/save"_gfx, &canvas_save);
        
        //Colors
        Graphics::createFunctionBinding(frame, "rgb"_gfx, &rgb);
        Graphics::createFunctionBinding(frame, "rgba"_gfx, &rgba);
        Graphics::createFunctionBinding(frame, "set-fill"_gfx, &set_fill);
        Graphics::createFunctionBinding(frame, "set-stroke"_gfx, &set_stroke);
        
        //Core Operations
        Graphics::createFunctionBinding(frame, "fill"_gfx, &fill);
        Graphics::createFunctionBinding(frame, "stroke"_gfx, &stroke);
        
        //Path Operations
        Graphics::createFunctionBinding(frame, "path/make"_gfx, &path_make);
        Graphics::createFunctionBinding(frame, "path/rect"_gfx, &path_rect);
        Graphics::createFunctionBinding(frame, "path/round-rect"_gfx, &path_roundRect);
        Graphics::createFunctionBinding(frame, "path/oval"_gfx, &path_oval);
        
        Graphics::createFunctionBinding(frame, "path/move"_gfx, &path_move);
        Graphics::createFunctionBinding(frame, "path/line"_gfx, &path_line);
        Graphics::createFunctionBinding(frame, "path/arc"_gfx, &path_arc);
        Graphics::createFunctionBinding(frame, "path/curve"_gfx, &path_curve);
        
        Graphics::createFunctionBinding(frame, "path/bounding-box"_gfx, &path_boundingBox);
        Graphics::createFunctionBinding(frame, "path/current-point"_gfx, &path_currentPoint);
        Graphics::createFunctionBinding(frame, "path/empty?"_gfx, &path_isEmpty);
        Graphics::createFunctionBinding(frame, "path/contains-point"_gfx, &path_containsPoint);
        
        Graphics::createFunctionBinding(frame, "path/fill"_gfx, &path_fill);
        Graphics::createFunctionBinding(frame, "path/stroke"_gfx, &path_stroke);
    }
}
