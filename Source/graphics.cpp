//
//  graphics.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Include_GraphicsStack

#include "graphics.h"
#include "interpreter.h"
#include "stackframe.h"
#include "function.h"
#include "number.h"
#include "file.h"
#include "word.h"
#include "blob.h"

#include "color.h"
#include "path.h"
#include "context.h"
#include "image.h"
#include "layer.h"

#include "textline.h"
#include "font.h"

namespace gfx {
    
#pragma mark - Utility Functions
    
    Array<Base> *VectorFromSize(Size size)
    {
        return autoreleased(new Array<Base>{
            make<Number>(size.width),
            make<Number>(size.height)
        });
    }
    
    Size VectorToSize(const Array<Base> *sizeVector)
    {
        gfx_assert_param(sizeVector);
        gfx_assert(sizeVector->count() == 2, "wrong number of numbers in vector"_gfx);
        
        return Size{
            dynamic_cast_or_throw<Number *>(sizeVector->at(0))->value(),
            dynamic_cast_or_throw<Number *>(sizeVector->at(1))->value(),
        };
    }
    
    Array<Base> *VectorFromPoint(Point point)
    {
        return autoreleased(new Array<Base>{
            make<Number>(point.x),
            make<Number>(point.y)
        });
    }
    
    Point VectorToPoint(const Array<Base> *pointVector)
    {
        gfx_assert_param(pointVector);
        gfx_assert(pointVector->count() == 2, "wrong number of numbers in vector"_gfx);
        
        return Point{
            dynamic_cast_or_throw<Number *>(pointVector->at(0))->value(),
            dynamic_cast_or_throw<Number *>(pointVector->at(1))->value(),
        };
    }
    
    Array<Base> *VectorFromRect(Rect rect)
    {
        return autoreleased(new Array<Base>{
            make<Number>(rect.origin.x),
            make<Number>(rect.origin.y),
            make<Number>(rect.size.width),
            make<Number>(rect.size.height),
        });
    }
    
    Rect VectorToRect(const Array<Base> *rectVector)
    {
        gfx_assert_param(rectVector);
        
        if(rectVector->count() == 4) {
            return Rect{
                Point{
                    dynamic_cast_or_throw<Number *>(rectVector->at(0))->value(),
                    dynamic_cast_or_throw<Number *>(rectVector->at(1))->value(),
                },
                Size{
                    dynamic_cast_or_throw<Number *>(rectVector->at(2))->value(),
                    dynamic_cast_or_throw<Number *>(rectVector->at(3))->value(),
                }
            };
        } else if(rectVector->count() == 2) {
            return Rect{
                Point{
                    0.0,
                    0.0,
                },
                Size{
                    dynamic_cast_or_throw<Number *>(rectVector->at(0))->value(),
                    dynamic_cast_or_throw<Number *>(rectVector->at(1))->value(),
                }
            };
        } else {
            throw Exception("wrong number of numbers in vector for rect"_gfx, nullptr);
            return Rect{};
        }
    }
    
#pragma mark - Context Functions
    
    static void ctx_begin(StackFrame *stack)
    {
        auto sizeVector = stack->popType<Array<Base>>();
        auto size = VectorToSize(sizeVector);
        Context::pushContext(Context::bitmapContextWith(size));
    }
    
    static void ctx_end(StackFrame *stack)
    {
        Context::popContext();
    }
    
    static void ctx_size(StackFrame *stack)
    {
        auto ctxSize = Context::currentContext()->boundingRect().size;
        stack->push(VectorFromSize(ctxSize));
    }
    
    static void ctx_save(StackFrame *stack)
    {
        auto path = stack->popString();
        
        auto image = Context::currentContext()->makeImage();
        auto data = image->makeRepresentation(Image::RepresentationType::PNG);
        
        auto file = make<File>(path, File::Mode::Write);
        file->write(data);
    }
    
    static void ctx_getSize(StackFrame *stack)
    {
        /* -- vec */
        
        auto context = Context::currentContext();
        auto vector = VectorFromSize(context->boundingRect().size);
        stack->push(vector);
    }
    
#pragma mark - Layer Functions
    
