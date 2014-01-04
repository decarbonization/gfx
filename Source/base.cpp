//
//  base.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/13/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "base.h"
#include <typeinfo>
#include <cxxabi.h>
#include <stack>

#include "str.h"

#if TARGET_OS_MAC
#   import "platform/osx.h"
#endif /* TARGET_OS_MAC */

namespace gfx {
    const void *RetainCallBack(CFAllocatorRef allocator, const void *value)
    {
        static_cast<const Base *>(value)->retain();
        return value;
    }
    
    void ReleaseCallBack(CFAllocatorRef allocator, const void *value)
    {
        static_cast<const Base *>(value)->release();
    }
    
    CFStringRef CopyDescriptionCallBack(const void *value)
    {
        const String *string = static_cast<const Base *>(value)->description();
        return CFStringCreateCopy(kCFAllocatorDefault, string->getStorage());
    }
    
    Boolean EqualCallBack(const void *value1, const void *value2)
    {
        return static_cast<const Base *>(value1)->isEqual(static_cast<const Base *>(value2));
    }
    
    HashCode HashCallBack(const void *value)
    {
        return static_cast<const Base *>(value)->hash();
    }
    
    
    CFArrayCallBacks const Base::kArrayCallbacks = { 0, &RetainCallBack, &ReleaseCallBack, &CopyDescriptionCallBack, &EqualCallBack };
    CFDictionaryKeyCallBacks const Base::kDictionaryKeyCallbacks = { 0, &RetainCallBack, &ReleaseCallBack, &CopyDescriptionCallBack, &EqualCallBack, &HashCallBack };
    CFDictionaryValueCallBacks const Base::kDictionaryValueCallbacks = { 0, &RetainCallBack, &ReleaseCallBack, &CopyDescriptionCallBack, &EqualCallBack };
    
#pragma mark -
    
    Base::Base() :
        mRetainCount(1)
    {
    }
    
    Base::~Base()
    {
    }
    
#pragma mark - Lifecycle
    
    void Base::retain() const
    {
        mRetainCount++;
    }
    
    void Base::release() const
    {
        if(--mRetainCount <= 0) {
            delete this;
        }
    }
    
    Base::RetainCount Base::retainCount() const
    {
        return mRetainCount;
    }
    
#pragma mark - Identity
    
    HashCode Base::hash() const
    {
        return (HashCode)this;
    }
    
    const String *Base::className() const
    {
        int status = 0;
        size_t length = 0;
        char *className = abi::__cxa_demangle(typeid(*this).name(), NULL, &length, &status);
        if(className) {
            cf::StringAutoRef string = CFStringCreateWithBytes(kCFAllocatorDefault, (UInt8 *)className, length - 1, kCFStringEncodingUTF8, false);
            free(className);
            return make<String>(string);
        } else {
            return nullptr;
        }
    }
    
    const String *Base::description() const
    {
        String::Builder description;
        
        description << "<" << this->className() << ":" << (void *)this << ">";
        
        return description;
    }
    
    bool Base::isEqual(const Base *other) const
    {
        return (this == other);
    }
    
#pragma mark - Auto Lifecycle
    
#if !TARGET_OS_MAC
    static std::stack<AutoreleasePool *> pools;
    
    static void pushPool(AutoreleasePool *pool)
    {
        pools.push(pool);
    }
    
    static void popPool()
    {
        pools.pop();
    }
    
    static AutoreleasePool *getCurrentPool()
    {
        if(pools.size() > 0) {
            return pools.top();
        } else {
            fprintf(stderr, "*** Warning, no autorelease pool in place, just leaking.\n");
            return nullptr;
        }
    }
#endif /* !TARGET_OS_MAC */
    
#pragma mark - • AutoreleasePool
    
    void AutoreleasePool::Autorelease(const Base *object)
    {
        if(!object)
            return;
        
#if TARGET_OS_MAC
        //There can exist NSAutoreleasePools long before the gfx stack
        //is fully up and running, so on OS X we bypass our autorelease
        //pool stack completely.
        platform::autorelease_pool_current_add(object);
#else
        AutoreleasePool *currentPool = getCurrentPool();
        if(currentPool)
            currentPool->add(object);
#endif /* TARGET_OS_MAC */
    }
    
#pragma mark - Lifecycle
    
    AutoreleasePool::AutoreleasePool() :
#if TARGET_OS_MAC
        mPool(platform::autorelease_pool_make())
#else
        mStorage()
#endif /* TARGET_OS_MAC */
    {
#if !TARGET_OS_MAC
        pushPool(this);
#endif /* !TARGET_OS_MAC */
    }
    
    AutoreleasePool::~AutoreleasePool()
    {
#if TARGET_OS_MAC
        platform::autorelease_pool_drain(&mPool);
#else
        for (const Base *object : mStorage) {
            object->release();
        }
        
        popPool();
#endif /* TARGET_OS_MAC */
    }
    
    void AutoreleasePool::add(const Base *object)
    {
#if TARGET_OS_MAC
        platform::autorelease_pool_add(mPool, object);
#else
        mStorage.push_back(object);
#endif /* TARGET_OS_MAC */
    }
    
    void Base::autorelease() const
    {
        AutoreleasePool::Autorelease(this);
    }
}
