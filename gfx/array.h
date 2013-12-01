//
//  array.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/14/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_array_h
#define gfx_array_h

#include "base.h"
#include "string.h"
#include "exception.h"
#include <initializer_list>

namespace gfx {
    
    ///The Array class encapsulates an ordered collection of Base-derived objects.
    ///
    /// \tparam T   The Base-derived type stored within the array. Defaults to `Base`.
    ///
    ///All instances of `Array` have mutable storage, to represent an
    ///immutable array, qualify the instance with the const modifier.
    template<typename T = Base>
    class Array : public Base
    {
        static_assert(std::is_base_of<Base, T>::value, "Array requires Base-derived types");
        static_assert(!std::is_pointer<T>::value, "T must be a bare type");
        
        ///The storage of the array.
        CFMutableArrayRef mStorage;
        
    public:
        
        ///Constructs an empty array.
        Array() :
            mStorage(CFArrayCreateMutable(kCFAllocatorDefault, 0, &Base::kArrayCallbacks))
        {
        }
        
        ///Constructs an array by copying the contents of given array.
        Array(const Array<T> *other) :
            mStorage(CFArrayCreateMutableCopy(kCFAllocatorDefault, other->count(), other->mStorage))
        {
        }
        
        ///Constructs an array by copying the contents of a given CFArray.
        Array(CFArrayRef array) :
            mStorage(CFArrayCreateMutableCopy(kCFAllocatorDefault, CFArrayGetCount(array), array))
        {
        }
        
        ///Constructs an array using an initializer list.
        Array(std::initializer_list<T *> list) :
            Array()
        {
            for (T *value : list)
                append(value);
        }
        
        ///The destructor.
        ~Array()
        {
            if(mStorage) {
                CFRelease(mStorage);
                mStorage = NULL;
            }
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
            
            iterate([&description](T *value, Index index, bool *stop) {
                AutoreleasePool pool;
                
                description << "\t";
                description << value->description();
                description << ",\n";
            });
            
            description.string()->deleteRange(Range(description.length() - 1, 1));
            description << "\n}";
            
            return description.string();
        }
        
        bool isEqual(const Array *other) const
        {
            if(!other)
                return false;
            
            return CFEqual(mStorage, other->mStorage);
        }
        
        bool isEqual(const Base *other) const override
        {
            if(!other)
                return false;
            
            if(this->isKindOfClass<Array<T>>()) {
                return this->isEqual((const Array<T> *)other);
            }
            
            return false;
        }
        
#pragma mark - Storage
        
        ///Returns the underlying storage of the dictionary.
        ///
        ///This should be considered an implementation detail,
        ///but is exposed for platform compatibility.
        CFArrayRef getStorage() const
        {
            return (CFArrayRef)mStorage;
        }
        CFMutableArrayRef getStorage()
        {
            return mStorage;
        }
        
#pragma mark - Reading
        
        ///Returns the number of values contained within the receiver.
        Index count() const
        {
            return CFArrayGetCount(mStorage);
        }
        
        ///Returns the value stored at a given index.
        ///
        /// \throws Exception for out of bounds errors.
        T *at(Index index) const
        {
            gfx_assert((index < this->count()), "out of range access"_gfx);
            
            auto value = (T *)CFArrayGetValueAtIndex(mStorage, index);
            return retained_autoreleased(value);
        }
        
        ///Returns a new subarray with the values
        ///contained within a given range of the receiver.
        Array<T> *subarray(Range range) const
        {
            const void *rawObjects[range.length];
            CFArrayGetValues(mStorage, range, rawObjects);
            
            cf::ArrayAutoRef tempArray = CFArrayCreate(kCFAllocatorDefault, rawObjects, range.length, NULL);
            return make<Array<T>>(tempArray);
        }
        
        ///Returns the first value contained in the receiver, or null if the array is empty.
        T *first() const
        {
            Index count = this->count();
            if(count == 0)
                return nullptr;
            else
                return this->at(0);
        }
        
