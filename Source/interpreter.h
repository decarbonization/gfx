//
//  interpreter.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__interpreter__
#define __gfx__interpreter__

#include "base.h"
#include "array.h"
#include "offset.h"
#include "broadcastsignal.h"

#include <stack>
#include <list>

namespace gfx {
    class StackFrame;
    class Function;
    class Word;
    class Annotation;
    
    ///The Interpreter class encapsulates evaluation of already-parsed GFX code
    ///from the `gfx::Parser` class, as well as management of shared global state.
    class Interpreter : public Base
    {
    public:
        
        typedef std::function<bool(StackFrame *currentFrame, Word *word)> WordHandler;
        typedef std::list<WordHandler> WordHandlerList;
        
    protected:
        
        ///The root frame of the interpreter. This contains the core and graphics stack functions.
        GFX_weak StackFrame *mRootFrame;
        
        ///The stack frames currently in use.
        std::stack<StackFrame *> mFrames;
        
        ///The currently executing function stack trace.
        GFX_strong Array<const Function> *mRunningFunctions;
        
        ///The search paths used by the import method.
        GFX_strong Array<const String> *mSearchPaths;
        
        ///Whether or not import is disabled.
        bool mImportAllowed;
        
        ///The functors to invoke to handle words.
        WordHandlerList mWordHandlers;
        
        
        ///Raises an exception with a given reason, originating from a specified source offset.
        ///
        /// \param  reason  The reason for the failure. Should not be null.
        /// \param  source  The source code offset of the failure.
        ///
        void fail(const String *reason, Offset source);
        
    public:
        ///Returns a bool indicating whether or not a given value is truthy.
        ///
        /// \param  value   The value to check for truthiness.
        ///
        /// \result true if the value has truthiness; false otherwise.
        ///
        static bool IsTrue(Base *value);
        
        
        ///Constructs an interpreter.
        explicit Interpreter();
        
        ///The destructor.
        virtual ~Interpreter();
        
#pragma mark - Interpretation
        
        ///The context from which an expression is being evaluated.
        enum class EvalContext {
            ///The expression is being evaluated from the root of a document.
            Normal,
            
            ///The expression is being evaluated within a vector expression.
            Vector,
            
            ///The expression is being evaluated within a function expression.
            Function,
        };
        
    protected:
        
        ///Evaluates the a given expression object in a given context.
        ///
        /// \param  expression  The expression to evaluate. May not be null.
        /// \param  context     The context the expression is to be evaluated in.
        ///
        ///The result of evaluating the expression may be retrieved through
        ///the interpreter's current stack frame's top most value.
        ///
        /// \seealso(gfx::Interpreter::lastValue)
        void evalExpression(Base *expression, EvalContext context);
        
    public:
        
        ///Signals that an annotation was encountered when evaluating an expression tree.
        ///
        ///Note that annotations found outside of `EvalContext::Normal` will be ignored.
        Signal<const Annotation *> AnnotationFoundSignal;
        
        ///Evaluates a given array of expressions in a given context.
        ///
        /// \param  expressions The expressions to evaluate. May not be null.
        /// \param  context     The context to evaluate the expressions in. Default
        ///                     value is `gfx::Interpreter::EvalContext::Normal`.
        ///
        ///The result of evaluating the expressions may be retrieved through
        ///the interpreter's current stack frame's top most value.
        ///
        /// \seealso(gfx::Interpreter::lastValue)
        void eval(const Array<Base> *expressions, EvalContext context = EvalContext::Normal);
        
#pragma mark - Resetting
        
        ///Destroys all state associated with the interpreter,
        ///effectively rendering it as though it were just
        ///constructed. This method does not destroy any non-
        ///code related state. E.g. import allowed status,
        ///search paths will remain.
        ///
        /// \seealso(gfx::Interpreter::ResetSignal)
        void reset();
        
        ///A signal sent whenever an interpreter is told to reset.
        Signal<Interpreter *> ResetSignal;
        
#pragma mark - Word Handling
        
