//
//  exception.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/16/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__exception__
#define __gfx__exception__

#include <exception>
#include "dictionary.h"
#include "str.h"

namespace gfx {
    class String;
    
    ///The Exception class is a simple extension of std::exception
    ///that uses gfx core types instead of STL types. It and its
    ///derivatives are the only exception types thrown by gfx.
    class Exception : public std::exception
    {
        ///The reason for the exception.
        const String *mReason;
        
        ///The user info associated with the exception.
        Dictionary<const String, Base> *mUserInfo;
        
    public:
        
        ///Constructs an exception with a given `reason` String and `userInfo` Dictionary.
        ///
        /// \param  reason      The reason for the exception.
        /// \param  userInfo    The user info of the exception. Will be copied.
        ///
        Exception(const String *reason, const Dictionary<const String, Base> *userInfo);
        
        ///Constructs an exception by copying the contents of another exception.
        Exception(const Exception &other);
        
        ///Copies the contents of a given exception into itself.
        Exception &operator=(const Exception &other);
        
        ///The destructor.
        virtual ~Exception();
        
#pragma mark - Overrides
        
        ///Returns the `Exception::reason()`.
        virtual const char *what() const noexcept;
        
#pragma mark - Introspection
        
        ///Returns the reason of the exception.
        virtual const String *reason() const;
        
        ///Returns the user info of the exception.
        ///
        ///The user info may be modified by outside parties.
        virtual Dictionary<const String, Base> *userInfo() const;
    };
    
    ///The corresponding value is the offset-line from the code where the exception originated.
    extern const String *const kUserInfoKeyOffsetLine;
    
    ///The corresponding value is the offset-column from the code where the exception originated.
    extern const String *const kUserInfoKeyOffsetColumn;
    
    ///The corresponding value is a backtrace string from the interpreter.
    ///
    ///This value is automatically populated when the Interpreter
    ///observes an exception has been raised.
    extern const String *const kUserInfoKeyBacktraceString;
}

#endif /* defined(__gfx__exception__) */
