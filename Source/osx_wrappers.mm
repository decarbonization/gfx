//
//  osx_wrappers.mm
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "osx_wrappers.h"
#include "str.h"
#include "assertions.h"

@interface GFXBaseWrapper : NSObject {
    const gfx::Base *_object;
    NSString *_className;
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
    
    [_className release];
    _className = nil;
    
    [super dealloc];
}

- (instancetype)initWithBaseObject:(const gfx::Base *)object
{
    if((self = [super init])) {
        _object = object;
        
        if(!object->isKindOfClass<gfx::String>())
            _className = [(NSString *)_object->className()->getStorage() copy];
    }
    
    return self;
}

@end

#pragma mark -

namespace gfx {
    namespace platform {
        NSAutoreleasePool *autorelease_pool_make()
        {
            return [NSAutoreleasePool new];
        }
        
        void autorelease_pool_drain(NSAutoreleasePool **ioPool)
        {
            gfx_assert_param(ioPool);
            
            [*ioPool drain];
            *ioPool = nil;
        }
        
        void autorelease_pool_add(NSAutoreleasePool *pool, const Base *object)
        {
            gfx_assert_param(pool);
            
            if(object)
                [[[GFXBaseWrapper alloc] initWithBaseObject:object] autorelease];
        }
        
        void autorelease_pool_current_add(const Base *object)
        {
            if(object) [[[GFXBaseWrapper alloc] initWithBaseObject:object] autorelease];
        }
    }
}
