//
//  path.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__path__
#define __gfx__path__

#if GFX_Include_GraphicsStack

#include "base.h"
#include <CoreGraphics/CoreGraphics.h>

namespace gfx {
    class Path : public Base
    {
    public:
        
        typedef CGMutablePathRef NativeType;
        typedef CGPathRef ConstNativeType;
        
    protected:
        
        CGMutablePathRef mPath;
        CGAffineTransform mTransform;
        
    public:
        
        static Path *withRect(Rect rect);
        static Path *withRoundedRect(Rect rect, Float cornerWidth, Float cornerHeight);
        static Path *withOval(Rect rect);
        
#pragma mark - Lifecycle
        
        Path();
        Path(CGPathRef path);
        Path(const Path *path);
        ~Path();
        
#pragma mark - Identity
        
        virtual bool isEqual(const Path *other) const;
        virtual bool isEqual(const Base *other) const override;
        virtual HashCode hash() const override;
        
#pragma mark -
        
        NativeType get();
        ConstNativeType get() const;
        
#pragma mark - Constructing Paths
        
        void moveToPoint(Point point);
        void lineToPoint(Point point);
        void closePath();
        void addPath(const Path *path);
        void arcToPoint(Point point1, Point point2, Float radius);
        void curveToPoint(Point point, Point controlPoint1, Point controlPoint2, Float radius);
        
#pragma mark - Getting Information about Paths
        
        Rect boundingBox() const;
        Rect pathBoundingBox() const;
        Point currentPoint() const;
        
#pragma mark -
        
        bool isEmpty() const;
        bool isRectangle(Rect *outRect) const;
        bool containsPoint(Point point) const;
        
#pragma mark - Drawing
        
        void set() const;
        void fill() const;
        void stroke() const;
    };
}

#endif /* GFX_Include_GraphicsStack */

#endif /* defined(__gfx__path__) */