    protected:
        
        ///Applies a given word to receiver's word handlers, stopping
        ///when it encounters a handler that returns `true`.
        ///
        /// \param  currentFrame    The frame to evaluate any side-effects within. Required.
        /// \param  word            The word to attempt to find a value for. Required.
        ///
        /// \result true if the word was handled; false otherwise.
        ///
        ///It is assumed that the caller will raise an exception or otherwise
        ///report an error if this method returns false.
        bool handleWord(StackFrame *currentFrame, Word *word);
        
    public:
        
        ///Places a given word handler at the beginning of the word handler list.
        ///
        /// \param  wordHandler The word handler to place at the beginning of the handler list.
        ///
        ///The given word handler will be invoked before all others, unless this
        ///method is invoked again with a different word handler.
        void prependWordHandler(const WordHandler &wordHandler);
        
        ///Places a given word handler at the end of the word handler list.
        ///
        /// \param  wordHandler The word handler to place at the end of the handler list.
        ///
        ///The given handler will be invoked after all others, unless this
        ///method is invoked again with a different word handler.
        ///
        ///If the last word handler does not return a non-null value,
        ///the interpreter will raise an exception through the
        ///`gfx::Interpreter::failForUnboundWord` method.
        void appendWordHandler(const WordHandler &wordHandler);
        
        ///Raises an exception to indicate that a given word has
        ///no known word available for it. The default unbound
        ///word handler simply invokes this method.
        ///
        /// \param  word    The word with no known value. Should not be null.
        ///
        /// \throws `gfx::Exception`
        ///
        void failForUnboundWord(const Word *word);
        
#pragma mark - Stack Frames
        
        ///Pushes a frame onto the interpreter's frame stack. This
        ///frame will be the target for all in-place stack operations.
        void pushFrame(StackFrame *frame);
        
        ///Pops the top most frame from the interpreter's frame stack.
        ///
        /// \throws `gfx::Exception` if there are no frames on the stack.
        void popFrame();
        
        ///Returns the top most frame on the interpreter's frame stack, if any.
        StackFrame *currentFrame() const;
        
        ///Returns the top most value of the interpreter's current frame, if any.
        Base *lastValue() const;
        
#pragma mark - Backtrace Tracking
        
        ///Informs the interpreter that a given function has been entered.
        ///
        ///The interpreter updates its backtrace information as a
        ///side-effect of this method being invoked.
        void enteredFunction(const Function *function);
        
        ///Informs the interpreter that a given function has returned.
        ///
        ///The interpreter updates its backtrace information as a
        ///side-effect of this method being invoked.
        void exitedFunction(const Function *function);
        
        ///Returns the current function backtrace.
        const String *backtrace() const;
        
#pragma mark - Files & Import Support
        
        ///Sets whether or not imports are allowed.
        void setImportAllowed(bool importAllowed);
        
        ///Returns whether or not imports are allowed.
        bool isImportAllowed() const;
        
#pragma mark -
        
        ///Returns the known search paths of the interpreter.
        ///
        ///Search paths are generally stored as least to most specific.
        const Array<const String> *searchPaths() const;
        
        ///Adds a given search path to the interpreter.
        void addSearchPath(const String *searchPath);
        
        ///Removes a givne search path from the interpreter.
        void removeSearchPath(const String *searchPath);
        
#pragma mark -
        
        ///Attempts to import the contents of a gfx file
        ///known by a given name into the interpreter.
        ///
        /// \param  filename    The name of the gfx file. Required.
        ///
        /// \result true if the file could be found and imported; false otherwise.
        bool import(const String *filename);
    };
    
    template<typename NewT, typename OldT>
    NewT dynamic_cast_or_throw(OldT value)
    {
        NewT newValue = dynamic_cast<NewT>(value);
        if(!newValue)
            throw Exception(str("type mismatch"), nullptr);
        
        return newValue;
    }
}

#endif /* defined(__gfx__interpreter__) */
