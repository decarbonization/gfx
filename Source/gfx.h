//
//  gfx.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/19/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_gfx_h
#define gfx_gfx_h

#if __cplusplus
#   include <gfx/gfx_defines.h>

#   include <gfx/types.h>
#   include <gfx/base.h>
#   include <gfx/str.h>
#   include <gfx/exception.h>
#   include <gfx/array.h>
#   include <gfx/dictionary.h>
#   include <gfx/filepolicy.h>
#   include <gfx/session.h>

#   include <gfx/parser.h>
#   include <gfx/interpreter.h>
#   include <gfx/papertape.h>
#   include <gfx/function.h>
#   include <gfx/offset.h>
#   include <gfx/word.h>
#   include <gfx/stackframe.h>

#   include <gfx/graphics.h>
#   include <gfx/context.h>
#   include <gfx/image.h>
#   include <gfx/layer.h>
#   include <gfx/color.h>

#   include <gfx/gradient.h>
#   include <gfx/shadow.h>
#endif /* __cplusplus */

#if __OBJC__
#   import "GFXDefines.h"
#   import "GFXValue.h"
#   import "GFXInterpreter.h"
#   import "GFXLayer.h"
#   import "GFXView.h"
#endif /* __OBJC__ */

#endif
