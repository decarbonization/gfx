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
        CFMutableDataRef mData;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Constructs a Blob of a given size.
        ///
        /// \param  size    The initial size of the blob's buffer.
        ///
        Blob(Index size = 0);
        
        ///Constructs a Blob using the bytes contained in a CFData object.
        ///
        /// \param  data    The data to copy. Should not be null.
        ///
        Blob(CFDataRef data);
        
        ///Constructs a Blob using a given buffer and size.
        ///
        /// \param  buffer  The buffer to copy into the blob. Ignored if null.
        /// \param  size    The size of the buffer.
        ///
        Blob(const UInt8 *buffer, Index size);
        
        ///Construct a Blob by using the data backing of another blob.
        ///
        /// \param  blob    The other blob to use as a data backing. Should not be null.
        ///
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
        
        ///Returns the underlying mutable storage class used by the blob.
        CFMutableDataRef getStorage();
        
        ///Returns the number of bytes contained by the blob instance.
        Index length() const;
        
        ///Returns the internal bytes contained by the blob instance.
        ///
        ///__Important:__ if all references to the storage of the blob are
        ///destroyed, the value returned by this method becomes invalid.
        const UInt8 *bytes() const;
        
        ///Returns the internal bytes contained by the blob instance as a mutable pointer.
        UInt8 *bytes();
        
        ///Copies a range of bytes from the blob's internal storage
        ///into a given external byte array.
        ///
        /// \param  range       The range of bytes to copy. Throws if not within range.
        /// \param  outBuffer   The buffer to copy the bytes to. Must be large enough to hold `range`.
        ///
        void getBytes(Range range, UInt8 *outBuffer);
        
#pragma mark - Modifying The Blob
        
        ///Appends the bytes from a buffer onto the contents of the blob.
        ///
        /// \param  buffer  The buffer to append.
        /// \param  length  The length of the buffer.
        ///
        void append(const UInt8 *buffer, Index length);
        
        ///Appends the bytes contained in a given blob to the contents of the receiver.
        ///
        /// \param  other   The blob to append.
        ///
        void append(const Blob *other);
        
        ///Deletes a range of bytes from the contents of the blob.
        ///
        /// \param  range   The range of bytes to delete. Must be within {0, length()}.
        ///
        void deleteRange(Range range);
        
        ///Replace the bytes within a given range with a given buffer.
        ///
        /// \param  range   The range of bytes to delete. Must be within {0, length()}.
        /// \param  buffer  The buffer to replace the bytes with.
        /// \param  length  The length of the buffer.
        ///
        void replaceRange(Range range, const UInt8 *buffer, Index length);
        
        ///Replace the bytes within a given range with the data contained in given blob.
        ///
        /// \param  range   The range of bytes to delete. Must be within {0, length()}.
        /// \param  other   The blob to replace the range with.
        ///
        void replaceRange(Range range, const Blob *other);
        
        ///Increase the length of the blob by n bytes, writing zeros into the new portion of the buffer.
        ///
        /// \param  amount  The number of bytes to increase the blob's buffer by.
        ///
        void increaseLength(Index amount);
        
        ///Set the length of the blob.
        ///
        /// \param  length  The new length of the buffer.
        ///
        ///If the length is greater than the current blob size,
        ///zeros will be written in the new portion. If the length
        ///is lesser, the contents of the blob are truncated.
        void setLength(Index length);
    };
}

#endif /* defined(__gfx__data__) */
