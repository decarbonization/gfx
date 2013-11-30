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
    
    ///A type for hashes returned by the `Base::hash() const` method.
    typedef CFHashCode  HashCode;
    
    ///A type used for indexes, counts, etc.
    typedef CFIndex     Index;
    
#if GFX_Include_GraphicsStack
    
    /*! \section    Graphics Types */
    
    ///A structure that contains a point in a 2 dimensional coordinate system.
    ///
    /// \field  x   The x offset.
    /// \field  y   The y offset.
    ///
    typedef CGPoint             Point;
    
    ///A structure that contains width and height values.
    ///
    /// \field  width   The width of the size.
    /// \field  height  The height of the size.
    ///
    typedef CGSize              Size;
    
    ///A structure that contains the location and dimensions of a shape.
    ///
    /// \field  origin  The coordinates of the shape.
    /// \field  size    The size of the shape.
    ///
    typedef CGRect              Rect;
    
    ///An opaque structure for holding an affine transformation matrix.
    class Transform2D : protected CGAffineTransform
    {
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
        static Transform2D const Identity = CGAffineTransformIdentity;
        
#pragma mark -
        
        ///Returns a new 2D transform constructed by rotating the receiver.
        Transform2D rotate(Float angleInRadians) const
        {
            return CGAffineTransformRotate(*this, angleInRadians);
        }
        
        ///Returns a new 2D transform constructed by scaling the receiver.
        Transform2D scale(Float scaleX, Float scaleY) const
        {
            return CGAffineTransformScale(*this, scaleX, scaleY);
        }
        
        ///Returns a new 2D transform constructed by translating the receiver.
        Transform2D translate(Float translateX, Float translateY) const
        {
            return CGAffineTransformTranslate(*this, translateX, translateY);
        }
        
        ///Returns a new 2D transform constructed by inverting the receiver.
        Transform2D invert() const
        {
            return CGAffineTransformInvert(*this);
        }
        
        ///Returns a new 2D transform constructed by concating the receiver with another given 2D transform.
        Transform2D concat(const Transform2D &other) const
        {
            return CGAffineTransformConcat(*this, other);
        }
    };
    
#endif /* GFX_Include_GraphicsStack */
}

#endif
