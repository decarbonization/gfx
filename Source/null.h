//
//  null.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/17/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__null__
#define __gfx__null__

#include "base.h"

namespace gfx {
    ///The Null class represents the null value in the Gfx language.
    class Null final : public Base
    {
    private:
        
        ///The constructor. Private, there can only be one null.
        Null();
        
        ///The destructor. Unconditionally throws.
        ///The one Null instance cannot be deallocated.
        ~Null();
        
    public:
        
        ///Returns the shared Null instance, creating it if it does not already exist.
        static Null *shared();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
    };
}

#endif /* defined(__gfx__null__) */