        ///Returns the last value contained in the receiver, or null if the array is empty.
        T *last() const
        {
            Index count = this->count();
            if(count == 0)
                return nullptr;
            else
                return this->at(count - 1);
        }
        
        ///Returns the first index of a given value within the receiver,
        ///or kCFNotFound if the value is not contained within the array.
        Index firstIndexOf(Range range, T *value) const
        {
            return CFArrayGetFirstIndexOfValue(mStorage, range, value);
        }
        
        ///Returns the last index of a given value within the receiver,
        ///or kCFNotFound if the value is not contained within the array.
        Index lastIndexOf(Range range, T *value) const
        {
            return CFArrayGetLastIndexOfValue(mStorage, range, value);
        }
        
        ///Returns a bool indicating whether or not the array contains a given value.
        bool contains(Range range, T *value) const
        {
            return CFArrayContainsValue(mStorage, range, value);
        }
        
#pragma mark - Mutation
        
        ///Inserts a given value at a given index within the receiver.
        void insertAt(T *value, Index index)
        {
            gfx_assert((index < this->count()), "out of range"_gfx);
            gfx_assert_param(value);
            
            CFArrayInsertValueAtIndex(mStorage, index, value);
        }
        
        ///Appends a given value to the end of the receiver.
        void append(T *value)
        {
            gfx_assert_param(value);
            
            CFArrayAppendValue(mStorage, value);
        }
        
        ///Appends the values contained in an array to the end of the receiver.
        void appendArray(const Array<T> *array)
        {
            gfx_assert_param(array);
            
            CFArrayAppendArray(mStorage, array->getStorage(), Range(0, array->count()));
        }
        
        ///Removes the value at a given index.
        ///
        /// \throws Exception for out of bounds errors.
        void removeAt(Index index)
        {
            gfx_assert((index < this->count()), "out of range"_gfx);
            
            CFArrayRemoveValueAtIndex(mStorage, index);
        }
        
        void remove(T *value)
        {
            gfx_assert_param(value);
            
            Index indexOfValue = this->firstIndexOf(Range(0, count()), value);
            if(indexOfValue != kCFNotFound)
                this->removeAt(indexOfValue);
        }
        
        ///Removes the first object contained in the array.
        ///Does nothing if the array is empty.
        void removeFirst()
        {
            Index count = this->count();
            if(count > 0)
                this->removeAt(0);
        }
        
        ///Removes the last object contained in the array.
        ///Does nothing if the array is empty.
        void removeLast()
        {
            Index count = this->count();
            if(count > 0)
                this->removeAt(count - 1);
        }
        
        ///Removes all of the values contained within the array.
        void removeAll()
        {
            CFArrayRemoveAllValues(mStorage);
        }
        
        ///Exchanges the values at the two given indexes within the receiver.
        ///
        /// \throws Exception for out of bounds errors.
        void exchange(Index index1, Index index2)
        {
            gfx_assert((index1 < this->count()), "out of range"_gfx);
            gfx_assert((index2 < this->count()), "out of range"_gfx);
            
            CFArrayExchangeValuesAtIndices(mStorage, index1, index2);
        }
        
    private:
        
        static CFComparisonResult comparer(T *val1, T *val2, std::function<CFComparisonResult(T *left, T *right)> *comparator)
        {
            return (*comparator)(val1, val2);
        }
        
    public:
        
        ///Sorts the contents of the array using a given comparator function.
        void sort(Range range, std::function<CFComparisonResult(T *left, T *right)> comparator)
        {
            CFArraySortValues(mStorage, range, (CFComparatorFunction)&comparer, &comparator);
        }
        
#pragma mark - Iteration
        
    public:
        
        ///Enumerates the contents of the receiver, applying a given function with each value.
        void iterate(std::function<void(T *value, Index index, bool *stop)> function) const
        {
            bool stop = false;
            for (Index index = 0, count = this->count(); index < count; index++) {
                function(this->at(index), index, &stop);
                
                if(stop)
                    break;
            }
        }
        
