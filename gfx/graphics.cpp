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
#include "array.h"
#include "number.h"
#include "file.h"
#include "word.h"
#include "blob.h"

#include "color.h"
#include "path.h"
#include "context.h"
#include "image.h"
#include "layer.h"

namespace gfx {
#pragma mark - Utility Functions
    
    namespace {
        Array<Base> *vectorFromSize(Size size)
        {
            return autoreleased(new Array<Base>{
                make<Number>(size.width),
                make<Number>(size.height)
            });
        }
        
        Size vectorToSize(const Array<Base> *sizeVector)
        {
            gfx_assert_param(sizeVector);
            gfx_assert(sizeVector->count() == 2, "wrong number of numbers in vector"_gfx);
            
            return Size{
                dynamic_cast_or_throw<Number *>(sizeVector->at(0))->value(),
                dynamic_cast_or_throw<Number *>(sizeVector->at(1))->value(),
            };
        }
        
        Array<Base> *vectorFromPoint(Point point)
        {
            return autoreleased(new Array<Base>{
                make<Number>(point.x),
                make<Number>(point.y)
            });
        }
        
        Point vectorToPoint(const Array<Base> *pointVector)
        {
            gfx_assert_param(pointVector);
            gfx_assert(pointVector->count() == 2, "wrong number of numbers in vector"_gfx);
            
            return Point{
                dynamic_cast_or_throw<Number *>(pointVector->at(0))->value(),
                dynamic_cast_or_throw<Number *>(pointVector->at(1))->value(),
            };
        }
        
        Array<Base> *vectorFromRect(Rect rect)
        {
            return autoreleased(new Array<Base>{
                make<Number>(rect.origin.x),
                make<Number>(rect.origin.y),
                make<Number>(rect.size.width),
                make<Number>(rect.size.height),
            });
        }
        
        Rect vectorToRect(const Array<Base> *rectVector)
        {
            gfx_assert_param(rectVector);
            gfx_assert(rectVector->count() == 4, "wrong number of numbers in vector"_gfx);
            
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
        }
    }
    
#pragma mark - Canvas Functions
    
    static void canvas_begin(StackFrame *stack)
    {
        auto sizeVector = stack->popType<Array<Base>>();
        Size size = vectorToSize(sizeVector);
        Context::pushContext(Context::bitmapContextWith(size));
    }
    
    static void canvas_end(StackFrame *stack)
    {
        Context::popContext();
    }
    
    static void canvas_size(StackFrame *stack)
    {
        auto canvasSize = Context::currentContext()->boundingRect().size;
        stack->push(vectorFromSize(canvasSize));
    }
    
    static void canvas_save(StackFrame *stack)
    {
        String *path = stack->popString();
        
        Image *image = Context::currentContext()->makeImage();
        Blob *data = image->makeRepresentation(Image::RepresentationType::PNG);
        
        auto file = make<File>(path, File::Mode::Write);
        file->write(data);
    }
    
#pragma mark - Layer Functions
    
    namespace {
        ///The FunctionDrawAdaptor class is a wrapper around a Function and Interpreter
        ///object that takes ownership of its parameters so that it can be safely used
        ///with an `std::function` adaptor.
        class DrawFunctionAdaptor
        {
            ///The interpreter that `mFunction` is to be evaluated with. Unsafe weak reference.
            Interpreter *mInterpreter;
            
            ///The implementation of the DrawFunction.
            Function *mFunction;
            
        public:
            
            ///Constructs an adaptor from a given interpreter and function,
            ///taking ownership of their lifecycle.
            DrawFunctionAdaptor(Interpreter *interpreter, Function *function) :
                mInterpreter(interpreter),
                mFunction(retained(function))
            {
            }
            
            ///Copies the contents of one adaptor into another.
            DrawFunctionAdaptor(const DrawFunctionAdaptor &other) :
                DrawFunctionAdaptor(other.mInterpreter, other.mFunction)
            {
            }
            
            ///The destructor.
            ~DrawFunctionAdaptor()
            {
                released(mFunction);
            }
            
