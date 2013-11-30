//
//  base.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/13/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__base__
#define __gfx__base__

#include <CoreFoundation/CoreFoundation.h>
#include <iostream>
#include <atomic>
#include "cf.h"
#include "assertions.h"
#include "types.h"

namespace gfx {
    class String;
    
    ///The Base class encapsulates a simple reference counted
    ///object intended to always be allocated on the heap.
    ///
    /// \seealso(AutoreleasePool)
    class Base
    {
    public:
        
        typedef size_t RetainCount;
        
    protected:
        
        ///The retain count of the instance. Starts at 1.
        mutable std::atomic<RetainCount> mRetainCount;
        
    public:
        ///Callbacks to make Base and Base-derived objects function within CFArray instances.
        static CFArrayCallBacks const kArrayCallbacks;
        
        ///Callbacks to make Base and Base-derived objects function as keys within CFDictionary instances.
        static CFDictionaryKeyCallBacks const kDictionaryKeyCallbacks;
        
        ///Callbacks to make Base and Base-derived objects function as values within CFDictionary instances.
        static CFDictionaryValueCallBacks const kDictionaryValueCallbacks;
        
        
        ///The default constructor for Base.
        ///
        ///Initializes the retain count.
        explicit Base();
        
        ///The destructor for Base.
        virtual ~Base();
        
#pragma mark - Lifecycle
        
        ///Atomically increments the object's retain count by 1.
        virtual void retain() const;
        
        ///Atomically decrements the object's retain count by 1,
        ///performing `delete this` if the retain count reaches 0.
        virtual void release() const;
        
        ///Decrements the object's retain count by 1 at the end of the current autorelease pool block.
        ///
        ///This method logs a warning each time it is called without an autorelease pool in place.
        ///
        /// \seealso(AutoreleasePool)
        virtual void autorelease() const;
        
        ///Returns the current retain count of the object.
        ///
        ///This should be treated as an instantaneous value,
        ///and should only be used for debugging purposes.
        virtual RetainCount retainCount() const;
        
#pragma mark - Identity
        
        ///Returns a code that can be used to identify the object in a hashing structure.
        ///
        ///The default implementation uses the pointer value of `this`.
        virtual HashCode hash() const;
        
        ///Returns the class name of the object.
        ///
        ///The value returned by this method may not be human readable.
        virtual const String *className() const;
        
        
        ///Returns a bool indicating whether or not the receiver
        ///is an instance or inherits from a given template class T.
        ///
        /// \param  T   The class.
        ///
        /// \result true if the object is an instance of `T`, or inherits from it.
        template<typename T>
        bool isKindOfClass() const
        {
            static_assert(std::is_base_of<Base, T>::value, "Base::isKindOfClass requires Base-derived types");
            static_assert(!std::is_pointer<T>::value, "T must be a bare type");
            
            return (dynamic_cast<const T *>(this) != nullptr);
        }
        
        
        ///Returns a textual description of the object.
        virtual const String *description() const;
        
        ///Returns a bool indicating whether or not the object matches another given object.
        ///
        /// \param  other   The object to compare the receiver against. *May be null.*
        ///
        /// \result true if the receiver matches `other`; false otherwise.
        ///
        ///The default implementation performs a simple pointer comparison.
        virtual bool isEqual(const Base *other) const;
    };
    
#pragma mark - Auto Lifecycle
    
    ///The AutoreleasePool class wraps an array of Base instances that will
    ///be told to `Base::release` when the AutoreleasePool is destructed.
    ///This mechanism enables objects allocated on the heap to be cleanly
    ///destroyed without receiving parties having to manage all lifecycles.
    class AutoreleasePool final
    {
        CFMutableArrayRef mStorage;
        
    public:
        
        ///The constructor.
        AutoreleasePool();
        
        ///The destructor.
        ~AutoreleasePool();
        
        ///Add an object to the receiver to be
        ///released when the pool goes out of scope.
        ///
        ///This method is currently *not* thread safe.
        void add(const Base *object);
        
    private:
        
        AutoreleasePool(const AutoreleasePool &);
        AutoreleasePool &operator=(AutoreleasePool &);
    };
    
#pragma mark - Memory Operations
    
    ///Retains an instance of Base-derived type T, returning it.
    ///
    /// \param  object  The object to retain. May be null.
    ///
    /// \result object.
    ///
    ///This function should be preferred over calling
    ///`Base::retain` directly as it is null safe.
    template<typename T>
    T *retained(T *object)
    {
        static_assert(std::is_base_of<Base, T>::value, "retained requires Base-derived types");
        static_assert(!std::is_pointer<T>::value, "T must be a bare type");
        
        if(object)
            object->retain();
        
        return object;
    }
    
    ///Autoreleases an instance of Base-derived type T, returning it.
    ///
    /// \param  object  The object to autorelease. May be null.
    ///
    /// \result object.
    ///
    ///This function should be preferred over calling
    ///`Base::autorelease` directly as it is null safe.
    template<typename T>
    T *autoreleased(T *object)
    {
        static_assert(std::is_base_of<Base, T>::value, "autoreleased requires Base-derived types");
        static_assert(!std::is_pointer<T>::value, "T must be a bare type");
        
        if(object)
            object->autorelease();
        
        return object;
    }
    
    ///Retains and autoreleases an instance of Base-derived type T, returning it.
    ///
    /// \param  object  The object to retain and autorelease. May be null.
    ///
    /// \result object.
    ///
    template<typename T>
    T *retained_autoreleased(T *object)
    {
        static_assert(std::is_base_of<Base, T>::value, "retained_autoreleased requires Base-derived types");
        static_assert(!std::is_pointer<T>::value, "T must be a bare type");
        
        return autoreleased(retained(object));
    }
    
    ///Releases an instance of Base-derived type T.
    ///
    /// \param  object  The object to release. May be null.
    ///
    ///This function should be preferred over calling
    ///`Base::release` directly as it is null safe.
    template<typename T>
    void released(T *object)
    {
        static_assert(std::is_base_of<Base, T>::value, "released requires Base-derived types");
        static_assert(!std::is_pointer<T>::value, "T must be a bare type");
        
        if(object)
            object->release();
    }
    
#pragma mark - Creation
    
    ///Copies an instance of Base-derived type T that has
    ///a constructor that accepts an instance of type T.
    ///
    /// \tparam  T   A subclass of Base that has a constructor of type `(const T *)`.
    ///
    /// \param  object  The object to copy.
    ///
    /// \result A copy of `object`.
    ///
    template<typename T>
    T *copy(T *object)
    {
        static_assert(std::is_base_of<Base, T>::value, "copy requires Base-derived types");
        static_assert(!std::is_pointer<T>::value, "T must be a bare type");
        
        if(!object)
            return nullptr;
        
        return new T(object);
    }
    
    ///Constructs a new instan ce of a given Base-derived type T, subsequently autoreleasing it.
    ///
    /// \tparam T       A Base-derived type.
    /// \param  Args... The constructor arguments of T.
    ///
    /// \param  args    The constructor arguments of T.
    ///
    /// \result A newly constructed instance of T that has been autoreleased.
    template<typename T, typename... Args> T *make(Args... args)
    {
        static_assert(std::is_base_of<Base, T>::value, "make requires Base-derived types");
        static_assert(!std::is_pointer<T>::value, "T must be a bare type");
        
        return autoreleased(new T(std::forward<Args>(args)...));
    }
}

#endif /* defined(__gfx__base__) */