        ///Maps the contents of the receiver, applying a given function with each value,
        ///and placing the returned value of the function into a new array.
        const Array *map(std::function<T *(T *value, Index index, bool *stop)> function) const
        {
            Array<T> *newArray = make<Array<T>>();
            
            bool stop = false;
            for (Index index = 0, count = this->count(); index < count; index++) {
                T *newValue = function(this->at(index), index, &stop);
                newArray->append(newValue);
                if(stop)
                    break;
            }
            
            return newArray;
        }
        
        ///Filters the contents of the receiver, applying a given function with each value,
        ///and placing the values for which the function returns true in a new array.
        const Array *filter(std::function<bool(T *value, Index index, bool *stop)> function) const
        {
            Array<T> *newArray = make<Array<T>>();
            
            bool stop = false;
            for (Index index = 0, count = this->count(); index < count; index++) {
                T *value =this->at(index);
                if(function(value, index, &stop))
                   newArray->append(value);
                
                if(stop)
                    break;
            }
            
            return newArray;
        }
        
#pragma mark -
        
        ///The Iterator class exists to enable Array to be used with the range-based for loop.
        class Iterator
        {
            const Array *mArray;
            Index mOffset;
            
        public:
            
            explicit Iterator(const Array *inArray, bool end) :
                mArray(retained(inArray)),
                mOffset(end? inArray->count() : 0)
            {
            }
            
            Iterator(const Iterator& other) :
                mArray(other.mArray),
                mOffset(other.mOffset)
            {
            }
            
            ~Iterator()
            {
                mArray->release();
            }
            
            Iterator& operator++()
            {
                ++mOffset;
                return *this;
            }
            
            Iterator operator++(int)
            {
                Iterator tmp(*this);
                operator++();
                return tmp;
            }
            
            bool operator==(const Iterator& rhs)
            {
                return mOffset == rhs.mOffset;
            }
            
            bool operator!=(const Iterator& rhs)
            {
                return mOffset != rhs.mOffset;
            }
            
            T *operator*()
            {
                return mArray->at(mOffset);
            }
        };
    };
    
    template<typename T>
    static inline typename Array<T>::Iterator begin(const Array<T> *array)
    {
        return typename Array<T>::Iterator(array, false);
    }
    
    template<typename T>
    static inline typename Array<T>::Iterator end(const Array<T> *array)
    {
        return typename Array<T>::Iterator(array, true);
    }
    
#pragma mark - String Operations
    
    ///Splits a given string using a given separator string, returning an array.
    ///
    /// \param  string          The string to split. Required.
    /// \param  separatorString The separator string to search for. Required.
    ///
    /// \result A new autoreleased Array<String> instance.
    static inline Array<String> *SplitString(const String *string, const String *separatorString)
    {
        gfx_assert_param(string);
        gfx_assert_param(separatorString);
        
        auto results = make<Array<String>>();
        cf::ArrayAutoRef cfStrings = CFStringCreateArrayBySeparatingStrings(kCFAllocatorDefault, string->getStorage(), separatorString->getStorage());
        for (Index index = 0, count = CFArrayGetCount(cfStrings); index < count; index++) {
            results->append(make<String>((CFStringRef)CFArrayGetValueAtIndex(cfStrings, index)));
        }
        
        return results;
    }
    
    ///Joins the contents of a given array using a given separator string.
    ///
    /// \tparam T   Any Base-derived type. Description will be called for each.
    ///
    /// \param  values          An array of Base-derived values who have overriden description methods. Required.
    /// \param  separatorString The string to merge the values with. Required.
    ///
    /// \result A new autoreleased String instance.
    template<typename T>
    static inline String *JoinArray(const Array<T> *values, const String *separatorString)
    {
        gfx_assert_param(values);
        gfx_assert_param(separatorString);
        
        cf::MutableArrayAutoRef cfStrings = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        values->iterate([cfStrings](Base *value, Index index, bool *stop) {
            CFArrayAppendValue(cfStrings, value->description()->getStorage());
        });
        
        cf::StringAutoRef joinedCFString = CFStringCreateByCombiningStrings(kCFAllocatorDefault, cfStrings, separatorString->getStorage());
        return make<String>(joinedCFString);
    }
}

#endif
