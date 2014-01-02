//
//  GFXHelpers.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/8/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_GFXHelpers_h
#define gfx_GFXHelpers_h

#import <Foundation/Foundation.h>
#import "GFXDefines.h"
#if __cplusplus
#   include "str.h"
#   include "number.h"
#   include "exception.h"
#endif /* __cplusplus */

/*!
 The GFXHelpers header contains constants and functions
 that aid in the marshalling of basic types and errors.
 
 \seealso(GFXValue)
 */

#pragma mark - String Conversions

#if __cplusplus && __OBJC__

///Returns an NSString with a copy of the contents contained in a given `gfx::String`.
///
/// \param  string  The string to convert.
///
/// \result An NSString representation of `string`.
///
///Passing a null parameter will result in nil being returned.
static inline NSString *NSStringFromGFXString(const gfx::String *string)
{
    if(!string)
        return nil;
    
    return [[(NSString *)string->getStorage() copy] autorelease];
}

///Returns a `gfx::String` with a copy of the contents contained in a given NSString.
///
/// \param  string  The string to convert.
///
/// \result A `gfx::String` representation of `string`.
///
///Pasing a nil parameter will result in null being returned.
static inline gfx::String *NSStringToGFXString(NSString *string)
{
    if(!string)
        return nullptr;
    
    return gfx::make<gfx::String>((CFStringRef)string);
}

#pragma mark -

///Converts a given `gfx::Exception` object into an NSError.
///
/// \param  errorCode   The code to give the resultant NSError.
/// \param  e           The exception to convert.
///
/// \result An NSError mirroring the contents of the given exception.
///
///__Important:__ The full user info of the exception is not copied, only error offset information.
static inline NSError *NSErrorFromGFXException(GFXErrorCode errorCode, const gfx::Exception &e)
{
    auto userInfo = e.userInfo();
    
    NSInteger line = -1;
    NSInteger column = 0;
    NSString *backtrace = @"";
    if(userInfo->get(gfx::kUserInfoKeyOffsetLine) && userInfo->get(gfx::kUserInfoKeyOffsetColumn)) {
        line = static_cast<gfx::Number *>(e.userInfo()->get(gfx::kUserInfoKeyOffsetLine))->value();
        column = static_cast<gfx::Number *>(e.userInfo()->get(gfx::kUserInfoKeyOffsetColumn))->value();
    }
    if(auto underlyingBacktrace = static_cast<gfx::String *>(userInfo->get(gfx::kUserInfoKeyBacktraceString))) {
        backtrace = NSStringFromGFXString(underlyingBacktrace);
    }
    
    return [NSError errorWithDomain:GFXErrorDomain
                               code:errorCode
                           userInfo:@{NSLocalizedDescriptionKey: NSStringFromGFXString(e.reason()),
                                      GFXErrorUserInfoOffsetLineKey: @(line),
                                      GFXErrorUserInfoOffsetColumnKey: @(column),
                                      GFXErrorUserInfoBacktraceKey: backtrace}];
}

#endif /* __cplusplus && __OBJC__ */
#endif /* gfx_GFXHelpers_h */
