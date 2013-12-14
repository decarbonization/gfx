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
        gfx_assert(sizeVector->count() == 2, str("wrong number of numbers in vector"));
        
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
        gfx_assert(pointVector->count() == 2, str("wrong number of numbers in vector"));
        
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
            throw Exception(str("wrong number of numbers in vector for rect"), nullptr);
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
    
#if GFX_Language_SupportsFiles
    static void ctx_save(StackFrame *stack)
    {
        auto path = stack->popString();
        
        auto image = Context::currentContext()->makeImage();
        auto data = image->makeRepresentation(Image::RepresentationType::PNG);
        
        auto file = make<File>(path, File::Mode::Write);
        file->write(data);
    }
#endif /* GFX_Language_SupportsFiles */
    
#pragma mark - Layer Functions
    
    static void layer_make(StackFrame *stack)
    {
        /* vec func -- Layer */
        Scoped<Function> drawFunction = stack->popFunction();
        auto frame = VectorToRect(stack->popType<Array<Base>>());
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
    
    static void layer_addChild(StackFrame *stack)
    {
        /* layer(parent) layer(child) -- */
        auto child = stack->popType<Layer>();
        auto parent = stack->popType<Layer>();
        parent->addSublayer(child);
    }
    
    static void layer_removeAsChild(StackFrame *stack)
    {
        /* layer -- */
        auto layer = stack->popType<Layer>();
        layer->removeFromSuperlayer();
    }
    
    static void layer_parent(StackFrame *stack)
    {
        /* layer -- layer */
        auto layer = stack->popType<Layer>();
        stack->push(layer->superlayer());
    }
    
    static void layer_children(StackFrame *stack)
    {
        /* layer -- vec */
        auto layer = stack->popType<Layer>();
        stack->push(copy(layer->sublayers()));
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
    
    static void path_upTriangle(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        stack->push(Path::withTriangle(rect, Path::TriangleDirection::Up));
    }
    
    static void path_downTriangle(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        stack->push(Path::withTriangle(rect, Path::TriangleDirection::Down));
    }
    
    static void path_leftTriangle(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        stack->push(Path::withTriangle(rect, Path::TriangleDirection::Left));
    }
    
    static void path_rightTriangle(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        stack->push(Path::withTriangle(rect, Path::TriangleDirection::Right));
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
    
    void Graphics::addTo(StackFrame *frame)
    {
        //Context Functions
        frame->createFunctionBinding(str("ctx/begin"), &ctx_begin);
        frame->createFunctionBinding(str("ctx/end"), &ctx_end);
        frame->createFunctionBinding(str("ctx/size"), &ctx_size);
#if GFX_Language_SupportsFiles
        frame->createFunctionBinding(str("ctx/save"), &ctx_save);
#endif /* GFX_Language_SupportsFiles */
        
        //Layer Functions
        frame->createFunctionBinding(str("layer"), &layer_make);
        frame->createFunctionBinding(str("layer/frame"), &layer_frame);
        frame->createFunctionBinding(str("layer/set-frame"), &layer_setFrame);
        frame->createFunctionBinding(str("layer/display"), &layer_display);
        frame->createFunctionBinding(str("layer/render"), &layer_render);
        
        frame->createFunctionBinding(str("layer/add-child"), &layer_addChild);
        frame->createFunctionBinding(str("layer/remove-as-child"), &layer_removeAsChild);
        frame->createFunctionBinding(str("layer/parent"), &layer_parent);
        frame->createFunctionBinding(str("layer/children"), &layer_children);
        
        //Colors
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
        
        //Core Operations
        frame->createFunctionBinding(str("fill-rect"), &fill);
        frame->createFunctionBinding(str("stroke-rect"), &stroke);
        
        //Path Operations
        frame->createFunctionBinding(str("path/make"), &path_make);
        frame->createFunctionBinding(str("path/rect"), &path_rect);
        frame->createFunctionBinding(str("path/round-rect"), &path_roundRect);
        frame->createFunctionBinding(str("path/oval"), &path_oval);
        frame->createFunctionBinding(str("path/up-triangle"), &path_upTriangle);
        frame->createFunctionBinding(str("path/down-triangle"), &path_downTriangle);
        frame->createFunctionBinding(str("path/left-triangle"), &path_leftTriangle);
        frame->createFunctionBinding(str("path/right-triangle"), &path_rightTriangle);
        
        frame->createFunctionBinding(str("path/move"), &path_move);
        frame->createFunctionBinding(str("path/line"), &path_line);
        frame->createFunctionBinding(str("path/arc"), &path_arc);
        frame->createFunctionBinding(str("path/curve"), &path_curve);
        
        frame->createFunctionBinding(str("path/bounding-box"), &path_boundingBox);
        frame->createFunctionBinding(str("path/current-point"), &path_currentPoint);
        frame->createFunctionBinding(str("path/empty?"), &path_isEmpty);
        frame->createFunctionBinding(str("path/contains-point"), &path_containsPoint);
        
        frame->createFunctionBinding(str("path/fill"), &path_fill);
        frame->createFunctionBinding(str("path/stroke"), &path_stroke);
        
        //Core Text Operations
        frame->createFunctionBinding(str("font"), &font_make);
        
        frame->createFunctionBinding(str("text"), &text_make);
        frame->createFunctionBinding(str("text/size"), &text_size);
        frame->createFunctionBinding(str("text/draw"), &text_draw);
    }
    
#pragma mark -
    
    void Graphics::attachTo(Interpreter *interpreter)
    {
        Graphics::addTo(interpreter->currentFrame());
        interpreter->prependWordHandler([interpreter](StackFrame *currentFrame, Word *word) {
            if(word->string()->hasPrefix(str("#"))) {
                auto color = make<Color>(word->string());
                currentFrame->push(color);
                return true;
            } else {
                return false;
            }
        });
    }
}

#endif /* GFX_Include_GraphicsStack */
