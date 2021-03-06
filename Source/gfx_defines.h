//
//  gfx_defines.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/19/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_gfx_defines_h
#define gfx_gfx_defines_h

/*
 The following defines should be conditionally introduced by
 the environment compiling the Gfx langauge and graphics stack:
 
 GFX_TARGET_OSX_FRAMEWORK: Indicates that GFX is being compiled into a framework targeting OS X.
 - Gfx will look for its runtime resources within the bundle of the framework.
 
 GFX_TARGET_IOS_LIBRARY: Indicates that GFX is being compiled into an iOS static library.
 - Gfx will look for its runtime resources within the main bundle of the host.
 
 GFX_TARGET_SYS_LIBRARY: Indicates that GFX is being compiled into a system library.
 - GFX_TARGET_INSTALLATION_DIR should also be defined to indicate where the library will be installed.
 - Gfx will look for its runtime resources in GFX_TARGET_INSTALLATION_DIR/share/gfx
 */

#pragma mark - Glue

///Language safe glue that allows the same extern statement
///to be used without preprocessor ugliness.
///
///__Important:__ OBJC_EXTERN_CLASS must always be used outside of namespaces.
#if __OBJC__
#   define OBJC_EXTERN_CLASS    @class
#else
#   define OBJC_EXTERN_CLASS    class
#endif /* __OBJC__ */

#pragma mark - Information

///A C-string literal containing the current version of Gfx.
#define GFX_Version                     "0.0.1"

///The major component of the current version of Gfx.
#define GFX_Version_Major               0

///The minor component of the current version of Gfx.
#define GFX_Version_Minor               0

///The bug fix component of the current version of Gfx.
#define GFX_Version_BugFix              1

#pragma mark - Feature Toggles

///Whether or not the import directive should
///be exposed to the Gfx language.
#define GFX_Language_SupportsImport     1

///Whether or not the __recurse function
///should be exposed to allow for efficient
///tail recursion. Has a performance penalty
///for platforms where exceptions are not free.
#define GFX_Language_SupportsRecursion  1

///Whether or not the graphics stack should be
///compiled and included with the language.
#define GFX_Include_GraphicsStack       1

#pragma mark - Mac Config Options

#if TARGET_OS_MAC
///Whether or not to use the implementation of `gfx::Layer`
///that is backed by an instance of `CGLayer`. Mutually-
///exclusive with `GFX_Layer_Use_CA`.
#   define GFX_Layer_Use_CG             0

///Whether or not to use the implementation of `gfx::Layer`
///that is backed by an instance of `CALayer`. Mutually-
///exclusive with `GFX_Layer_Use_CG`.
#   define GFX_Layer_Use_CA             1
#endif /* TARGET_OS_MAC && !TARGET_OS_IPHONE */

#endif
