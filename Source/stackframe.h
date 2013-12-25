//
//  stackframe.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__stackframe__
#define __gfx__stackframe__

#include "base.h"
#include "array.h"
#include "dictionary.h"
#include "broadcastsignal.h"
#include "exception.h"
#include <tuple>
#include <mutex>

namespace gfx {
    class Interpreter;
    class Number;
    class Function;
    
#pragma mark -
    
    ///The StackFrame class encapsulates a LiFO stack used to pass values
    ///into and out of functions, as well as a map of variable bindings
    ///used for storing and lookup.
    ///
    ///Frames may have parents assigned to them. When a frame has a parent,
    ///two additional behaviors are possible:
    ///
    /// -   Popping from an empty stack will pull from the `parent` stack.
    ///     This enables functions to have isolated stacks, but still pull
    ///     from the call site. See `gfx::InterpretedFunction` for more info.
    /// -   Binding lookup and creation can traverse `parent` frames. This
    ///     enables the Gfx language to have lexical scope. In the future,
    ///     this functionality may be separated into its own class.
    ///
    ///Frames constructed with parent frames will observe their parent's
    ///`gfx::StackFrame::DestroySignal`, and will autorelease themselves.
    ///They will also clear the reference to their parent, as such it is
    ///safe to retain and continue to use the frame.
    ///
    ///StackFrame includes a locking mechanism around its read and write
    ///operations. It is guaranteed to be thread-safe.
    class StackFrame : public Base
    {
        ///The type of the exception raised by `gfx::StackFrame`
        ///when an attmept is made to mutate a frozen frame object.
        class AccessViolationException : public Exception
        {
        public:
            using Exception::Exception;
        };
        
    protected:
        
        ///The mutex that ensures thread-safety across read-write operations.
        mutable std::recursive_mutex mReadWriteMutex;
        
        ///The storage for the LiFO stack of the frame.
        GFX_strong Array<Base> *mStorage;
        
        ///The variable bindings of the frame.
        GFX_strong Dictionary<const String, Base> *mBindings;
        
        ///The parent frame of this frame. Used for pop/
        ///peak operations, and binding lookups/assignments.
        ///
        ///Child frames observe their parent's `DestroySignal`
        ///and autorelease themselves. This prevents dangling
        ///pointers and allows frames to be implicitly destroyed.
        GFX_safe_weak StackFrame *mParent;
        
        ///The interpreter that the stack frame is attached to.
        GFX_weak Interpreter *mInterpreter;
        
        ///Whether or not the frame's contents are frozen.
        bool mIsFrozen;
        
        ///The current destroy signal reference, if any.
        Signal<Nothing>::ObserverReference mDestroySignalReference;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Construct a frame with an optional parent, and a required interpreter.
        ///
        /// \param  parent      The parent of the frame. May be null.
        /// \param  interpreter The interpreter of the frame. Should not be null.
        ///
        ///If a `parent` frame is provided, the newly constructed frame will observe
        ///its `gfx::StackFrame::DestroySignal`, and will autorelease itself whenever
        ///this signal fires. It will also clear its reference ot the parent frame,
        ///so it is safe to retain the frame and continue to use it.
        explicit StackFrame(StackFrame *parent, Interpreter *interpreter);
        
        ///The destructor. Fires `gfx::StackFrame::DestroySignal`.
        virtual ~StackFrame();
        
        
        ///A signal sent when `gfx::StackFrame::~StackFrame` is invoked
        ///to inform observers that a frame is no longer valid.
        Signal<Nothing> DestroySignal;
        
#pragma mark - Stack Methods
        
        ///Push a new value onto the stack.
        ///
        /// \param  value   The value to push on to the stack. May not be null.
        ///
        void push(Base *value);
        
        ///Pops a value from the stack, and returns it.
        ///
        /// \throws Exception when the stack is empty.
        Base *pop();
        
        ///Pops a value from the stack and verifies that it is a number.
        ///If the type check passes, the value is returned, otherwise an
        ///exception is raised.
        ///
        /// \throws Exception when the stack is empty, or there is a type check error.
        Number *popNumber();
        
        ///Pops a value from the stack and verifies that it is a string.
        ///If the type check passes, the value is returned, otherwise an
        ///exception is raised.
        ///
        /// \throws Exception when the stack is empty, or there is a type check error.
        String *popString();
        
