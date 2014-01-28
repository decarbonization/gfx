//
//  expression.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/13/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__expression__
#define __gfx__expression__

#include "base.h"
#include "offset.h"
#include "array.h"

namespace gfx {
    ///The Expression class encapsulates a collection of other Gfx language syntax components.
    ///
    ///An Expression may either be of type Vector or Function.
    class Expression : public Base
    {
    public:
        
        ///The different types an Expression can be.
        enum class Type {
            ///The Expression is a vector.
            Vector,
            
            ///The Expression is a hash.
            Hash,
            
            ///The expression is a function.
            Function,
            
            ///The expression is a nested expression that will
            ///be evaluated like a regular expression sequence.
            Subexpression,
        };
        
    protected:
        
        ///The origin of the expression.
        Offset mOffset;
        
        ///The type of the expression.
        Type mType;
        
        ///The syntax components contained in the Expression.
        Array<Base> *mSubexpressions;
        
    public:
        ///Construct an Expression with a given offset, type, and subexpressions array.
        explicit Expression(Offset offset, Type type, Array<Base> *subexpressions);
        
        ///The destructor.
        virtual ~Expression();
        
#pragma mark - Accessors
        
        ///Returns the syntax components contained in the expression.
        Array<Base> *subexpressions() const;
        
        ///Returns the type of the expression.
        Type type() const;
        
        ///Returns the offset of the expression.
        Offset offset() const;
        
        virtual const String *description() const override;
    };
}

#endif /* defined(__gfx__expression__) */
