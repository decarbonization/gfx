//
//  GFXInterpreter.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/6/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

@class GFXValue;

///The GFXInterpreter class encapsulates a barebones Objective-C interface
///around the `gfx::Parser` and `gfx::Interpreter` C++ classes.
///
///__Important:__ `GFXInterpreter` is currently a main thread only class.
///An exception will be raised if it is called from a background thread.
///
/// \seealso(GFXValue)
@interface GFXInterpreter : NSObject

#pragma mark - Evaluation

///Parses the contents of a given string, returning an expression tree wrapped in a `GFXValue`.
///
/// \param  string      The string to parse. Required.
/// \param  outError    On return, contains any errors that occurred during parsing.
///
/// \result A GFXValue containing the expression tree if parsing was successful; nil otherwise.
///
- (GFXValue *)parseString:(NSString *)string error:(NSError **)outError;

///Evaluates an abstract syntax tree returned by `-[self parseString:error:]`.
///
/// \param  value       The abstract syntax tree. Required.
/// \param  outError    On returns, contains any evaluation errors that occurred.
///
/// \result YES if the abstract syntax tree could be evaluated; NO otherwise.
- (BOOL)evaluate:(GFXValue *)value error:(NSError **)outError;

#pragma mark -

///Parse and evaluates the contents of a given string.
///
/// \param  string      The string to parse and evaluate. Required.
/// \param  outError    On return, contains any parsing and evaluation errors that occurred.
///
/// \result YES if the string could be parsed and evaluated; NO otherwise.
- (BOOL)evaluateString:(NSString *)string error:(NSError **)outError;

#pragma mark - Stack Frames

- (BOOL)pushEmptyStackFrame;
- (BOOL)popTopStackFrame;

@end
