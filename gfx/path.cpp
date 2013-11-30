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
    
#pragma mark - Lifecycle
    
    Path::Path() :
        Base(),
        mPath(CGPathCreateMutable()),
        mTransform(CGAffineTransformIdentity)
    {
        
    }
    
    Path::Path(CGPathRef path) :
        Base(),
        mPath(CGPathCreateMutableCopy(path)),
        mTransform(CGAffineTransformIdentity)
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
        CGPathAddPath(this->get(), &otherPath->mTransform, otherPath->get());
    }
    
    void Path::arcToPoint(Point point1, Point point2, Float radius)
    {
        CGPathAddArcToPoint(get(), &mTransform, point1.x, point1.y, point2.x, point2.y, radius);
    }
    
    void Path::curveToPoint(Point point, Point controlPoint1, Point controlPoint2, Float radius)
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
        CGContextAddPath(Context::currentContext()->get(), get());
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
}

#endif /* GFX_Include_GraphicsStack */
