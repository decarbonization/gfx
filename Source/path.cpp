//
//  path.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Include_GraphicsStack

#include "path.h"
#include "context.h"
#include "str.h"

#include "stackframe.h"
#include "graphics.h"
#include "number.h"

namespace gfx {
    
    Path *Path::withRect(Rect rect)
    {
        CGPathRef ovalPath = CGPathCreateWithRect(rect, NULL);
        Path *path = make<Path>(ovalPath);
        CFRelease(ovalPath);
        return path;
    }
    
    Path *Path::withRoundedRect(Rect rect, Float cornerWidth, Float cornerHeight)
    {
        CGPathRef ovalPath = CGPathCreateWithRoundedRect(rect, cornerWidth, cornerHeight, NULL);
        Path *path = make<Path>(ovalPath);
        CFRelease(ovalPath);
        return path;
    }
    
    Path *Path::withOval(Rect rect)
    {
        CGAffineTransform transform = Context::currentContext()->currentTransformationMatrix();
        CGPathRef ovalPath = CGPathCreateWithEllipseInRect(rect, &transform);
        Path *path = make<Path>(ovalPath);
        CFRelease(ovalPath);
        return path;
    }
    
    Path *Path::withTriangle(Rect rect, TriangleDirection direction)
    {
        Path *emptyPath = make<Path>();
        
        switch (direction) {
            case TriangleDirection::Up: {
                emptyPath->moveToPoint(Point{ rect.getMinX(), rect.getMaxY() });
                emptyPath->lineToPoint(Point{ rect.getMaxX(), rect.getMaxY() });
                emptyPath->lineToPoint(Point{ rect.getMidX(), rect.getMinY() });
                emptyPath->lineToPoint(Point{ rect.getMinX(), rect.getMaxY() });
                
                break;
            }
                
            case TriangleDirection::Down: {
                emptyPath->moveToPoint(Point{ rect.getMinX(), rect.getMinY() });
                emptyPath->lineToPoint(Point{ rect.getMaxX(), rect.getMinY() });
                emptyPath->lineToPoint(Point{ rect.getMidX(), rect.getMaxY() });
                emptyPath->lineToPoint(Point{ rect.getMinX(), rect.getMinY() });
                
                break;
            }
                
            case TriangleDirection::Left: {
                emptyPath->moveToPoint(Point{ rect.getMaxX(), rect.getMinY() });
                emptyPath->lineToPoint(Point{ rect.getMinX(), rect.getMidY() });
                emptyPath->lineToPoint(Point{ rect.getMaxX(), rect.getMaxY() });
                emptyPath->lineToPoint(Point{ rect.getMaxX(), rect.getMinY() });
                
                break;
            }
                
            case TriangleDirection::Right: {
                emptyPath->moveToPoint(Point{ rect.getMinX(), rect.getMinY() });
                emptyPath->lineToPoint(Point{ rect.getMaxX(), rect.getMidY() });
                emptyPath->lineToPoint(Point{ rect.getMinX(), rect.getMaxY() });
                emptyPath->lineToPoint(Point{ rect.getMinX(), rect.getMinY() });
                
                break;
            }
        }
        
        emptyPath->closePath();
        
        return emptyPath;
    }
    
#pragma mark - Lifecycle
    
    Path::Path() :
        Base(),
        mPath(CGPathCreateMutable()),
        mTransform(Transform2D::Identity),
        mLineCapStyle(LineCap::Butt),
        mLineJoinStyle(LineJoin::Miter),
        mLineWidth(1.0)
    {
        
    }
    
    Path::Path(ConstNativeType path) :
        Base(),
        mPath(CGPathCreateMutableCopy(path)),
        mTransform(Transform2D::Identity),
        mLineCapStyle(LineCap::Butt),
        mLineJoinStyle(LineJoin::Miter),
        mLineWidth(1.0)
    {
    }
    
    Path::Path(const Path *path) :
        Path(path->get())
    {
        
    }
    
    Path::~Path()
    {
        if(mPath) {
            CFRelease(mPath);
            mPath = NULL;
        }
    }
    
#pragma mark - Identity
    
    bool Path::isEqual(const Path *other) const
    {
        if(!other)
            return false;
        
        return CFEqual(get(), other->get());
    }
    
    bool Path::isEqual(const Base *other) const
    {
        if(!other)
            return false;
        
        if(other->isKindOfClass<Path>())
            return this->isEqual((const Path *)other);
        
        return false;
    }
    
    HashCode Path::hash() const
    {
        return CFHash(get());
    }
    
#pragma mark -
    
    Path::NativeType Path::get()
    {
        return mPath;
    }
    
    Path::ConstNativeType Path::get() const
    {
        return mPath;
    }
    
#pragma mark - Constructing Paths
    
    void Path::moveToPoint(Point point)
    {
        CGPathMoveToPoint(get(), &mTransform, point.x, point.y);
    }
    
    void Path::lineToPoint(Point point)
    {
        CGPathAddLineToPoint(get(), &mTransform, point.x, point.y);
    }
    
    void Path::closePath()
    {
        CGPathCloseSubpath(get());
    }
    
    void Path::addPath(const Path *otherPath)
    {
        gfx_assert_param(otherPath);
        
        CGPathAddPath(this->get(), &otherPath->mTransform, otherPath->get());
    }
    
