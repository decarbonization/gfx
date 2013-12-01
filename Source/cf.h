//
//  cf.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/14/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_cf_h
#define gfx_cf_h

#include <CoreFoundation/CoreFoundation.h>

namespace gfx {
    namespace cf {
        ///Retains a given CF object of type T, returning it.
        template<typename T> T retained(T value)
        {
            CFRetain(value);
            return value;
        }
        
        ///The AutoRef class wraps instances of CFBase types and ensures exception safe disposal.
        ///
        /// \param  T   A CFBase-derived type.
        ///
        template<typename T>
        class AutoRef
        {
            T mValue;
            
        public:
            
            AutoRef(T value) :
                mValue(value)
            {
            }
            
            AutoRef(const AutoRef<T> &other) :
                mValue(other.mValue)
            {
                CFRetain(mValue);
            }
            
            ~AutoRef()
            {
                if(mValue) {
                    CFRelease(mValue);
                    mValue = NULL;
                }
            }
            
            operator T() const
            {
                return mValue;
            }
            
            operator CFTypeRef() const
            {
                return (CFTypeRef)mValue;
            }
            
            T operator->() const
            {
                return mValue;
            }
            
            T operator *() const
            {
                return mValue;
            }
        };
        
        ///Scope-bound CFStringRef.
        typedef AutoRef<CFStringRef> StringAutoRef;
        
        ///Scope-bound CFMutableStringRef.
        typedef AutoRef<CFMutableStringRef> MutableStringAutoRef;
        
        ///Scope-bound CFNumberRef.
        typedef AutoRef<CFNumberRef> NumberAutoRef;
        
        ///Scope-bound CFArrayRef.
        typedef AutoRef<CFArrayRef> ArrayAutoRef;
        
        ///Scope-bound CFMutableStringRef.
        typedef AutoRef<CFMutableArrayRef> MutableArrayAutoRef;
        
        ///Scope-bound CFDictionaryRef.
        typedef AutoRef<CFDictionaryRef> DictionaryAutoRef;
        
        ///Scope-bound CFMutableDictionaryRef.
        typedef AutoRef<CFMutableDictionaryRef> MutableDictionaryAutoRef;
    }
}

#endif
