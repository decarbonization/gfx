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
    ///The Blob class encapsulates an immutable sequence of bytes.
    class Blob : public Base
    {
        ///The underlying storage of the Blob class.
        CFDataRef mData;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Constructs an empty Blob.
        Blob();
        
        ///Constructs a Blob using the bytes contained in a CFData object.
        ///
        /// \param  data    The data to use as backing. Should not be null.
        ///
        ///__Important:__ the data object passed in is retained and not copied.
        Blob(CFDataRef data);
        
        ///Constructs a Blob using a given buffer and size.
        ///
        /// \param  buffer  The buffer to copy into the blob. Should not be null.
        /// \param  size    The size of the buffer.
        ///
        Blob(const UInt8 *buffer, Index size);
        
        ///Construct a Blob by using the data backing of another blob.
        ///
        /// \param  blob    The other blob to use as a data backing. Should not be null.
        ///
        ///__Important:___ The data contained in the other blob is not copied. The lifecycle
        ///of the data contained in blobs is indepednent from the blobs enclosing them. As such,
        ///it is safe to release the old blob after a copy of it has been made.
        Blob(const Blob *blob);
        
        ///The destructor.
        ~Blob();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        
        ///Returns a bool indicating whether or not the blob matches a given blob.
        bool isEqual(const Blob *other) const;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
        ///Returns a "human readable" string representation of the bytes contained in the blob.
        ///
        ///The description method of Blob does not return a representation of the bytes.
        const String *inspect() const;
        
#pragma mark - Introspection
        
        ///Returns the underlying storage class used by the blob.
        CFDataRef getStorage() const;
        
        ///Returns the number of bytes contained by the blob instance.
        Index length() const;
        
        ///Returns the internal bytes contained by the blob instance.
        ///
        ///__Important:__ if all references to the storage of the blob are
        ///destroyed, the value returned by this method becomes invalid.
        const UInt8 *bytes() const;
        
        ///Copies a range of bytes from the blob's internal storage
        ///into a given external byte array.
        ///
        /// \param  range       The range of bytes to copy. Throws if not within range.
        /// \param  outBuffer   The buffer to copy the bytes to. Must be large enough to hold `range`.
        ///
        void getBytes(CFRange range, UInt8 *outBuffer);
    };
}

#endif /* defined(__gfx__data__) */