    static void layer_make(StackFrame *stack)
    {
        /* vec func -- Layer */
        Scoped<Function> drawFunction = stack->popFunction();
        auto sizeOrRect = stack->popType<Array<Base>>();
        
        Rect frame = {};
        if(sizeOrRect->count() == 4) {
            frame = VectorToRect(sizeOrRect);
        } else if(sizeOrRect->count() == 2) {
            frame.size = VectorToSize(sizeOrRect);
        } else {
            throw Exception("Invalid vector given to `layer`. Must contain either 2 numbers for a size, or 4 numbers for a rect."_gfx, nullptr);
        }
        
        Interpreter *interpreter = stack->interpreter();
        stack->push(make<Layer>(frame, [interpreter, drawFunction](Layer *layer, Rect rect) {
            StackFrame *frame = interpreter->currentFrame();
            frame->push(VectorFromRect(rect));
            drawFunction->apply(frame);
            frame->safeDrop();
        }));
    }
    
    static void layer_frame(StackFrame *stack)
    {
        /* Layer -- vec */
        auto layer = stack->popType<Layer>();
        stack->push(VectorFromRect(layer->frame()));
    }
    
    static void layer_setFrame(StackFrame *stack)
    {
        /* layer vec -- */
        auto frameVector = stack->popType<Array<Base>>();
        auto layer = stack->popType<Layer>();
        layer->setFrame(VectorToRect(frameVector));
    }
    
    static void layer_display(StackFrame *stack)
    {
        /* layer -- */
        auto layer = stack->popType<Layer>();
        layer->setNeedsDisplay();
    }
    
    static void layer_render(StackFrame *stack)
    {
        /* layer -- */
        auto layer = stack->popType<Layer>();
        layer->render(Context::currentContext());
    }
    
#pragma mark - Color Functions
    
    static void rgb(StackFrame *stack)
    {
        auto blue = stack->popNumber();
        auto green = stack->popNumber();
        auto red = stack->popNumber();
        
        stack->push(make<Color>(red->value(), green->value(), blue->value(), 1.0));
    }
    
    static void rgba(StackFrame *stack)
    {
        auto alpha = stack->popNumber();
        auto blue = stack->popNumber();
        auto green = stack->popNumber();
        auto red = stack->popNumber();
        
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
        Rect rect = VectorToRect(rectVector);
        Path::fillRect(rect);
    }
    
    static void stroke(StackFrame *stack)
    {
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        Path::strokeRect(rect);
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
        Rect rect = VectorToRect(rectVector);
        stack->push(Path::withRect(rect));
    }
    
    static void path_roundRect(StackFrame *stack)
    {
        /* vec num -- path */
        auto radius = stack->popNumber();
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        stack->push(Path::withRoundedRect(rect, radius->value(), radius->value()));
    }
    
    static void path_oval(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        stack->push(Path::withOval(rect));
    }
    
#pragma mark -
    
    static void path_move(StackFrame *stack)
    {
        /* path vec -- path */
        auto point = VectorToPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->moveToPoint(point);
        stack->push(path);
    }
    
    static void path_line(StackFrame *stack)
    {
        /* path vec -- path */
        auto point = VectorToPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->lineToPoint(point);
        stack->push(path);
    }
    
    static void path_arc(StackFrame *stack)
    {
        /* path vec1 vec2 num -- path */
        auto radius = stack->popNumber();
        auto point2 = VectorToPoint(stack->popType<Array<Base>>());
        auto point1 = VectorToPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->arcToPoint(point1, point2, radius->value());
        stack->push(path);
    }
    
    static void path_curve(StackFrame *stack)
    {
        auto controlPoint2 = VectorToPoint(stack->popType<Array<Base>>());
        auto controlPoint1 = VectorToPoint(stack->popType<Array<Base>>());
        auto point = VectorToPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->curveToPoint(point, controlPoint1, controlPoint2);
        stack->push(path);
    }
    
#pragma mark -
    
