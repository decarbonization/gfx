//
//  gfx_defines.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/19/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_gfx_defines_h
#define gfx_gfx_defines_h

#pragma mark - Information

///A C-string literal containing the current version of Gfx.
#define GFX_Version                 "0.0.1"

///The major component of the current version of Gfx.
#define GFX_Version_Major           0

///The minor component of the current version of Gfx.
#define GFX_Version_Minor           0

///The bug fix component of the current version of Gfx.
#define GFX_Version_BugFix          1

#pragma mark - Feature Toggles

///Whether or not support for basic file operations
///should be exposed to the Gfx language.
#define GFX_Language_SupportsFiles  1

///Whether or not the import directive should
///be exposed to the Gfx language.
#define GFX_Language_SupportsImport 1

///Whether or not the graphics stack should be
///compiled and included with the language.
#define GFX_Include_GraphicsStack   1

#pragma mark - Mac Config Options

#if TARGET_OS_MAC
///Whether or not to use the implementation of `gfx::Layer`
///that is backed by an instance of `CGLayer`. Mutually-
///exclusive with `GFX_Layer_Use_CA`.
#   define GFX_Layer_Use_CG         1

///Whether or not to use the implementation of `gfx::Layer`
///that is backed by an instance of `CALayer`. Mutually-
///exclusive with `GFX_Layer_Use_CG`.
#   define GFX_Layer_Use_CA         0
#endif /* TARGET_OS_MAC && !TARGET_OS_IPHONE */

#endif
