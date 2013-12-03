//
//  osx_wrappers.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__osx_wrappers__
#define __gfx__osx_wrappers__

#if __OBJC__
#   import <Foundation/Foundation.h>
#endif

OBJC_EXTERN_CLASS NSAutoreleasePool;

namespace gfx {
    class Base;
    
    namespace platform {
        NSAutoreleasePool *autorelease_pool_make();
        void autorelease_pool_drain(NSAutoreleasePool **ioPool);
        void autorelease_pool_add(NSAutoreleasePool *pool, const Base *object);
        void autorelease_pool_current_add(const Base *object);
    }
}

#endif /* defined(__gfx__osx_wrappers__) */
