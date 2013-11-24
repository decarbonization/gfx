//
//  path.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__path__
#define __gfx__path__

#include "base.h"
#include <CoreGraphics/CoreGraphics.h>

namespace gfx {
    class Path : public Base
    {
        CGMutablePathRef mPath;
        CGAffineTransform mTransform;
        
    public:
        
        static Path *withRect(CGRect rect);
        static Path *withRoundedRect(CGRect rect, CGFloat cornerWidth, CGFloat cornerHeight);
        static Path *withOval(CGRect rect);
        
#pragma mark - Lifecycle
        
        Path();
        Path(CGPathRef path);
        Path(const Path *path);
        ~Path();
        
#pragma mark - Identity
        
        virtual bool isEqual(const Path *other) const;
        virtual bool isEqual(const Base *other) const override;
        virtual CFHashCode hash() const override;
        
#pragma mark -
        
        CGMutablePathRef getPath();
        CGPathRef getPath() const;
        
#pragma mark - Constructing Paths
        
        void moveToPoint(CGPoint point);
        void lineToPoint(CGPoint point);
        void closePath();
        void addPath(const Path *path);
        void arcToPoint(CGPoint point1, CGPoint point2, CGFloat radius);
        void curveToPoint(CGPoint point, CGPoint controlPoint1, CGPoint controlPoint2, CGFloat radius);
        
#pragma mark - Getting Information about Paths
        
        CGRect boundingBox() const;
        CGRect pathBoundingBox() const;
        CGPoint currentPoint() const;
        
#pragma mark -
        
        bool isEmpty() const;
        bool isRectangle(CGRect *outRect) const;
        bool containsPoint(CGPoint point) const;
        
#pragma mark - Drawing
        
        void fill() const;
        void stroke() const;
    };
}

#endif /* defined(__gfx__path__) */
