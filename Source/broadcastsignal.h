//
//  broadcastsignal.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_broadcastsignal_h
#define gfx_broadcastsignal_h

#include "str.h"
#include <vector>
#include <unordered_map>

namespace gfx {
    class String;
    
    struct Nothing {};
    
    ///The Signal class encapsulates a simple one-to-many notification system.
    ///
    /// \tparam Param   The type of the parameter passed onto observer functors.
    ///                 If a value type is specified, that value will be copied
    ///                 for each observer functor invocation.
    ///
    ///Usage of the signal class is simple. Assuming `Signal<int> mSignal`,
    ///observers add new functors through the `gfx::Signal::add` method like
    ///`mSignal.add([](int value) { ... })`. Broadcasters transmit through the
    ///signal through a simple application like `mSignal(12)`.
    template<typename Param = Nothing>
    class Signal
    {
    public:
        
        ///A functor that will be invoked whenever a Signal broadcasts.
        typedef std::function<void(Param)> ObserverFunctor;
        
        ///An opaque reference to an observer returned by `gfx::Signal::add`
        ///that is used to later remove observers from the Signal.
        typedef long ObserverReference;
        
    protected:
        
        ///The name of the Signal, if any.
        const String *mName;
        
        ///A unique value incremented and assigned to each new observer added to the signal.
        ObserverReference mReferenceCounter;
        
        ///The observers of the signal.
        std::unordered_map<ObserverReference, ObserverFunctor> mObservers;
        
    public:
        
        ///Constructs the signal with a given name.
        ///
        /// \param  name    The name of the signal. May be null.
        ///
        Signal(const String *name = nullptr) :
            mName(retained(name)),
            mReferenceCounter(0),
            mObservers()
        {
        }
        
        ///The destructor of the signal.
        ~Signal()
        {
            released(mName);
        }
        
#pragma mark - Introspection
        
        ///Returns the name of the signal.
        const String *name() const { return retained_autoreleased(mName); }
        
#pragma mark - Managing Observers
        
        ///Adds a new observer to the receiver, returning an observer
        ///refernece that can be used to remove the observer later.
        ///
        /// \param  observer    The observer functor to add.
        ///
        /// \result A reference to the observer to use to remove the observer later.
        ///
        ObserverReference add(const ObserverFunctor &observer)
        {
            mReferenceCounter++;
            mObservers[mReferenceCounter] = observer;
            
            return mReferenceCounter;
        }
        
        ///Synonym for `gfx::Signal<>::add`.
        ObserverReference operator+=(const ObserverFunctor &observer)
        {
            return add(observer);
        }
        
        ///Remove an observer using the refernece returned by `gfx::Signal::add`.
        ///
        /// \param  observerRef The reference returned by add.
        ///
        void remove(ObserverReference observerRef)
        {
            mObservers.erase(observerRef);
        }
        
#pragma mark - Dispatching
        
        ///Broadcast to every observer in the Signal.
        ///
        /// \param  param   The parameter to broadcast along with the signal.
        ///
        ///This method swallows `std::bad_function_call` exceptions.
        void broadcast(Param param) const
        {
            for (const std::pair<ObserverReference, ObserverFunctor> &observer : mObservers) {
                try {
                    observer.second(param);
                } catch (std::bad_function_call) {
                    //Don't care.
                }
            }
        }
        
        ///Synonym for `gfx::Signal::broadcast`.
        void operator() (Param param = Param()) const
        {
            broadcast(param);
        }
    };
}

#endif
