//
//  path.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "path.h"
#include "graphics.h"

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
        Path(path->path())
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
        
        return CFEqual(path(), other->path());
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
        return CFHash(path());
    }
    
#pragma mark -
    
    CGMutablePathRef Path::path()
    {
        return mPath;
    }
    
    CGPathRef Path::path() const
    {
        return mPath;
    }
    
#pragma mark - Constructing Paths
    
    void Path::moveToPoint(CGPoint point)
    {
        CGPathMoveToPoint(path(), &mTransform, point.x, point.y);
    }
    
    void Path::lineToPoint(CGPoint point)
    {
        CGPathAddLineToPoint(path(), &mTransform, point.x, point.y);
    }
    
    void Path::closePath()
    {
        CGPathCloseSubpath(path());
    }
    
    void Path::addPath(const Path *otherPath)
    {
        CGPathAddPath(this->path(), &otherPath->mTransform, otherPath->path());
    }
    
    void Path::arcToPoint(CGPoint point1, CGPoint point2, CGFloat radius)
    {
        CGPathAddArcToPoint(path(), &mTransform, point1.x, point1.y, point2.x, point2.y, radius);
    }
    
    void Path::curveToPoint(CGPoint point, CGPoint controlPoint1, CGPoint controlPoint2, CGFloat radius)
    {
        CGPathAddCurveToPoint(path(), &mTransform, controlPoint1.x, controlPoint1.y, controlPoint2.x, controlPoint2.y, point.x, point.y);
    }
    
#pragma mark - Getting Information about Paths
    
    CGRect Path::boundingBox() const
    {
        return CGPathGetBoundingBox(path());
    }
    
    CGRect Path::pathBoundingBox() const
    {
        return CGPathGetBoundingBox(path());
    }
    
    CGPoint Path::currentPoint() const
    {
        return CGPathGetCurrentPoint(path());
    }
    
#pragma mark -
    
    bool Path::isEmpty() const
    {
        return CGPathIsEmpty(path());
    }
    
    bool Path::isRectangle(CGRect *outRect) const
    {
        return CGPathIsRect(path(), outRect);
    }
    
    bool Path::containsPoint(CGPoint point) const
    {
        return CGPathContainsPoint(path(), &mTransform, point, false);
    }
    
#pragma mark - Drawing
    
    void Path::fill() const
    {
        CGContextRef context = Graphics::getCurrentContext();
        CGContextSaveGState(context);
        {
            CGContextAddPath(context, path());
            CGContextFillPath(context);
        }
        CGContextRestoreGState(context);
    }
    
    void Path::stroke() const
    {
        CGContextRef context = Graphics::getCurrentContext();
        CGContextSaveGState(context);
        {
            CGContextAddPath(context, path());
            CGContextStrokePath(context);
        }
        CGContextRestoreGState(context);
    }
}
