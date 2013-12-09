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

#pragma mark - Errors

///The error domain used by errors returned from the GFX ObjC API.
///
/// \seealso(GFXErrorCode)
#define GFXErrorDomain  @"GFXErrorDomain"

///The different types of errors in the `GFXErrorDomain`.
typedef NS_ENUM(NSInteger, GFXErrorCode) {
    ///A generic error.
    GFXErrorGeneric,
    
    ///A parsing error.
    GFXErrorParsingDidFail,
    
    ///An interpretation error.
    GFXErrorInterpretationDidFail,
    
    ///A render failure error.
    GFXErrorRenderingDidFail,
};

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

///The corresponding value is an NSNumber describing the line the error originated from.
///
///The number will be `(-1)` if the origin of the error is unknown.
#define GFXOffsetLineKey    @"GFXOffsetLineKey"

///The corresponding value is an NSNumber describing the line-column the error originated from.
#define GFXOffsetColumnKey  @"GFXOffsetColumnKey"

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
    NSInteger line = -1;
    NSInteger column = 0;
    if(e.userInfo()->get(gfx::kUserInfoKeyOffsetLine) && e.userInfo()->get(gfx::kUserInfoKeyOffsetColumn)) {
        line = static_cast<gfx::Number *>(e.userInfo()->get(gfx::kUserInfoKeyOffsetLine))->value();
        column = static_cast<gfx::Number *>(e.userInfo()->get(gfx::kUserInfoKeyOffsetColumn))->value();
    }
    
    return [NSError errorWithDomain:GFXErrorDomain
                               code:errorCode
                           userInfo:@{NSLocalizedDescriptionKey: NSStringFromGFXString(e.reason()),
                                      GFXOffsetLineKey: @(line),
                                      GFXOffsetColumnKey: @(column)}];
}

#endif /* __cplusplus && __OBJC__ */
#endif /* gfx_GFXHelpers_h */
