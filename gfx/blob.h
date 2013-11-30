//
//  blob.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__data__
#define __gfx__data__

#include "base.h"

namespace gfx {
    class Blob : public Base
    {
        CFDataRef mData;
        
    public:
        
#pragma mark - Lifecycle
        
        Blob();
        Blob(CFDataRef data);
        Blob(const Blob *blob);
        ~Blob();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        
        ///Returns a bool indicating whether or not the blob matches a given blob.
        bool isEqual(const Blob *other) const;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        const String *inspect() const;
        
#pragma mark - Introspection
        
        CFDataRef getStorage() const;
        Index length() const;
        const UInt8 *bytes() const;
        void getBytes(CFRange range, UInt8 *outBuffer);
    };
}

#endif /* defined(__gfx__data__) */
