//
//  number.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_number_h
#define gfx_number_h

#include "base.h"
#include "str.h"

namespace gfx {
    ///The Number class encapsulates a simple `double` in an
    ///object that may be placed within an Array or Dictionary.
    class Number : public Base
    {
        ///The value of the number.
        double mValue;
        
    public:
        
        ///Returns the minimum representable number.
        static Number *Minimum()
        {
            static Number *sharedMinimum = new Number(DBL_MIN);
            return sharedMinimum;
        }
        
        ///Returns the maximum representable number.
        static Number *Maximum()
        {
            static Number *sharedMaximum = new Number(DBL_MAX);
            return sharedMaximum;
        }
        
        ///Returns a number representing false.
        static Number *False()
        {
            static Number *sharedFalse = new Number(0.0);
            return sharedFalse;
        }
        
        ///Returns a number representing true.
        static Number *True()
        {
            static Number *sharedTrue = new Number(1.0);
            return sharedTrue;
        }
        
        ///Construct a zero number.
        Number() :
            mValue(0.0)
        {
        }
        
        ///Construct a number with a given double value.
        Number(double value) :
            mValue(value)
        {
        }
        
#pragma mark - Identity
        
        ///Uses the number's double value as a hash code.
        HashCode hash() const override
        {
            return (HashCode)mValue;
        }
        
        bool isEqual(const Number *other) const
        {
            if(!other)
                return false;
            
            return (mValue == other->mValue);
        }
        
        bool isEqual(const Base *other) const override
        {
            if(!other)
                return false;
            
            if(this->isKindOfClass<Number>()) {
                return this->isEqual((const Number *)other);
            }
            
            return false;
        }
        
        const String *description() const override
        {
            String::Builder description;
            description << this->value();
            return description;
        }
        
#pragma mark - Accessing
        
        ///Returns the double value of the receiver.
        virtual double value() const
        {
            return mValue;
        }
    };
}

#endif
