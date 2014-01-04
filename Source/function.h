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
#include <exception>

namespace gfx {
    class StackFrame;
    class Expression;
    
#if GFX_Language_SupportsRecursion
    
    ///The RecursionMarkerException is used by the __recurse
    ///core function to indicate that a tail-recursion is
    ///requested. If thrown outside of an interpreted function
    ///context, the interpreter will catch the exception and
    ///raise a more informative one.
    class RecursionMarkerException : public std::exception
    {
    public:
        
        using std::exception::exception;
        virtual const char *what() noexcept { return "recursion marker exception"; }
    };
    
#endif /* GFX_Language_SupportsRecursion */
    
    ///The Function abstract class describes the methods necessary
    ///to implement a functor value in the gfx language.
    ///
    ///Concrete subclasses of `gfx::Function` should wrap the contents
    ///of their `apply` method with the following method calls so that
    ///stack traces function correctly:
    ///
    /// stack->interpreter()->enteredFunction(this);
    /// ...
    /// stack->interpreter()->exitedFunction(this);
    ///
    class Function : public Base
    {
    public:
        
        ///Invokes the logic contained within the function, providing
        ///a stack frame to read and write shared state from.
        ///
        /// \param  stack   The frame to read and write shared state from. Will never be null.
        ///
        ///The function should push its return value, if any, onto the passed in frame.
        virtual void apply(StackFrame *stack) const = 0;
    };
    
#pragma mark -
    
    ///The NativeFunction concrete class wraps a native (as in C++) function
    ///so that it can be used as a functor value in the gfx language.
    class NativeFunction : public Function
    {
    public:
        
        ///The type that represents native (C++) functions.
        ///
        /// \param  stack   The frame to read and write shared state from. Will never be null.
        ///
        ///The function should push its return value, if any, onto the passed in frame.
        typedef std::function<void(StackFrame *stack)> Type;
        
    protected:
        
        ///The implementation of the native function.
        NativeFunction::Type mImplementation;
        
        ///The name of the native function.
        const String *mName;
        
    public:
        
        ///Constructs the native function with a name and implementation
        ///
        /// \param  name            The name of the native function. Provided to aid in debugging. Should not be null.
        /// \param  implementation  The logic of the native function.
        ///
        NativeFunction(const String *name, NativeFunction::Type implementation) :
            mName(retained(name)),
            mImplementation(implementation)
        {
        }
        
        ///The destructor.
        ~NativeFunction()
        {
            released(mName);
        }
        
        ///Returns the name of the native function.
        const String *name() const { return retained_autoreleased(mName); }
        
#pragma mark - Overrides
        
        virtual const String *description() const override;
        virtual void apply(StackFrame *stack) const override;
    };
    
#pragma mark -
    
    ///The InterpretedFunction concrete class wraps an instance of `gfx::Expression`
    ///to allow interpreted logic to be used as a functor value in Gfx.
    ///
    ///The expression associated with the interpreted function is evaluated with its
    ///own stack frame wrapping the frame passed into `apply`. It is also applied
    ///with an autorelease pool so that all temporary objects created are scope bound.
    class InterpretedFunction : public Function
    {
        ///The source Expression object.
        GFX_strong Expression *mSource;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Constructs an interpreted function with a given source expression.
        ///
        /// \param  source      The expression to evaluate when applied. Should not be null.
        ///
        explicit InterpretedFunction(Expression *source);
        
        ///The destructor.
        virtual ~InterpretedFunction();
        
#pragma mark - Overrides
        
        virtual void apply(StackFrame *stack) const override;
        virtual const String *description() const override;
    };
}

#endif
