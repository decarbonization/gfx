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
    typedef CGPoint     Point;
    
    ///A structure that contains width and height values.
    ///
    /// \field  width   The width of the size.
    /// \field  height  The height of the size.
    ///
    typedef CGSize      Size;
    
    ///A structure that contains the location and dimensions of a shape.
    ///
    /// \field  origin  The coordinates of the shape.
    /// \field  size    The size of the shape.
    ///
    typedef CGRect      Rect;
    
#endif /* GFX_Include_GraphicsStack */
}

#endif
