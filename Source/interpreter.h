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
#include <stack>

namespace gfx {
    class StackFrame;
    class Function;
    class Word;
    
    class Interpreter : public Base
    {
    public:
        
        typedef std::function<Base *(Word *word)> UnboundWordHandler;
        
    protected:
        
        /*weak*/ StackFrame *mRootFrame;
        std::stack<StackFrame *> mFrames;
        Array<const Function> *mRunningFunctions;
        Array<const String> *mSearchPaths;
        bool mImportAllowed;
        UnboundWordHandler mUnboundWordHandler;
        
        
        void fail(const String *reason, Offset source);
        
    public:
        static bool IsTrue(Base *value);
        
        explicit Interpreter();
        virtual ~Interpreter();
        
#pragma mark - Interpretation
        
        enum class EvalContext
        {
            Normal,
            Vector,
            Function,
        };
        
        void evalExpression(Base *expression, EvalContext context);
        void eval(const Array<Base> *expressions, EvalContext context = EvalContext::Normal);
        
#pragma mark - Unbound Word Handling
        
        void setUnboundWordHandler(UnboundWordHandler handler);
        UnboundWordHandler unboundWordHandler() const;
        void failForUnboundWord(const Word *word);
        
#pragma mark - Stack Frames
        
        void pushFrame(StackFrame *frame);
        void popFrame();
        StackFrame *currentFrame() const;
        Base *lastValue() const;
        
#pragma mark - Backtrace Tracking
        
        void enteredFunction(const Function *function);
        void exitedFunction(const Function *function);
        const String *backtrace() const;
        
#pragma mark - Files & Import Support
        
        void setFileName(const String *filename);
        const String *fileName() const;
        
#pragma mark -
        
        void setImportAllowed(bool importAllowed);
        bool isImportAllowed() const;
        
#pragma mark -
        
        const Array<const String> *searchPaths() const;
        void addSearchPath(const String *searchPath);
        void removeSearchPath(const String *searchPath);
        
#pragma mark -
        
        bool import(const String *filename);
    };
    
    template<typename NewT, typename OldT>
    NewT dynamic_cast_or_throw(OldT value)
    {
        NewT newValue = dynamic_cast<NewT>(value);
        if(!newValue)
            throw Exception("type mismatch"_gfx, nullptr);
        
        return newValue;
    }
}

#endif /* defined(__gfx__interpreter__) */
