//
//  function.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/16/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "function.h"
#include "interpreter.h"
#include "expression.h"
#include "stackframe.h"

namespace gfx {
    ///A simple type that evaluates a specified function upon destruction
    ///to guarantee certain logic always runs, regardless of exceptions.
    class at_end
    {
        std::function<void()> mAction;
        
    public:
        
        at_end(const std::function<void()> &action) :
            mAction(action)
        {
        }
        
        ~at_end()
        {
            mAction();
        }
    };
    
    void NativeFunction::apply(StackFrame *stack) const
    {
        stack->interpreter()->enteredFunction(this);
        at_end finally([stack, this] {
            stack->interpreter()->exitedFunction(this);
        });
        
        mImplementation(stack);
    }
    
    const String *NativeFunction::description() const
    {
        return (String::Builder() << "&'" << mName << "'");
    }
    
#pragma mark -
    
    InterpretedFunction::InterpretedFunction(Expression *source) :
        mSource(retained(source))
    {
    }
    
    InterpretedFunction::~InterpretedFunction()
    {
        released(mSource);
    }
    
    void InterpretedFunction::apply(StackFrame *stack) const
    {
        AutoreleasePool pool;
        
        Interpreter *interpreter = stack->interpreter();
        interpreter->enteredFunction(this);
        
        StackFrame *functionFrame = make<StackFrame>(stack, interpreter);
        interpreter->pushFrame(functionFrame);
        at_end finally([stack, this, interpreter, functionFrame] {
            interpreter->popFrame();
            interpreter->exitedFunction(this);
            
            if(!functionFrame->empty())
                stack->push(functionFrame->pop());
        });
        
        interpreter->eval(mSource->subexpressions(), Interpreter::EvalContext::Function);
    }
    
    const String *InterpretedFunction::description() const
    {
        return mSource->description();
    }
}
