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
#include "string.h"

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
            return make<Number>(DBL_MIN);
        }
        
        ///Returns the maximum representable number.
        static Number *Maximum()
        {
            return make<Number>(DBL_MAX);
        }
        
        ///Returns a number representing false.
        static Number *False()
        {
            return make<Number>(0.0);
        }
        
        ///Returns a number representing true.
        static Number *True()
        {
            return make<Number>(1.0);
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
