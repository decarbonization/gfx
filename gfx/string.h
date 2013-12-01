//
//  string.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__string__
#define __gfx__string__

#include "base.h"
#include <stdarg.h>

namespace gfx {
    ///The String class is a simple wrapper around the CFString type.
    ///
    ///All instances of `String` have mutable storage, to represent an
    ///immutable string, qualify the instance with the const modifier.
    class String : public Base
    {
        ///The storage of the string.
        CFMutableStringRef mStorage;
        
    public:
        
        ///Constructs an empty string.
        String();
        
        ///Constructs a string from a given c string with a given encoding.
        String(const char *string, CFStringEncoding encoding = kCFStringEncodingUTF8);
        
        ///Constructs a string from a given buffer of utf16 data, length, and encoding.
        String(const UniChar *buffer, Index length, CFStringEncoding encoding);
        
        ///Constructs a string by copying another string.
        String(const String *string);
        
        ///Constructs a string by copying a CFString's contents.
        String(CFStringRef string);
        
        ///The destructor.
        ~String();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        
        ///Returns a bool indicating whether or not the string matches a given string.
        bool isEqual(const String *other) const;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
        ///Compares the string to another string, returning its sorting order.
        ///
        /// \param  other   The string to compare the receiver against. May not be null.
        /// \param  range   The range of the receiver to compare.
        /// \param  options Comparison options.
        ///
        /// \result The result of the comparison.
        ///
        CFComparisonResult compare(const String *other, Range range, CFStringCompareFlags options = 0) const;
        
        ///Returns a bool indicating whether or not the receiver starts with a given string.
        ///
        /// \param  other   May not be null.
        ///
        bool hasPrefix(const String *other) const;
        
        ///Returns a bool indicating whether or not the receiver ends with a given string.
        ///
        /// \param  other   May not be null.
        ///
        bool hasSuffix(const String *other) const;
        
#pragma mark - Storage
        
        ///Returns the underlying storage of the string.
        ///
        ///This should be considered an implementation detail,
        ///but is exposed for platform compatibility.
        CFMutableStringRef getStorage();
        CFStringRef getStorage() const;
        
#pragma mark -
        
        ///Returns the length of the string.
        Index length() const;
        
        ///Returns the encoding of the string.
        CFStringEncoding encoding() const;
        
        ///Returns the character located at a given offset.
        ///
        /// \throws Exception when offset is out of bounds.
        ///
        UniChar at(Index offset) const;
        
        ///Returns a new String created from a given range of the receiver's data.
        String *substring(Range range) const;
        
        ///Returns a C string representation of the receiver with
        ///a given encoding, defaulting to the kCFStringEncodingUTF8.
        const char *getCString(CFStringEncoding encoding = kCFStringEncodingUTF8) const;
        
        ///Places the characters in a given range into a given utf16 data buffer.
        void getCharacters(Range range, UniChar *ioBuffer) const;
        
#pragma mark - Conversions
        
        ///Converts the strings contents to a double value.
        double doubleValue() const;
        
        ///Converts the strings contents to an int value.
        int intValue() const;
        
#pragma mark -
        
        ///Returns an upper-case version of the string.
        String *uppercasedString() const;
        
        ///Returns a lower-case version of the string.
        String *lowercasedString() const;
        
        ///Returns a capitalized version of the string.
        String *capitalizedString() const;
        
#pragma mark - Searching
        
        ///Searches for a given string within the receiver, returning the range if found.
        ///
        /// \param  stringToFind    The string to search for. Required.
        /// \param  range           The range in the receiver to search within.
        /// \param  options         The search options.
        ///
        /// \result A range consisting of {kCFNotFound, 0} if nothing could be found; a valid range otherwise.
        ///
        Range find(const String *stringToFind, Range range, CFStringCompareFlags options = 0) const;
        
