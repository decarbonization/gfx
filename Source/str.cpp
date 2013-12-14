//
//  string.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "str.h"
#include "exception.h"

namespace gfx {
    const String *const String::Empty = new String();
    
    String::String() :
        mStorage(CFStringCreateMutable(kCFAllocatorDefault, 0))
    {
    }
    
    String::String(const char *string, CFStringEncoding encoding) :
        mStorage(CFStringCreateMutable(kCFAllocatorDefault, strlen(string)))
    {
        CFStringAppendCString(mStorage, string, encoding);
    }
    
    String::String(const UniChar *buffer, Index length, CFStringEncoding encoding) :
        mStorage(CFStringCreateMutable(kCFAllocatorDefault, length))
    {
        CFStringAppendCharacters(mStorage, buffer, length);
    }
    
    String::String(CFStringRef string) :
        mStorage(CFStringCreateMutableCopy(kCFAllocatorDefault, 0, string))
    {
    }
    
    String::String(const String *string) :
        mStorage(CFStringCreateMutableCopy(kCFAllocatorDefault, 0, string->mStorage))
    {
    }
    
    String::~String()
    {
        CFRelease(mStorage);
    }
    
#pragma mark - Identity
    
    HashCode String::hash() const
    {
        return CFHash(mStorage);
    }
    
    bool String::isEqual(const String *other) const
    {
        if(!other)
            return false;
        
        return CFEqual(this->getStorage(), other->getStorage());
    }
    
    bool String::isEqual(const Base *other) const
    {
        if(!other)
            return false;
        
        if(this->isKindOfClass<String>()) {
            return this->isEqual((const String *)other);
        }
        
        return false;
    }
    
    const String *String::description() const
    {
        return retained_autoreleased(this);
    }
    
    CFComparisonResult String::compare(const String *other, Range range, CFStringCompareFlags options) const
    {
        gfx_assert_param(other);
        
        return CFStringCompareWithOptions(this->getStorage(), other->getStorage(), range, options);
    }
    
    bool String::hasPrefix(const String *other) const
    {
        gfx_assert_param(other);
        
        return CFStringHasPrefix(this->getStorage(), other->getStorage());
    }
    
    bool String::hasSuffix(const String *other) const
    {
        gfx_assert_param(other);
        
        return CFStringHasSuffix(this->getStorage(), other->getStorage());
    }
    
#pragma mark - Storage
    
    CFMutableStringRef String::getStorage()
    {
        return mStorage;
    }
    
    CFStringRef String::getStorage() const
    {
        return (CFStringRef)mStorage;
    }
    
#pragma mark -
    
    Index String::length() const
    {
        return CFStringGetLength(mStorage);
    }
    
    CFStringEncoding String::encoding() const
    {
        return CFStringGetFastestEncoding(mStorage);
    }
    
    UniChar String::at(Index offset) const
    {
        gfx_assert((offset < this->length()), str("out of bounds"));
        
        return CFStringGetCharacterAtIndex(mStorage, offset);
    }
    
    String *String::substring(Range range) const
    {
        return make<String>(CFStringCreateWithSubstring(kCFAllocatorDefault, mStorage, range));
    }
    
    const char *String::getCString(CFStringEncoding encoding) const
    {
        return CFStringGetCStringPtr(mStorage, encoding);
    }
    
    void String::getCharacters(Range range, UniChar *ioBuffer) const
    {
        gfx_assert_param(ioBuffer);
        
        CFStringGetCharacters(mStorage, range, ioBuffer);
    }
    
#pragma mark - Conversions
    
    double String::doubleValue() const
    {
        return CFStringGetDoubleValue(mStorage);
    }
    
    int String::intValue() const
    {
        return CFStringGetIntValue(mStorage);
    }
    
#pragma mark -
    
    String *String::uppercasedString() const
    {
        String *copy = make<String>(this);
        CFStringUppercase(copy->getStorage(), NULL);
        return copy;
    }
    
    String *String::lowercasedString() const
    {
        String *copy = make<String>(this);
        CFStringLowercase(copy->getStorage(), NULL);
        return copy;
    }
    
    String *String::capitalizedString() const
    {
        String *copy = make<String>(this);
        CFStringCapitalize(copy->getStorage(), NULL);
        return copy;
    }
    
#pragma mark - Searching
    
    Range String::find(const String *stringToFind, Range range, CFStringCompareFlags options) const
    {
        gfx_assert_param(stringToFind);
        
        Range result;
        if(CFStringFindWithOptions(mStorage, stringToFind->getStorage(), range, options, &result)) {
            return result;
        } else {
            return Range(kCFNotFound, 0);
        }
    }
    
    Index String::findAndReplace(const String *stringToFind, const String *stringToReplace, Range searchRange, CFStringCompareFlags options)
    {
        gfx_assert_param(stringToFind);
        gfx_assert_param(stringToReplace);
        
        return CFStringFindAndReplace(mStorage, stringToFind->getStorage(), stringToReplace->getStorage(), searchRange, options);
    }
    
#pragma mark - Mutation
    
    void String::replace(Range rangeToReplace, const String *replacementString)
    {
        gfx_assert_param(replacementString);
        
        CFStringReplace(mStorage, rangeToReplace, replacementString->getStorage());
    }
    
    void String::replaceAll(const String *replacementString)
    {
        gfx_assert_param(replacementString);
        
        CFStringReplaceAll(mStorage, replacementString->getStorage());
    }
    
#pragma mark -
    
    void String::insert(const String *stringToInsert, Index insertionPoint)
    {
        gfx_assert_param(stringToInsert);
        
        CFStringInsert(mStorage, insertionPoint, stringToInsert->getStorage());
    }
    
    void String::append(const String *stringToAppend)
    {
        gfx_assert_param(stringToAppend);
        
        CFStringAppend(mStorage, stringToAppend->getStorage());
    }
    
    void String::deleteRange(Range rangeToDelete)
    {
        CFStringDelete(mStorage, rangeToDelete);
    }
}