    void Path::arcToPoint(Point point1, Point point2, Float radius)
    {
        CGPathAddArcToPoint(get(), &mTransform, point1.x, point1.y, point2.x, point2.y, radius);
    }
    
    void Path::curveToPoint(Point point, Point controlPoint1, Point controlPoint2)
    {
        CGPathAddCurveToPoint(get(), &mTransform, controlPoint1.x, controlPoint1.y, controlPoint2.x, controlPoint2.y, point.x, point.y);
    }
    
#pragma mark - Getting Information about Paths
    
    Rect Path::boundingBox() const
    {
        return CGPathGetBoundingBox(get());
    }
    
    Rect Path::pathBoundingBox() const
    {
        return CGPathGetBoundingBox(get());
    }
    
    Point Path::currentPoint() const
    {
        return CGPathGetCurrentPoint(get());
    }
    
#pragma mark -
    
    bool Path::isEmpty() const
    {
        return CGPathIsEmpty(get());
    }
    
    bool Path::isRectangle(Rect *outRect) const
    {
        return CGPathIsRect(get(), outRect);
    }
    
    bool Path::containsPoint(Point point) const
    {
        return CGPathContainsPoint(get(), &mTransform, point, false);
    }
    
#pragma mark - Drawing
    
    void Path::set() const
    {
        Context *context = Context::currentContext();
        CGContextAddPath(context->get(), this->get());
        CGContextSetLineCap(context->get(), (CGLineCap)lineCapStyle());
        CGContextSetLineJoin(context->get(), (CGLineJoin)lineJoinStyle());
        CGContextSetLineWidth(context->get(), lineWidth());
        CGContextClip(context->get());
    }
    
    void Path::fill() const
    {
        Context *context = Context::currentContext();
        context->transaction([this](Context *context) {
            CGContextAddPath(context->get(), get());
            CGContextSetLineCap(context->get(), (CGLineCap)lineCapStyle());
            CGContextSetLineJoin(context->get(), (CGLineJoin)lineJoinStyle());
            CGContextSetLineWidth(context->get(), lineWidth());
            CGContextFillPath(context->get());
        });
    }
    
    void Path::stroke() const
    {
        Context *context = Context::currentContext();
        context->transaction([this](Context *context) {
            CGContextAddPath(context->get(), get());
            CGContextSetLineCap(context->get(), (CGLineCap)lineCapStyle());
            CGContextSetLineJoin(context->get(), (CGLineJoin)lineJoinStyle());
            CGContextSetLineWidth(context->get(), lineWidth());
            CGContextStrokePath(context->get());
        });
    }
    
#pragma mark -
    
    void Path::fillRect(Rect rect)
    {
        Context *context = Context::currentContext();
        CGContextFillRect(context->get(), rect);
    }
    
    void Path::strokeRect(Rect rect)
    {
        Context *context = Context::currentContext();
        CGContextStrokeRect(context->get(), rect);
    }
    
    void Path::strokeLine(Point point1, Point point2)
    {
        Context *context = Context::currentContext();
        Point points[] = { point1, point2 };
        CGContextStrokeLineSegments(context->get(), points, 2);
    }
    
#pragma mark - Path Attributes
    
    Path::LineCap Path::lineCapStyle() const
    {
        return mLineCapStyle;
    }
    
    void Path::setLineCapStyle(LineCap capStyle)
    {
        mLineCapStyle = capStyle;
    }
    
    void Path::setDefaultLineCapStyle(LineCap capStyle)
    {
        CGContextSetLineCap(Context::currentContext()->get(), (CGLineCap)capStyle);
    }
    
#pragma mark -
    
    Path::LineJoin Path::lineJoinStyle() const
    {
        return mLineJoinStyle;
    }
    
    void Path::setLineJoinStyle(LineJoin joinStyle)
    {
        mLineJoinStyle = joinStyle;
    }
    
    void Path::setDefaultLineJoinStyle(LineJoin joinStyle)
    {
        CGContextSetLineJoin(Context::currentContext()->get(), (CGLineJoin)joinStyle);
    }
    
#pragma mark -
    
    Float Path::lineWidth() const
    {
        return mLineWidth;
    }
    
    void Path::setLineWidth(Float width)
    {
        mLineWidth = width;
    }
    
    void Path::setDefaultLineWidth(Float width)
    {
        CGContextSetLineWidth(Context::currentContext()->get(), width);
    }
    
#pragma mark -
    
    void Path::setTransform(const Transform2D &transform)
    {
        mTransform = transform;
    }
    
    Transform2D Path::transform() const
    {
        return mTransform;
    }
    
#pragma mark - Core Operations
    
    static void fn_fill(StackFrame *stack)
    {
        auto rectVector = stack->popType<Array<Base>>();
        Rect rect = VectorToRect(rectVector);
        Path::fillRect(rect);
    }
    
    static void fn_stroke(StackFrame *stack)
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
    
#pragma mark -
    
    void Path::addTo(StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        frame->createVariableBinding(str("<path>"), str("gfx::Path"));
        
        frame->createFunctionBinding(str("fill-rect"), &fn_fill);
        frame->createFunctionBinding(str("stroke-rect"), &fn_stroke);
        
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
    }
}

#endif /* GFX_Include_GraphicsStack */
