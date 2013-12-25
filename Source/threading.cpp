//
//  threading.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "threading.h"
#include <pthread.h>
#include <CoreFoundation/CoreFoundation.h>

namespace gfx {
    namespace threading {
        StorageDictionary *threadStorage()
        {
            static pthread_key_t sharedThreadStorageKey;
            static pthread_once_t guard;
            pthread_once(&guard, []{
                auto destructor = [](void *inStorage) {
                    try {
                        released((Dictionary<String, Base> *)inStorage);
                    } catch (std::exception e) {
                        std::cerr << "*** Warning, swallowed exception '" << e.what() << "' from the gfx::Context stack thread destructor." << std::endl;
                    } catch (...) {
                        std::cerr << "*** Warning, swallowed unknown exception from the gfx::Context stack thread destructor." << std::endl;
                    }
                };
                
                gfx_assert((pthread_key_create(&sharedThreadStorageKey, destructor) == 0),
                           str("Could not create shared context stack key"));
            });
            
            auto threadStorage = (StorageDictionary *)pthread_getspecific(sharedThreadStorageKey);
            
            if(threadStorage == nullptr) {
                threadStorage = new StorageDictionary();
                pthread_setspecific(sharedThreadStorageKey, threadStorage);
            }
            
            return threadStorage;
        }
        
#pragma mark -
        
        void performOnMainThread(std::function<void()> task)
        {
            if(!task)
                return;
            
            CFRunLoopPerformBlock(CFRunLoopGetMain(), kCFRunLoopCommonModes, ^{
                task();
            });
        }
    }
}