    static void path_boundingBox(StackFrame *stack)
    {
        /* path -- vec */
        auto path = stack->popType<Path>();
        auto boundingBox = path->boundingBox();
        auto array = VectorFromRect(boundingBox);
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
        auto point = VectorToPoint(stack->popType<Array<Base>>());
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
    
#pragma mark - Core Text Operations
    
    static void font_make(StackFrame *stack)
    {
        /* str num -- font */
        auto size = stack->popNumber();
        auto name = stack->popString();
        stack->push(Font::withName(name, size->value()));
    }
    
#pragma mark -
    
    static void text_make(StackFrame *stack)
    {
        /* font color str -- text */
        auto string = stack->popString();
        auto color = stack->popType<Color>();
        auto font = stack->popType<Font>();
        
        auto attributes = make<Dictionary<const String, Base>>();
        attributes->set(kFontAttributeName, font);
        attributes->set(kForegroundColorAttributeName, color);
        stack->push(TextLine::withString(string, attributes));
    }
    
    static void text_size(StackFrame *stack)
    {
        /* text -- vec */
        auto text = stack->popType<TextLine>();
        auto vector = VectorFromSize(text->bounds().size);
        stack->push(vector);
    }
    
    static void text_draw(StackFrame *stack)
    {
        /* text vec -- */
        auto point = VectorToPoint(stack->popType<Array<Base>>());
        auto text = stack->popType<TextLine>();
        
        text->drawAtPoint(point);
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
        //Context Functions
        Graphics::createFunctionBinding(frame, "ctx/begin"_gfx, &ctx_begin);
        Graphics::createFunctionBinding(frame, "ctx/end"_gfx, &ctx_end);
        Graphics::createFunctionBinding(frame, "ctx/size"_gfx, &ctx_size);
        Graphics::createFunctionBinding(frame, "ctx/save"_gfx, &ctx_save);
        Graphics::createFunctionBinding(frame, "everything"_gfx, &ctx_getSize);
        
        //Layer Functions
        Graphics::createFunctionBinding(frame, "layer"_gfx, &layer_make);
        Graphics::createFunctionBinding(frame, "layer/frame"_gfx, &layer_frame);
        Graphics::createFunctionBinding(frame, "layer/set-frame"_gfx, &layer_setFrame);
        Graphics::createFunctionBinding(frame, "layer/display"_gfx, &layer_display);
        Graphics::createFunctionBinding(frame, "layer/render"_gfx, &layer_render);
        
        //Colors
        Graphics::createVariableBinding(frame, "white"_gfx, Color::white());
        Graphics::createVariableBinding(frame, "black"_gfx, Color::black());
        Graphics::createVariableBinding(frame, "translucent"_gfx, Color::clear());
        Graphics::createVariableBinding(frame, "red"_gfx, Color::red());
        Graphics::createVariableBinding(frame, "green"_gfx, Color::green());
        Graphics::createVariableBinding(frame, "blue"_gfx, Color::blue());
        Graphics::createVariableBinding(frame, "orange"_gfx, Color::orange());
        Graphics::createVariableBinding(frame, "purple"_gfx, Color::purple());
        Graphics::createVariableBinding(frame, "pink"_gfx, Color::pink());
        Graphics::createVariableBinding(frame, "brown"_gfx, Color::brown());
        Graphics::createVariableBinding(frame, "yellow"_gfx, Color::yellow());
        
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
        
        //Core Text Operations
        Graphics::createFunctionBinding(frame, "font"_gfx, &font_make);
        
        Graphics::createFunctionBinding(frame, "text"_gfx, &text_make);
        Graphics::createFunctionBinding(frame, "text/size"_gfx, &text_size);
        Graphics::createFunctionBinding(frame, "text/draw"_gfx, &text_draw);
    }
    
#pragma mark -
    
    void Graphics::attachTo(Interpreter *interpreter)
    {
        Graphics::addTo(interpreter->currentFrame());
        interpreter->setUnboundWordHandler([interpreter](Word *word) -> Color * {
            if(word->string()->hasPrefix("#"_gfx)) {
                return make<Color>(word->string());
            } else {
                interpreter->failForUnboundWord(word);
                return nullptr;
            }
        });
    }
}

#endif /* GFX_Include_GraphicsStack */
