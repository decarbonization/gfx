//
//  types.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "types.h"

namespace gfx {
    
#pragma mark - • Transform2D
    
    Transform2D const Transform2D::Identity = CGAffineTransformIdentity;
    
#pragma mark - Lifecycle
    
    Transform2D::Transform2D(const CGAffineTransform &other) :
        CGAffineTransform(other)
    {
    }
    
#pragma mark - Creating New Transformations
    
    Transform2D Transform2D::rotate(Float angleInRadians) const
    {
        return CGAffineTransformRotate(*this, angleInRadians);
    }
    
    Transform2D Transform2D::scale(Float scaleX, Float scaleY) const
    {
        return CGAffineTransformScale(*this, scaleX, scaleY);
    }
    
    Transform2D Transform2D::translate(Float translateX, Float translateY) const
    {
        return CGAffineTransformTranslate(*this, translateX, translateY);
    }
    
    Transform2D Transform2D::invert() const
    {
        return CGAffineTransformInvert(*this);
    }
    
    Transform2D Transform2D::concat(const Transform2D &other) const
    {
        return CGAffineTransformConcat(*this, other);
    }
}
