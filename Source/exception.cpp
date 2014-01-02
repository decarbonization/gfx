//
//  exception.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/16/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "exception.h"
#include "str.h"
#include "dictionary.h"

namespace gfx {
    Exception::Exception(const String *reason, const Dictionary<const String, Base> *userInfo) :
        mReason(retained(reason)),
        mUserInfo(copy(userInfo) ?: new Dictionary<>())
    {
    }
    
    Exception::Exception(const Exception &other) :
        Exception(other.mReason, copy(other.mUserInfo))
    {
    }
    
    Exception &Exception::operator=(const Exception &other)
    {
        released(mReason);
        released(mUserInfo);
        
        mReason = retained(other.mReason);
        mUserInfo = copy(other.mUserInfo);
        
        return *this;
    }
    
    Exception::~Exception()
    {
        released(mReason);
        mReason = nullptr;
        
        released(mUserInfo);
        mUserInfo = nullptr;
    }
    
#pragma mark - Overrides
    
    const char *Exception::what() const noexcept
    {
        return this->reason()->getCString();
    }
    
#pragma mark - Introspection
    
    const String *Exception::reason() const
    {
        return retained_autoreleased(mReason);
    }
    
    Dictionary<const String, Base> *Exception::userInfo() const
    {
        return retained_autoreleased(mUserInfo);
    }
    
    
    const String *const kUserInfoKeyOffsetLine = new String(CFSTR("gfx::Offset/line"));
    const String *const kUserInfoKeyOffsetColumn = new String(CFSTR("gfx::Offset/column"));
    const String *const kUserInfoKeyBacktraceString = new String(CFSTR("gfx::Interpreter/backtrace"));
}
