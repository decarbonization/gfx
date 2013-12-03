//
//  assertions.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__assertions__
#define __gfx__assertions__

///Whether or not assertions are enabled.
#define gfx_assertions_enabled  1

namespace gfx {
    class String;
    
    ///Logs an error message that includes the name of the function, the name of the file, and the line number.
    ///
    /// \throws Exception
    extern void handleAssertionHandler(const char *function, const char *file, long lineNumber, const String *reason);
}

#if gfx_assertions_enabled

///Generates an assertion failure if a given condition is false.
///
///Do not place side effects in condition, as they will
///not be evaluated if assertions are disabled.
#   define gfx_assert(condition, reason) ({ if(!(condition)) gfx::handleAssertionHandler(__PRETTY_FUNCTION__, __FILE__, __LINE__, (reason)); })

///Generates an assertion failure if a given paramter is null.
///
///Do not place side effects in param, as they will
///not be evaluated if assertions are disabled.
#   define gfx_assert_param(param) gfx_assert(param != nullptr, make<gfx::String>(#param))

#endif /* gfx_assertions_enabled */

#endif /* defined(__gfx__assertions__) */
