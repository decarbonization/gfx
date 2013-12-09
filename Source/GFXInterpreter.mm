//
//  GFXInterpreter.m
//  gfx
//
//  Created by Kevin MacWhinnie on 12/6/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "GFXInterpreter.h"

#include "str.h"
#include "parser.h"
#include "interpreter.h"
#include "stackframe.h"

#import "GFXValue.h"
#import "GFXHelpers.h"

@interface GFXInterpreter () {
    ///The underlying interpreter of the ObjC wrapper.
    GFX_strong gfx::Interpreter *_interpreter;
}

@end

#pragma mark -

@implementation GFXInterpreter

#pragma mark - Lifecycle

- (void)dealloc
{
    gfx::released(_interpreter);
    _interpreter = nullptr;
    
    [super dealloc];
}

- (id)init
{
    if((self = [super init])) {
        _interpreter = new gfx::Interpreter();
    }
    
    return self;
}

#pragma mark - Identity

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@:%p '%@'>",
            NSStringFromClass(self.class), self,
            NSStringFromGFXString(_interpreter->description())];
}

#pragma mark - Evaluation

///Checks if the current thread is the main thread,
///raises a NSObjectNotAvailableException if not.
- (void)requireMainThread
{
    if(![NSThread isMainThread])
        [NSException raise:NSObjectNotAvailableException
                    format:@"GFXInterpreter is a main thread only class. %@ accessed from non main thread", self];
}

#pragma mark -

- (GFXValue *)parseString:(NSString *)string error:(NSError **)outError
{
    NSParameterAssert(string);
    
    try {
        auto result = gfx::Parser(NSStringToGFXString(string)).parse();
        return [GFXValue valueWithObject:gfx::retained(result) takeOwnership:YES];
    } catch (gfx::Exception e) {
        if(outError) *outError = NSErrorFromGFXException(GFXErrorParsingDidFail, e);
        
        return nil;
    }
}

- (BOOL)evaluate:(GFXValue *)value error:(NSError **)outError
{
    NSParameterAssert(value);
    
    [self requireMainThread];
    
    try {
        auto expressions = gfx::lift_value<const gfx::Array<gfx::Base>>(value);
        _interpreter->eval(expressions);
        
        return YES;
    } catch (gfx::Exception e) {
        if(outError) *outError = NSErrorFromGFXException(GFXErrorInterpretationDidFail, e);
        
        return NO;
    }
}

#pragma mark -

- (BOOL)evaluateString:(NSString *)string error:(NSError **)outError
{
    NSParameterAssert(string);
    
    GFXValue *expressions = [self parseString:string error:outError];
    if(!expressions)
        return NO;
    
    return [self evaluate:expressions error:outError];
}

#pragma mark - Stack Frames

- (BOOL)pushEmptyStackFrame
{
    try {
        auto newFrame = gfx::make<gfx::StackFrame>(_interpreter->currentFrame(), _interpreter);
        _interpreter->pushFrame(newFrame);
        
        return YES;
    } catch (gfx::Exception e) {
        return NO;
    }
}

- (BOOL)popTopStackFrame
{
    try {
        _interpreter->popFrame();
        
        return YES;
    } catch (gfx::Exception e) {
        return NO;
    }
}

@end
