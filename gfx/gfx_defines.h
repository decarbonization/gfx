//
//  gfx_defines.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/19/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_gfx_defines_h
#define gfx_gfx_defines_h

///Whether or not support for basic file operations
///should be exposed to the Gfx language.
#define GFX_Language_SupportsFiles  1

///Whether or not the import directive should
///be exposed to the Gfx language.
#define GFX_Language_SupportsImport 1

///Whether or not the graphics stack should be
///compiled and included with the language.
#define GFX_Include_GraphicsStack   1

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
