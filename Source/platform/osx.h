//
//  platform/osx.h
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

namespace gfx {
    class Base;
    
    namespace platform {
        typedef void AutoreleasePool;
        
        AutoreleasePool *autorelease_pool_make();
        void autorelease_pool_drain(AutoreleasePool **ioPool);
        void autorelease_pool_add(AutoreleasePool *pool, const Base *object);
        void autorelease_pool_current_add(const Base *object);
        
#pragma mark -
        
        float display_default_scale_get();
    }
}

#endif /* defined(__gfx__osx_wrappers__) */
