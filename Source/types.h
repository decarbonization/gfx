//
//  types.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/29/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_types_h
#define gfx_types_h

#include <CoreGraphics/CoreGraphics.h>

/*!
 Gfx currently only targets Apple platforms that include the CoreGraphics stack,
 however, to aid in the possibility of Gfx becoming cross platform in the future,
 this file contains generic, namespaced types for various basic things.
 */

namespace gfx {
    
    /*! \section    Basic Types */
    
    ///Platform and architecture independent type The basic type
    ///for all floating point values used in the graphics stack.
    typedef CGFloat     Float;
#define GFX_FLOAT_MIN   CGFLOAT_MIN
#define GFX_FLOAT_MAX   CGFLOAT_MAX
    
    ///A type for hashes returned by the `Base::hash() const` method.
    typedef CFHashCode  HashCode;
    
    ///A type used for indexes, counts, etc.
    typedef CFIndex     Index;
    
    ///A type for ranges of bytes, characters, etc.
    struct Range : CFRange
    {
        ///Empty constructor.
        Range() :
            CFRange{}
        {
        }
        
        ///Construct a range from a location and length.
        Range(Index inLocation, Index inLength) :
            CFRange{inLocation, inLength}
        {
        }
        
        ///Construct a range from a CFRange.
        Range(CFRange range) :
            CFRange(range)
        {
        }
        
        ///Returns the maximum index referred to by this range.
        Index max() const
        {
            return this->location + this->length;
        }
        
        ///Returns a bool indicating whether or not an index is contained within the range.
        bool contains(Index index) const
        {
            return (index >= this->location && index <= this->max());
        }
    };
    
#if GFX_Include_GraphicsStack
    
    /*! \section    Graphics Types */
    
    ///A structure that contains a point in a 2 dimensional coordinate system.
    ///
    /// \field  x   The x offset.
    /// \field  y   The y offset.
    ///
    struct Point : CGPoint
    {
        ///Constructs an empty point.
        Point() :
            CGPoint{}
        {
        }
        
        ///Constructs a point with given x and y coordinates.
        ///
        /// \param  x   The x coordinate.
        /// \param  y   The y coordinate.
        ///
        Point(Float x, Float y) :
            CGPoint{x, y}
        {
        }
        
        ///Constructs a point with an instance of its superclass.
        Point(const CGPoint &p) :
            CGPoint(p)
        {
        }
    };
    
    ///A structure that contains width and height values.
    ///
    /// \field  width   The width of the size.
    /// \field  height  The height of the size.
    ///
    struct Size : CGSize
    {
        ///Constructs an empty size.
        Size() :
            CGSize{}
        {
        }
        
        ///Constructs a size with a given width and height.
        ///
        /// \param  width   The width.
        /// \param  height  The height.
        Size(Float width, float height) :
            CGSize{width, height}
        {
        }
        
        ///Constructs a size with an instance of its superclass.
        Size(const CGSize &s) :
            CGSize(s)
        {
        }
    };
    
    ///A structure that contains the location and dimensions of a shape.
    ///
    /// \field  origin  The coordinates of the shape.
    /// \field  size    The size of the shape.
    ///
    struct Rect : CGRect
    {
        ///Constructs an empty rectangle.
        Rect() :
            CGRect{}
        {
        }
        
        ///Constructs a rectangle with a given origin and size.
        ///
        /// \param  origin  The origin.
        /// \param  size    The size.
        ///
        Rect(Point origin, Size size) :
            CGRect{origin, size}
        {
        }
        
        ///Constructs a rectangle with an instance of its superclass.
        Rect(const CGRect &r) :
            CGRect(r)
        {
        }
        
#pragma mark -
        
        ///Returns the width of the rectangle.
        Float getWidth() const { return CGRectGetWidth(*this); }
        
        ///Returns the height of the rectangle.
        Float getHeight() const { return CGRectGetHeight(*this); }
        
#pragma mark -
        
        ///Returns the smallest value for the x coordinate of the rectangle.
        Float getMinX() const { return CGRectGetMinX(*this); }
        
        ///Returns the middle value for the x coordinate of the rectangle.
        Float getMidX() const { return CGRectGetMidX(*this); }
        
        ///Returns the largest value for the x coordinate of the rectangle.
        Float getMaxX() const { return CGRectGetMaxX(*this); }
        
#pragma mark -
        
        ///Returns the smallest value for the y coordinate of the rectangle.
        Float getMinY() const { return CGRectGetMinY(*this); }
        
        ///Returns the middle value for the y coordinate of the rectangle.
        Float getMidY() const { return CGRectGetMidY(*this); }
        
        ///Returns the largest value for the y coordinate of the rectangle.
        Float getMaxY() const { return CGRectGetMaxY(*this); }
    };
    
    ///An opaque structure for holding an affine transformation matrix.
    struct Transform2D : CGAffineTransform
    {
        /*! \section    Lifecycle */
        
        ///Returns a new 2D transform matrix constructed from a provided rotation value.
        static Transform2D makeRotation(Float angleInRadians)
        {
            return Transform2D::Identity.rotate(angleInRadians);
        }
        
        ///Returns a new 2D transform matrix constructed from provided scaling values.
        static Transform2D makeScale(Float scaleX, Float scaleY)
        {
            return Transform2D::Identity.scale(scaleX, scaleY);
        }
        
        ///Returns a new 2D transform matrix constructed from provided translation values.
        static Transform2D makeTranslation(Float translateX, Float translateY)
        {
            return Transform2D::Identity.translate(translateX, translateY);
        }
        
        ///The identity transform.
        static Transform2D const Identity;
        
        ///Constructs a Transform2D from a native affine transform.
        Transform2D(const CGAffineTransform &other);
        
        
        /*! \section    Creating New Transformation */
        
        ///Returns a new 2D transform constructed by rotating the receiver.
        Transform2D rotate(Float angleInRadians) const;
        
        ///Returns a new 2D transform constructed by scaling the receiver.
        Transform2D scale(Float scaleX, Float scaleY) const;
        
        ///Returns a new 2D transform constructed by translating the receiver.
        Transform2D translate(Float translateX, Float translateY) const;
        
        ///Returns a new 2D transform constructed by inverting the receiver.
        Transform2D invert() const;
        
        ///Returns a new 2D transform constructed by concating the receiver with another given 2D transform.
        Transform2D concat(const Transform2D &other) const;
    };
    
#endif /* GFX_Include_GraphicsStack */
}

#endif
