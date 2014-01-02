//
//  threading.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__threading__
#define __gfx__threading__

#include "dictionary.h"
#include <functional>

namespace gfx {
    
    ///The threading namespace contains types and functions to aid
    ///in working in a multi-threaded environment.
    namespace threading {
        ///Short-hand for the dictionary type used by
        ///the `gfx::threading::threadStorage` function.
        typedef Dictionary<const Base, Base> StorageDictionary;
        
        ///Returns the storage dictionary for the current thread,
        ///creating it if it does not already exist.
        ///
        ///The returned dictionary has its lifecycle directly tied
        ///to the lifecycle of the calling thread. The thread will
        ///be immediately released when the calling thread is destroyed.
        extern StorageDictionary *threadStorage();
        
#pragma mark -
        
        ///Schedules a task function to be performed on the main thread.
        ///
        /// \param  task    The function to invoke on the main thread. Ignored if invalid.
        ///
        ///This function assumes that a run loop is in operation. The task
        ///function will not be executed if there is no active run loop.
        extern void performOnMainThread(std::function<void()> task);
    }
}

#endif /* defined(__gfx__threading__) */
