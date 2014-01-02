//
//  GFXDefines.h
//  gfx
//
//  Created by Kevin MacWhinnie on 1/2/14.
//  Copyright (c) 2014 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_GFXDefines_h
#define gfx_GFXDefines_h

#import <Foundation/Foundation.h>

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

#pragma mark -

///The corresponding value is an NSNumber describing the line the error originated from.
///
///The number will be `(-1)` if the origin of the error is unknown.
#define GFXErrorUserInfoOffsetLineKey    @"GFXErrorUserInfoOffsetLineKey"

///The corresponding value is an NSNumber describing the line-column the error originated from.
#define GFXErrorUserInfoOffsetColumnKey  @"GFXErrorUserInfoOffsetColumnKey"

///The corresponding value is an NSString describing the backtrace of the last exception.
#define GFXErrorUserInfoBacktraceKey     @"GFXErrorUserInfoBacktraceKey"

#endif
