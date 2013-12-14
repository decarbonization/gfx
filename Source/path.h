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
    class StackFrame;
    
    ///The Path class encapsulates a bézier path and the basic operations necessary to operate on it.
    ///
    ///The underlying type of the Path class is platform dependent. On iOS and OS X, it is a CGMutablePath.
    class Path : public Base
    {
    public:
        
        ///The underlying native type used by Path.
        typedef CGMutablePathRef NativeType;
        
        ///The immutable variant of the underlying native type used by Path.
        typedef CGPathRef ConstNativeType;
        
        ///Junction types for stroked lines.
        enum class LineJoin {
            ///A join with a sharp angled corner. Default.
            Miter = kCGLineJoinMiter,
            
            ///A join with a rounded corner.
            Round = kCGLineJoinRound,
            
            ///A join with a squared end.
            Bevel = kCGLineJoinBevel,
        };
        
        ///Line cap types for stroked lines.
        enum class LineCap {
            ///A line with a square end. Default.
            Butt    = kCGLineCapButt,
            
            ///A line with a rounded end.
            Round   = kCGLineCapRound,
        };
        
        ///The different directions for the point of a triangle.
        enum class TriangleDirection {
            ///The triangle point is on the top.
            Up,
            
            ///The triangle point is on the bottom.
            Down,
            
            ///The triangle point is on the left.
            Left,
            
            ///The triangle point is on the right.
            Right,
        };
        
    protected:
        
        ///The object the Path class is wrapping and operating on.
        NativeType mPath;
        
        ///The transform applicable to the path.
        Transform2D mTransform;
        
        LineCap mLineCapStyle;
        LineJoin mLineJoinStyle;
        Float mLineWidth;
        
    public:
        
#pragma mark - Creating Paths
        
        ///Creates a new autoreleased `gfx::Path` object that
        ///will fill / stroke a given rectangle area.
        ///
        /// \param  rect    A rectangle within the coordinate system of the Context that will render the Path.
        ///
        /// \result A new autoreleased Path object ready for use.
        ///
        static Path *withRect(Rect rect);
        
        ///Creates a new autoreleased `gfx::Path` object that will
        ///fill / stroke a given rectangle area with rounded corners.
        ///
        /// \param  rect    A rectangle within the coordinate system of the Context that will render the Path.
        /// \param  cornerWidth The width of the rounded corners of the rectangle to be rendered.
        /// \param  cornerWidth The height of the rounded corners of the rectangle to be rendered.
        ///
        /// \result A new autoreleased Path object ready for use.
        ///
        static Path *withRoundedRect(Rect rect, Float cornerWidth, Float cornerHeight);
        
        ///Creates a new autoreleased `gfx::Path` object that
        ///will fill / stroke a given area with an oval.
        ///
        /// \param  rect    A rectangle within the coordinate system of the Context that will render the Path.
        ///
        /// \result A new autoreleased Path object ready for use.
        ///
        static Path *withOval(Rect rect);
        
        ///Creates a new autoreleased `gfx::Path` object that
        ///will fill / stroke a given area with a triangle that
        ///points in a given direection.
        ///
        /// \param  rect        A rectangle within the coordinate system of the Context that will render the path.
        /// \param  direction   The direction of the triangle.
        ///
        /// \result A new autoreleased Path object ready for use.
        ///
        /// \seealso(gfx::Path::TriangleDirection)
        static Path *withTriangle(Rect rect, TriangleDirection direction);
        
#pragma mark - Lifecycle
        
        ///Constructs an empty path.
        Path();
        
        ///Constructs a path from a given native path object.
        ///
        /// \param  path    The native path.
        ///
        Path(ConstNativeType path);
        
        ///Constructs a path by copying the contents of another path.
        ///
        /// \param  path    The path whose contents should be copied.
        ///
        Path(const Path *path);
        
        ///The destructor.
        ~Path();
        
#pragma mark - Identity
        
        virtual bool isEqual(const Path *other) const;
        virtual bool isEqual(const Base *other) const override;
        virtual HashCode hash() const override;
        
#pragma mark -
        
        ///Returns the underlying native path of the receiver.
        NativeType get();
        
        ///Returns the immutable underlying native path of the receiver.
        ConstNativeType get() const;
        
#pragma mark - Constructing Paths
        
        ///Moves the path's current point to a new location.
        ///
        /// \param  point   A coordinate in the current system.
        ///
        void moveToPoint(Point point);
        
        ///Creates a line from the current point to a given location.
        ///
        /// \param  point   The end location of the line segment, relative to the path's current point.
        ///
        void lineToPoint(Point point);
        
        ///Closes the most recently added subpath of the receiver.
        void closePath();
        
        ///Adds a given path as a subpath of the receiver.
        ///
        /// \param  path    The path to copy into the receiver as a subpath. May not be null.
        ///
        void addPath(const Path *path);
        
        ///Creates a new arc subpath between two given points, with a given radius.
        ///
        /// \param  point1  The first point.
        /// \param  point2  The second point.
        /// \param  radius  The radius to apply to the arc, in radians.
        ///
        void arcToPoint(Point point1, Point point2, Float radius);
        
        ///Creates a new curve subpath.
        ///
        /// \param  point           The end point of the curve.
        /// \param  controlPoint1   The first control point.
        /// \param  controlPoint2   The second control point.
        ///
        void curveToPoint(Point point, Point controlPoint1, Point controlPoint2);
        
#pragma mark - Getting Information about Paths
        
        ///Returns the bounding box containing all points within the path.
        Rect boundingBox() const;
        
        ///Returns the smallest bounding box containing all points within the path.
        Rect pathBoundingBox() const;
        
        ///Returns the current point of the path.
        Point currentPoint() const;
        
#pragma mark -
        
        ///Returns a bool indicating whether or not the path is empty.
        bool isEmpty() const;
        
        ///Returns a bool indicating whether or not the path is a
        ///rectangle shape, yielding the rectangle's area in an out param.
        ///
        /// \param  outRect On return, contains the rectangle's area. May be null.
        ///
        bool isRectangle(Rect *outRect) const;
        
        ///Returns a bool indicating whether or not a given point is contained within the path.
        ///
        /// \param  point   A coordinate within the current system.
        ///
        /// \result true if the point is within the path; false otherwise.
        bool containsPoint(Point point) const;
        
#pragma mark - Drawing
        
        ///Sets the receiver to the current `gfx::Context`'s drawing path.
        ///
        ///This method should always be invoked from within a context transaction.
        void set() const;
        
        ///Paints the area within the path in the current `gfx::Context`.
        void fill() const;
        
        ///Paints the line segments of the path in the current `gfx::Context`.
        void stroke() const;
        
#pragma mark -
        
        ///Fills the specified rectangle with the current fill `gfx::Color`.
        ///
        /// \param  rect    A rectangle in the current coordinate system.
        ///
        static void fillRect(Rect rect);
        
        ///Strokes the specified rectangle with the current stroke `gfx::Color`.
        ///
        /// \param  rect    A rectangle in the current coordinate system.
        ///
        static void strokeRect(Rect rect);
        
        ///Strokes a line between between two given points.
        ///
        /// \param  point1  The first point in the current coordinate system.
        /// \param  point2  The second point in the current coordinate system.
        ///
        static void strokeLine(Point point1, Point point2);
        
#pragma mark - Path Attributes
        
        ///Returns the line cap style of the path.
        LineCap lineCapStyle() const;
        
        ///Sets the line cap style of the path.
        void setLineCapStyle(LineCap capStyle);
        
        ///Sets the default line cap style for all paths.
        static void setDefaultLineCapStyle(LineCap capStyle);
        
#pragma mark -
        
        ///Returns the line join style of the path.
        LineJoin lineJoinStyle() const;
        
        ///Sets the line join style of the path.
        void setLineJoinStyle(LineJoin joinStyle);
        
        ///Sets the line join style for all paths.
        static void setDefaultLineJoinStyle(LineJoin joinStyle);
        
#pragma mark -
        
        ///Returns the line width of the path.
        Float lineWidth() const;
        
        ///Sets the line width of the path.
        void setLineWidth(Float width);
        
        ///Sets the line width for all paths.
        static void setDefaultLineWidth(Float width);
        
#pragma mark -
        
        ///Sets the 2D affine transform to apply to all
        ///future components added to the receiver.
        void setTransform(const Transform2D &transform);
        
        ///Returns the 2D transform applied to the path's components.
        ///
        ///Default value is `gfx::Transform2D::Identity`.
        Transform2D transform() const;
        
#pragma mark - Functions
        
        ///Adds the path function suite to a given stack frame.
        static void AddTo(StackFrame *frame);
    };
}

#endif /* GFX_Include_GraphicsStack */

#endif /* defined(__gfx__path__) */