            ///Performs the necessary glue to forward drawing responsibility into `mFunction`.
            void operator() (Layer *layer, Rect rect)
            {
                StackFrame *frame = mInterpreter->currentFrame();
                frame->push(vectorFromRect(rect));
                mFunction->apply(frame);
                frame->safeDrop();
            }
        };
    };
    
    static void layer_make(StackFrame *stack)
    {
        /* vec func -- Layer */
        auto drawFunction = stack->popFunction();
        auto sizeOrRect = stack->popType<Array<Base>>();
        
        Rect frame = {};
        if(sizeOrRect->count() == 4) {
            frame = vectorToRect(sizeOrRect);
        } else if(sizeOrRect->count() == 2) {
            frame.size = vectorToSize(sizeOrRect);
        } else {
            throw Exception("Invalid vector given to `layer`. Must contain either 2 numbers for a size, or 4 numbers for a rect."_gfx, nullptr);
        }
        
        //We would use a lambda here, except that the adaptor can outlive
        //the containing autorelease pool that `drawFunction`'s lifecycle
        //currently belongs to.
        stack->push(make<Layer>(frame, DrawFunctionAdaptor(stack->interpreter(), drawFunction)));
    }
    
    static void layer_frame(StackFrame *stack)
    {
        /* Layer -- vec */
        auto layer = stack->popType<Layer>();
        stack->push(vectorFromRect(layer->frame()));
    }
    
    static void layer_setFrame(StackFrame *stack)
    {
        /* layer vec -- */
        auto frameVector = stack->popType<Array<Base>>();
        auto layer = stack->popType<Layer>();
        layer->setFrame(vectorToRect(frameVector));
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
        Rect rect = vectorToRect(rectVector);
        Path::fillRect(rect);
    }
    
    static void stroke(StackFrame *stack)
    {
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = vectorToRect(rectVector);
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
        Rect rect = vectorToRect(rectVector);
        stack->push(Path::withRect(rect));
    }
    
    static void path_roundRect(StackFrame *stack)
    {
        /* vec num -- path */
        auto radius = stack->popNumber();
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = vectorToRect(rectVector);
        stack->push(Path::withRoundedRect(rect, radius->value(), radius->value()));
    }
    
    static void path_oval(StackFrame *stack)
    {
        /* vec -- path */
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = vectorToRect(rectVector);
        stack->push(Path::withOval(rect));
    }
    
#pragma mark -
    
    static void path_move(StackFrame *stack)
    {
        /* path vec -- path */
        auto point = vectorToPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->moveToPoint(point);
        stack->push(path);
    }
    
    static void path_line(StackFrame *stack)
    {
        /* path vec -- path */
        auto point = vectorToPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->lineToPoint(point);
        stack->push(path);
    }
    
    static void path_arc(StackFrame *stack)
    {
        /* path vec1 vec2 num -- path */
        auto radius = stack->popNumber();
        auto point2 = vectorToPoint(stack->popType<Array<Base>>());
        auto point1 = vectorToPoint(stack->popType<Array<Base>>());
        auto path = stack->popType<Path>();
        path->arcToPoint(point1, point2, radius->value());
        stack->push(path);
    }
    
    static void path_curve(StackFrame *stack)
    {
        auto controlPoint2 = vectorToPoint(stack->popType<Array<Base>>());
        auto controlPoint1 = vectorToPoint(stack->popType<Array<Base>>());
        auto point = vectorToPoint(stack->popType<Array<Base>>());
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
        auto array = vectorFromRect(boundingBox);
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
        auto point = vectorToPoint(stack->popType<Array<Base>>());
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
        Graphics::createFunctionBinding(frame, "canvas/size"_gfx, &canvas_size);
        Graphics::createFunctionBinding(frame, "canvas/save"_gfx, &canvas_save);
        
        //Layer Functions
        Graphics::createFunctionBinding(frame, "layer"_gfx, &layer_make);
        Graphics::createFunctionBinding(frame, "layer/frame"_gfx, &layer_frame);
        Graphics::createFunctionBinding(frame, "layer/set-frame"_gfx, &layer_setFrame);
        Graphics::createFunctionBinding(frame, "layer/display"_gfx, &layer_display);
        Graphics::createFunctionBinding(frame, "layer/render"_gfx, &layer_render);
        
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
