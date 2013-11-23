//
//  function.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_function_h
#define gfx_function_h

#include "base.h"

namespace gfx {
    class StackFrame;
    class Expression;
    
    class Function : public Base {
    public:
        virtual void apply(StackFrame *stack) const = 0;
    };
    
    class NativeFunction : public Function {
    public:
        typedef std::function<void(StackFrame *stack)> Type;
        
    protected:
        
        NativeFunction::Type mImplementation;
        const String *mName;
        
    public:
        
        NativeFunction(const String *name, NativeFunction::Type implementation) :
            mName(retained(name)),
            mImplementation(implementation)
        {
        }
        
        ~NativeFunction()
        {
            released(mName);
        }
        
        const String *name() const
        {
            return mName;
        }
        
        virtual const String *description() const override;
        
        virtual void apply(StackFrame *stack) const;
    };
    
    class InterpretedFunction : public Function {
        Expression *mSource;
        
    public:
        explicit InterpretedFunction(Expression *source);
        virtual ~InterpretedFunction();
        
        virtual void apply(StackFrame *stack) const override;
        
        virtual const String *description() const override;
    };
}

#endif
