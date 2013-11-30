//
//  blob.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "blob.h"
#include "string.h"

namespace gfx {
    
#pragma mark - Lifecycle
    
    Blob::Blob() :
        Blob(nullptr, 0)
    {
    }
    
    Blob::Blob(const UInt8 *buffer, Index size) :
        Base(),
        mData(CFDataCreate(kCFAllocatorDefault, buffer, size))
    {
        
    }
    
    Blob::Blob(CFDataRef data) :
        Base(),
        mData((CFDataRef)CFRetain(data))
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
            inspectString.string()->deleteRange(CFRangeMake(inspectString.length() - 1, 1));
        
        return inspectString;
    }
    
#pragma mark - Introspection
    
    CFDataRef Blob::getStorage() const
    {
        return mData;
    }
    
    Index Blob::length() const
    {
        return CFDataGetLength(mData);
    }
    
    const UInt8 *Blob::bytes() const
    {
        return CFDataGetBytePtr(mData);
    }
    
    void Blob::getBytes(CFRange range, UInt8 *outBuffer)
    {
        gfx_assert_param(outBuffer);
        gfx_assert(range.location < length() && range.location + range.length < length(), "out of bounds range"_gfx);
        
        CFDataGetBytes(mData, range, outBuffer);
    }
}
