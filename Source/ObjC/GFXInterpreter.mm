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
#include "annotation.h"

#import "GFXValue.h"
#import "GFXHelpers.h"

NSString *const GFXAnnotationContentsUserInfoKey = @"GFXAnnotationContentsUserInfoKey";
NSString *const GFXInterpreterFoundAnnotationNotification = @"GFXInterpreterFoundAnnotationNotification";

@interface GFXInterpreter () {
    ///The underlying interpreter of the ObjC wrapper.
    GFX_strong gfx::Interpreter *_interpreter;
    
    gfx::Signal<const gfx::Annotation *>::ObserverReference _annotationObserver;
}

@end

#pragma mark -

@implementation GFXInterpreter

#pragma mark - Lifecycle

- (void)dealloc
{
    _interpreter->AnnotationFoundSignal.remove(_annotationObserver);
    
    gfx::released(_interpreter);
    _interpreter = nullptr;
    
    [super dealloc];
}

- (id)init
{
    if((self = [super init])) {
        _interpreter = new gfx::Interpreter();
        _annotationObserver = _interpreter->AnnotationFoundSignal.add([self](const gfx::Annotation *annotation) {
            [[NSNotificationCenter defaultCenter] postNotificationName:GFXInterpreterFoundAnnotationNotification
                                                                object:self
                                                              userInfo:@{GFXAnnotationContentsUserInfoKey: NSStringFromGFXString(annotation->contents())}];
        });
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

- (BOOL)evaluate:(GFXValue *)value withStackFrame:(GFXValue *)stackFrame error:(NSError **)outError
{
    NSParameterAssert(value);
    NSParameterAssert(stackFrame);
    
    try {
        auto expressions = gfx::lift_value<gfx::Array<gfx::Base>>(value);
        _interpreter->eval(gfx::lift_value<gfx::StackFrame>(stackFrame), expressions);
        
        return YES;
    } catch (gfx::Exception e) {
        if(outError) *outError = NSErrorFromGFXException(GFXErrorInterpretationDidFail, e);
        
        return NO;
    }
}

#pragma mark -

- (BOOL)evaluateString:(NSString *)string withStackFrame:(GFXValue *)stackFrame error:(NSError **)outError
{
    NSParameterAssert(string);
    NSParameterAssert(stackFrame);
    
    GFXValue *expressions = [self parseString:string error:outError];
    if(!expressions)
        return NO;
    
    return [self evaluate:expressions withStackFrame:stackFrame error:outError];
}

#pragma mark - Stack Frames

- (GFXValue *)rootStackFrame
{
    return [GFXValue valueWithObject:_interpreter->rootFrame() takeOwnership:NO];
}

- (GFXValue *)emptyStackFrameWithParentFrame:(GFXValue *)parentFrame
{
    auto unboxedParentFrame = parentFrame? gfx::lift_value<gfx::StackFrame>(parentFrame) : nullptr;
    return [GFXValue valueWithObject:(new gfx::StackFrame(unboxedParentFrame, _interpreter)) takeOwnership:YES];
}

@end
