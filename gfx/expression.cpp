//
//  expression.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/16/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "expression.h"

namespace gfx {
    Expression::Expression(Offset offset, Type type, Array<Base> *subexpressions) :
        mOffset(offset),
        mType(type),
        mSubexpressions(retained(subexpressions))
    {
        
    }
    
    Expression::~Expression()
    {
        released(mSubexpressions);
    }
    
#pragma mark -
    
    Array<Base> *Expression::subexpressions() const
    {
        return mSubexpressions;
    }
    
    Expression::Type Expression::type() const
    {
        return mType;
    }
    
    Offset Expression::offset() const
    {
        return mOffset;
    }
    
    const String *Expression::description() const
    {
        String::Builder description;
        
        if(mType == Type::Vector)
            description << "[";
        else if(mType == Type::Function)
            description << "{";
        
        mSubexpressions->iterate([&description](Base *value, CFIndex index, bool *stop) {
            AutoreleasePool pool;
            
            description << value << " ";
        });
        
        description.string()->deleteRange(CFRangeMake(description.length() - 1, 1));
        
        if(mType == Type::Vector)
            description << "]";
        else if(mType == Type::Function)
            description << "}";
        
        return description.string();
    }
}