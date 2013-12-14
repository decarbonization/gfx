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
        CGPathRef ovalPath = CGPathCreateWithEllipseInRect(rect, NULL);
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
        mTransform(CGAffineTransformIdentity),
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
    }
    
    void Path::fill() const
    {
        Context *context = Context::currentContext();
        context->transaction([this](Context *context) {
            set();
            CGContextFillPath(context->get());
        });
    }
    
    void Path::stroke() const
    {
        Context *context = Context::currentContext();
        context->transaction([this](Context *context) {
            set();
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
}

#endif /* GFX_Include_GraphicsStack */
