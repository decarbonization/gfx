//
//  path.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "path.h"
#include "context.h"

namespace gfx {
    
    Path *Path::withRect(CGRect rect)
    {
        CGPathRef ovalPath = CGPathCreateWithRect(rect, NULL);
        Path *path = make<Path>(ovalPath);
        CFRelease(ovalPath);
        return path;
    }
    
    Path *Path::withRoundedRect(CGRect rect, CGFloat cornerWidth, CGFloat cornerHeight)
    {
        CGPathRef ovalPath = CGPathCreateWithRoundedRect(rect, cornerWidth, cornerHeight, NULL);
        Path *path = make<Path>(ovalPath);
        CFRelease(ovalPath);
        return path;
    }
    
    Path *Path::withOval(CGRect rect)
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
        Path(path->getPath())
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
        
        return CFEqual(getPath(), other->getPath());
    }
    
    bool Path::isEqual(const Base *other) const
    {
        if(!other)
            return false;
        
        if(other->isKindOfClass<Path>())
            return this->isEqual((const Path *)other);
        
        return false;
    }
    
    CFHashCode Path::hash() const
    {
        return CFHash(getPath());
    }
    
#pragma mark -
    
    CGMutablePathRef Path::getPath()
    {
        return mPath;
    }
    
    CGPathRef Path::getPath() const
    {
        return mPath;
    }
    
#pragma mark - Constructing Paths
    
    void Path::moveToPoint(CGPoint point)
    {
        CGPathMoveToPoint(getPath(), &mTransform, point.x, point.y);
    }
    
    void Path::lineToPoint(CGPoint point)
    {
        CGPathAddLineToPoint(getPath(), &mTransform, point.x, point.y);
    }
    
    void Path::closePath()
    {
        CGPathCloseSubpath(getPath());
    }
    
    void Path::addPath(const Path *otherPath)
    {
        CGPathAddPath(this->getPath(), &otherPath->mTransform, otherPath->getPath());
    }
    
    void Path::arcToPoint(CGPoint point1, CGPoint point2, CGFloat radius)
    {
        CGPathAddArcToPoint(getPath(), &mTransform, point1.x, point1.y, point2.x, point2.y, radius);
    }
    
    void Path::curveToPoint(CGPoint point, CGPoint controlPoint1, CGPoint controlPoint2, CGFloat radius)
    {
        CGPathAddCurveToPoint(getPath(), &mTransform, controlPoint1.x, controlPoint1.y, controlPoint2.x, controlPoint2.y, point.x, point.y);
    }
    
#pragma mark - Getting Information about Paths
    
    CGRect Path::boundingBox() const
    {
        return CGPathGetBoundingBox(getPath());
    }
    
    CGRect Path::pathBoundingBox() const
    {
        return CGPathGetBoundingBox(getPath());
    }
    
    CGPoint Path::currentPoint() const
    {
        return CGPathGetCurrentPoint(getPath());
    }
    
#pragma mark -
    
    bool Path::isEmpty() const
    {
        return CGPathIsEmpty(getPath());
    }
    
    bool Path::isRectangle(CGRect *outRect) const
    {
        return CGPathIsRect(getPath(), outRect);
    }
    
    bool Path::containsPoint(CGPoint point) const
    {
        return CGPathContainsPoint(getPath(), &mTransform, point, false);
    }
    
#pragma mark - Drawing
    
    void Path::set() const
    {
        CGContextAddPath(Context::currentContext()->getContext(), getPath());
    }
    
    void Path::fill() const
    {
        Context *context = Context::currentContext();
        context->transaction([this](Context *context) {
            set();
            CGContextFillPath(context->getContext());
        });
    }
    
    void Path::stroke() const
    {
        Context *context = Context::currentContext();
        context->transaction([this](Context *context) {
            set();
            CGContextStrokePath(context->getContext());
        });
    }
}