        ///Finds all instances of a given string, and replaces them with a given substitution.
        ///
        /// \param  stringToFind    The string to search for. Required.
        /// \param  stringToReplace The string to replace the search string with. Required.
        /// \param  searchRange     The range to search within in the receiver.
        /// \param  options         The search options.
        ///
        /// \result The number of occurances replaced.
        ///
        Index findAndReplace(const String *stringToFind, const String *stringToReplace, Range searchRange, CFStringCompareFlags options = 0);
        
#pragma mark - Mutation
        
        ///Replaces a range of data within the receiver with a given string.
        ///
        /// \param  rangeToReplace      The range to replace.
        /// \param  replacementString   The string to replace the range's data with. Required.
        ///
        void replace(Range rangeToReplace, const String *replacementString);
        
        ///Replaces the contents of the receiver with a given string.
        ///
        /// \param  replacementString   May not be null.
        ///
        void replaceAll(const String *replacementString);
        
#pragma mark -
        
        ///Inserts a given string at a given index.
        ///
        /// \param  stringToInsert  The string to insert. Required.
        /// \param  insertionPoint  Where to insert the string. Must be within {0, String::length()}
        ///
        /// \throws Exception on out of bounds errors.
        void insert(const String *stringToInsert, Index insertionPoint);
        
        ///Appends a given string to the receiver.
        ///
        /// \param  stringToAppend  May not be null.
        void append(const String *stringToAppend);
        
        ///Deletes a range of data from the receiver.
        void deleteRange(Range rangeToDelete);
        
#pragma mark - Builder
        
        ///The Builder class encapsulates the construction of complex strings,
        ///serving the basic role of format strings. A Builder is intended to
        ///be allocated on the stack, and implicitly converted to a String.
        class Builder
        {
            String *mString;
            
        public:
            
            ///The constructor.
            Builder() :
                mString(new String())
            {
            }
            
            ///The destructor.
            ~Builder()
            {
                mString->release();
            }
            
            ///Returns the length of the builder's string.
            Index length() const
            {
                return mString->length();
            }
            
            ///Returns the builder's string.
            String *string() const
            {
                return retained_autoreleased(mString);
            }
            operator String *() const
            {
                return this->string();
            }
            
#pragma mark - Appenders
            
            ///Appends a given format string and its parameters.
            ///
            /// \param  format  The format string. Required.
            /// \param  ...     The parameters for the format string.
            ///
            /// \result The receiver.
            ///
            ///This method is an implementation detail.
            Builder &append(CFStringRef format, ...)
            {
                va_list args;
                va_start(args, format);
                CFStringAppendFormatAndArguments(mString->getStorage(), NULL, format, args);
                va_end(args);
                
                return *this;
            }
            
            ///Append a `long` value.
            Builder &operator<<(long value)
            {
                this->append(CFSTR("%d"), value);
                
                return *this;
            }
            
            ///Append an `unsigned long` value.
            Builder &operator<<(unsigned long value)
            {
                this->append(CFSTR("%d"), value);
                
                return *this;
            }
            
            ///Append a `double` value.
            Builder &operator<<(double value)
            {
                this->append(CFSTR("%f"), value);
                
                return *this;
            }
            
            ///Append a single `UniChar` value.
            Builder &operator<<(UniChar value)
            {
                this->append(CFSTR("%C"), value);
                
                return *this;
            }
            
            ///Append a `void *` pointer address value.
            Builder &operator<<(void *value)
            {
                this->append(CFSTR("%p"), value);
                
                return *this;
            }
            
            ///Append a `const char *` c string value.
            Builder &operator<<(const char *string)
            {
                this->append(CFSTR("%s"), string);
                
                return *this;
            }
            
            ///Append a `const Base *` instance's description.
            Builder &operator<<(const Base *object)
            {
                mString->append(object->description());
                
                return *this;
            }
            
        private:
            Builder(const Builder &);
            Builder &operator=(const Builder &);
        };
    };
    
#pragma mark - Overloads
    
    static inline const String *operator"" _gfx(const char *rawString, size_t size)
    {
        return autoreleased(new String(rawString));
    }
}

#endif /* defined(__gfx__string__) */
