//
//  osx_wrappers.mm
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "osx.h"
#include "str.h"
#include "assertions.h"

#if TARGET_OS_IPHONE
#   import <UIKit/UIKit.h>
#endif /* TARGET_OS_IPHONE */

///Set to 1 to include the class name in the `GFXBaseWrapper`. Useful for debugging crashes.
#define Debug_IncludeClassName  1

@interface GFXBaseWrapper : NSObject {
    const gfx::Base *_object;
#if Debug_IncludeClassName
    NSString *_className;
#endif /* Debug_IncludeClassName */
}

- (instancetype)initWithBaseObject:(const gfx::Base *)object;

@end

@implementation GFXBaseWrapper

- (void)dealloc
{
    if(_object) {
        _object->release();
        _object = nil;
    }
    
#if Debug_IncludeClassName
    [_className release];
    _className = nil;
#endif /* Debug_IncludeClassName */
    
    [super dealloc];
}

- (instancetype)initWithBaseObject:(const gfx::Base *)object
{
    if((self = [super init])) {
        _object = object;
        
#if Debug_IncludeClassName
        if(object->isKindOfClass<gfx::String>())
            _className = @"gfx::String";
        else
            _className = [(NSString *)_object->className()->getStorage() copy];
#endif /* Debug_IncludeClassName */
    }
    
    return self;
}

@end

#pragma mark -

namespace gfx {
    namespace platform {
        AutoreleasePool *autorelease_pool_make()
        {
            return (AutoreleasePool *)[NSAutoreleasePool new];
        }
        
        void autorelease_pool_drain(AutoreleasePool **ioPool)
        {
            gfx_assert_param(ioPool);
            
            if(ioPool) {
                [(NSAutoreleasePool *)*ioPool drain];
                *ioPool = nil;
            }
        }
        
        void autorelease_pool_add(AutoreleasePool *pool, const Base *object)
        {
            gfx_assert_param(pool);
            
            if(object)
                [[[GFXBaseWrapper alloc] initWithBaseObject:object] autorelease];
        }
        
        void autorelease_pool_current_add(const Base *object)
        {
            if(object) [[[GFXBaseWrapper alloc] initWithBaseObject:object] autorelease];
        }
        
#pragma mark -
        
        float display_default_scale_get()
        {
#if TARGET_OS_IPHONE
            return [UIScreen mainScreen].scale;
#elif TARGET_OS_MAC
            CGDirectDisplayID mainDisplay = CGMainDisplayID();
            CGDisplayModeRef displayMode = CGDisplayCopyDisplayMode(mainDisplay);
            
            size_t scale = CGDisplayModeGetPixelHeight(displayMode) / CGDisplayModeGetHeight(displayMode);
            
            CGDisplayModeRelease(displayMode);
            
            return scale;
#else
#   error Unsupported platform.
#endif
        }
    }
}
