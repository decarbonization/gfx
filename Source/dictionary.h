//
//  dictionary.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_dictionary_h
#define gfx_dictionary_h

#include <CoreFoundation/CoreFoundation.h>
#include <initializer_list>
#include "base.h"
#include "str.h"
#include "number.h"

namespace gfx {
    
    ///The Dictionary class encapsulates a collection of key-value associations.
    ///
    /// \tparam Key     The Base-derived type used for keys. Defaults to `const String`.
    /// \tparam value   The Base-derived type used for values. Defaults to `Base`.
    ///
    ///All instances of `Dictionary` have mutable storage, to represent an
    ///immutable dictionary, qualify the instance with the const modifier.
    template<typename Key = const String, typename Value = Base>
    class Dictionary : public Base
    {
        static_assert(std::is_base_of<Base, Key>::value, "Dictionary requires Base-derived key types");
        static_assert(std::is_base_of<Base, Value>::value, "Dictionary requires Base-derived value types");
        static_assert(!std::is_pointer<Key>::value, "Key must be a bare type");
        static_assert(!std::is_pointer<Value>::value, "Value must be a bare type");
        
    public:
        
        ///A structure that contains a weak reference to a key object,
        ///and value object. Used for simple inline construction.
        struct Pair
        {
            ///The key of the pair.
            Key *key;
            
            ///The value of the pair.
            Value *value;
        };
        
    protected:
        
        ///The storage of the Dictionary.
        CFMutableDictionaryRef mStorage;
        
    public:
        
        ///Constructs an empty dictionary.
        Dictionary() :
            mStorage(CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &Base::kDictionaryKeyCallbacks, &Base::kDictionaryValueCallbacks))
        {
        }
        
        ///Constructs a dictionary by copying a CFDictionary's contents.
        Dictionary(CFDictionaryRef dictionary) :
            mStorage(CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, dictionary))
        {
        }
        
        ///Constructs a dictionary by copying another dictionary.
        Dictionary(const Dictionary<Key, Value> *dictionary) :
            Dictionary(dictionary->getStorage())
        {
        }
        
        ///Constructs a dictionary with an initializer list of pairs.
        Dictionary(const std::initializer_list<Pair> &list) :
            Dictionary()
        {
            for (const Pair &pair : list)
                this->set(pair.key, pair.value);
        }
        
        ///The destructor of the dictionary.
        virtual ~Dictionary()
        {
            CFRelease(mStorage);
        }
        
#pragma mark - Identity
        
        HashCode hash() const override
        {
            return CFHash(mStorage);
        }
        
        const String *description() const override
        {
            String::Builder description;
            description << "{\n";
            
            iterate([&description](Key *key, Value *value) {
                AutoreleasePool pool;
                
                description << "\t";
                description << key;
                description << " = ";
                description << value;
                description << ",\n";
            });
            
            description.string()->deleteRange(Range(description.length() - 1, 1));
            description << "\n}";
            
            return description.string();
        }
        
        bool isEqual(const Dictionary<Key, Value> *other) const
        {
            if(!other)
                return false;
            
            return CFEqual(mStorage, other->mStorage);
        }
        
        bool isEqual(const Base *other) const override
        {
            if(!other)
                return false;
            
            if(this->isKindOfClass<Dictionary<Key, Value>>()) {
                return this->isEqual((const Dictionary<Key, Value> *)other);
            }
            
            return false;
        }
        
        ///Returns the underlying storage of the dictionary.
        ///
        ///This should be considered an implementation detail,
        ///but is exposed for platform compatibility.
        CFDictionaryRef getStorage() const
        {
            return (CFDictionaryRef)mStorage;
        }
        CFMutableDictionaryRef getStorage()
        {
            return mStorage;
        }
        
#pragma mark - Accessing Values
        
        ///Returns the number of key-value associations stored in the dictionary.
        Index count() const
        {
            return CFDictionaryGetCount(mStorage);
        }
        
        ///Returns the number of instances of a given key are stored in the receiver.
        Index countOfKey(Key *key) const
        {
            gfx_assert_param(key);
            
            return CFDictionaryGetCountOfKey(mStorage, (const void *)key);
        }
        
        ///Returns the number of instances of a given value are stored in the receiver.
        Index countOfValue(Value *value) const
        {
            gfx_assert_param(value);
            
            return CFDictionaryGetCountOfKey(mStorage, (const void *)value);
        }
        
#pragma mark -
        
        ///Returns a bool indicating whether or not the receiver contains a given key.
        bool containsKey(Key *key) const
        {
            gfx_assert_param(key);
            
            return CFDictionaryContainsKey(mStorage, (const void *)key);
        }
        
        ///Returns a bool indicating whether or not the receiver contains a given value.
        bool containsValue(Value *value) const
        {
            gfx_assert_param(value);
            
            return CFDictionaryContainsValue(mStorage, (const void *)value);
        }
        
#pragma mark -
        
        ///Returns the value associated with a given key, if any.
        Value *get(Key *key) const
        {
            gfx_assert_param(key);
            
            return retained_autoreleased((Value *)CFDictionaryGetValue(mStorage, key));
        }
        
    private:
        
        static void applierAdaptor(Key *key, Value *value, std::function<void(Key *key, Value *value)> *function)
        {
            (*function)(key, value);
        }
        
    public:
        
        ///Enumerate over each key-value pair contained within the dictionary, applying a given function to each.
        void iterate(std::function<void(Key *key, Value *value)> function) const
        {
            CFDictionaryApplyFunction(mStorage, (CFDictionaryApplierFunction)&applierAdaptor, &function);
        }
        
#pragma mark - Changing Values
        
        ///Set the value associated with a given key.
        void set(Key *key, Value *value)
        {
            gfx_assert_param(key);
            
            CFDictionarySetValue(mStorage, key, value);
        }
        
        ///Take all of the values contained in a
        ///given dictionary and add them to the receiver.
        void takeValuesFrom(const Dictionary<Key, Value> *other)
        {
            other->iterate([this](Key *key, Value *value) {
                this->set(key, value);
            });
        }
        
        ///Remove the value associated with a given key.
        void remove(Key *key)
        {
            gfx_assert_param(key);
            
            CFDictionaryRemoveValue(mStorage, key);
        }
        
        ///Removes all of the contents of the receiver.
        void removeAll()
        {
            CFDictionaryRemoveAllValues(mStorage);
        }
        
#pragma mark - Utilities
        
        ///Creates and returns a new object that can be used in place of a given object
        ///within a dictionary when it is not appropriate the retain the given object.
        ///
        /// \param  object  The object it is not appropriate to retain. May not be null.
        ///
        /// \result A new object guaranteed to always have the same hash code given the same object.
        ///
        ///The object returned by this function is an implementation detail and subject to change.
        static Base *weakKeyForObject(const Base *object)
        {
            gfx_assert_param(object);
            
            return (String::Builder() << (void *)object);

        }
    };
}

#endif