        ///Pops a value from the stack and verifies that it is a function.
        ///If the type check passes, the value is returned, otherwise an
        ///exception is raised.
        ///
        /// \throws Exception when the stack is empty, or there is a type check error.
        Function *popFunction();
        
        ///Pops a value from the stack and verifies that it is of type `T`.
        ///If the type check passes, the value is returned, otherwise an
        ///exception is raised.
        ///
        /// \tparam T   A Base-derived type.
        ///
        /// \throws Exception when the stack is empty, or there is a type check error.
        template<typename T> T *popType()
        {
            Base *value = pop();
            if(!value->isKindOfClass<T>())
                throw Exception((String::Builder() << "wrong type on stack. got '" << value->className() << "'."), nullptr);
            
            return (T *)value;
        }
        
#pragma mark -
        
        ///Drops all of the values on the stack.
        ///
        ///Safe to call on an empty stack.
        void dropAll();
        
        ///Drops a single value from the stack.
        ///
        ///This method does nothing if the stack is empty.
        void safeDrop();
        
#pragma mark -
        
        ///Returns the top most value on the stack, or the top most value of the
        ///nearest parent frame that has a value. Raises if no value can be found.
        ///
        /// \throws Exception
        Base *peak() const;
        
        ///Returns the number of items currently on the stack.
        size_t depth() const;
        
        ///Returns a bool indicating whether or not the stack is empty.
        bool empty() const;
        
        ///Iterates each value on the stack, from first to last in.
        ///
        /// \param  function    A function to invoke for each item on the stack.
        ///
        void iterate(std::function<void(Base *value, Index index, bool *stop)> function) const;
        
#pragma mark - Parent
        
        ///Returns the parent of the frame.
        ///
        ///Although it's not possible to mutate this variable, it
        ///will automatically be cleared if the parent is destroyed.
        StackFrame *parent() const;
        
        ///Returns the interpreter of the frame.
        ///
        ///This is a weak reference. If an interpreter
        ///is released, it is the responsibility of that
        ///releaser to ensure all frames are destroyed too.
        Interpreter *interpreter() const;
        
#pragma mark - Bindings
        
        ///Sets the `value` for a binding with a given `key`.
        ///
        /// \param  key                 The name of the binding. Required.
        /// \param  value               The value of the binding. May be null.
        /// \param  searchParentScopes  Whether or not the parent scopes should be searched
        ///                             to find any existing binding with the name `key`, and
        ///                             to replace the binding's value if found. Default is `true`.
        ///
        void setBindingToValue(const String *key, Base *value, bool searchParentScopes = true);
        
        ///Returns the value, if any, for for the binding by the name `key`.
        ///
        /// \param  key                 The name of the binding. Required.
        /// \param  searchParentScopes  Whether or not parent scopes should be searched
        ///                             if the receiver does not contain a value by the
        ///                             name `key`. Default is `true`.
        ///
        /// \result The value for `key` or null if none exists.
        ///
        Base *bindingValue(const String *key, bool searchParentScopes = true) const;
        
#pragma mark -
        
        ///Convenience function that creates a binding/variable with a given name in the receiver.
        ///
        /// \param  name    The name of the variable/binding. Required.
        /// \param  value   The value.
        ///
        void createVariableBinding(const String *name, Base *value);
        
        ///Convenience function that creates a new function binding with a given
        ///name and native function implementation within the receiver.
        ///
        /// \param  name            The name of the binding. Required.
        /// \param  implementation  The native implementation of the function. Required.
        ///
        void createFunctionBinding(const String *name, std::function<void(StackFrame *stack)> implementation);
        
#pragma mark - Freezing
        
        ///Freezes the contents of the frame.
        ///
        ///Frozen frames will raise if there are
        ///any attempts to mutate their contents.
        void freeze();
        
        ///Unfreezes the contents of the frame.
        void unfreeze();
        
        ///Returns whether or not the contents of the frame are frozen.
        bool isFrozen() const;
        
    protected:
        
        ///Raises an exception if the contents of the frame are currently frozen.
        ///
        /// \param  message The message to associate with the exception. Required.
        ///
        void assertMutationPossible(const String *message = str("Attempting to mutate a frame that has been frozen."), const String *affectedBindingKey = nullptr) const;
    };
}

#endif /* defined(__gfx__stackframe__) */
