//
//  blob.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "blob.h"
#include "str.h"

namespace gfx {
    
#pragma mark - Lifecycle
    
    Blob::Blob(Index size) :
        Blob(nullptr, size)
    {
    }
    
    Blob::Blob(const UInt8 *buffer, Index size) :
        Base(),
        mData(CFDataCreateMutable(kCFAllocatorDefault, size))
    {
        if(buffer)
            append(buffer, size);
    }
    
    Blob::Blob(CFDataRef data) :
        Base(),
        mData(CFDataCreateMutableCopy(kCFAllocatorDefault, CFDataGetLength(data), data))
    {
    }
    
    Blob::Blob(const Blob *blob) :
        Blob(blob->getStorage())
    {
    }
    
    Blob::~Blob()
    {
        if(mData) {
            CFRelease(mData);
            mData = NULL;
        }
    }
    
#pragma mark - Identity
    
    HashCode Blob::hash() const
    {
        return CFHash(mData);
    }
    
    bool Blob::isEqual(const Blob *other) const
    {
        if(!other)
            return false;
        
        return CFEqual(this->getStorage(), other->getStorage());
    }
    
    bool Blob::isEqual(const Base *other) const
    {
        if(!other)
            return false;
        
        if(this->isKindOfClass<Blob>()) {
            return this->isEqual((const Blob *)other);
        }
        
        return false;
    }
    
    const String *Blob::description() const
    {
        String::Builder description;
        
        description << "<" << this->className() << ":" << (void *)this << " ";
        description << length() << " bytes";
        description << ">";
        
        return description;
    }
    
    const String *Blob::inspect() const
    {
        String::Builder inspectString;
        
        const UInt8 *bytes = this->bytes();
        for (Index i = 0, l = length(); i < l; i++) {
            inspectString.append(CFSTR("%x "), bytes[i]);
        }
        
        if(inspectString.length() > 0)
            inspectString.string()->deleteRange(Range(inspectString.length() - 1, 1));
        
        return inspectString;
    }
    
#pragma mark - Introspection
    
    CFDataRef Blob::getStorage() const
    {
        return mData;
    }
    
    CFMutableDataRef Blob::getStorage()
    {
        return mData;
    }
    
    Index Blob::length() const
    {
        return CFDataGetLength(getStorage());
    }
    
    const UInt8 *Blob::bytes() const
    {
        return CFDataGetBytePtr(getStorage());
    }
    
    UInt8 *Blob::bytes()
    {
        return CFDataGetMutableBytePtr(getStorage());
    }
    
    void Blob::getBytes(Range range, UInt8 *outBuffer)
    {
        gfx_assert_param(outBuffer);
        gfx_assert(range.location < length() && range.max() < length(), "out of bounds range"_gfx);
        
        CFDataGetBytes(getStorage(), range, outBuffer);
    }
    
#pragma mark - Modifying The Blob
    
    void Blob::append(const UInt8 *buffer, Index length)
    {
        CFDataAppendBytes(getStorage(), buffer, length);
    }
    
    void Blob::append(const Blob *other)
    {
        append(other->bytes(), other->length());
    }
    
    void Blob::deleteRange(Range range)
    {
        gfx_assert(range.location < length() && range.max() < length(), "out of bounds range"_gfx);
        
        CFDataDeleteBytes(getStorage(), range);
    }
    
    void Blob::replaceRange(Range range, const UInt8 *buffer, Index length)
    {
        gfx_assert(range.location < this->length() && range.max() < this->length(), "out of bounds range"_gfx);
        
        CFDataReplaceBytes(getStorage(), range, buffer, length);
    }
    
    void Blob::replaceRange(Range range, const Blob *other)
    {
        replaceRange(range, other->bytes(), other->length());
    }
    
    void Blob::increaseLength(Index amount)
    {
        CFDataIncreaseLength(getStorage(), amount);
    }
    
    void Blob::setLength(Index length)
    {
        CFDataSetLength(getStorage(), length);
    }
}
