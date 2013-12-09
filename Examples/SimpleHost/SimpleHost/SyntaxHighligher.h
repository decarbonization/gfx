//
//  SyntaxHighligher.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SHDefinition;

///The SyntaxHighligher class performs an arbitrary number of regular
///expression matches on a text storage, and adds attributes to ranges
///of text that correspond to the matches.
@interface SyntaxHighligher : NSObject

///Initialize the receiver with an array of SHDefinition objects.
///
/// \param  definitions The definitions to use to highlight text storages.
///
/// \result A fully initialized syntax highlighter.
- (instancetype)initWithDefinitions:(NSArray *)definitions;

#pragma mark - Properties

///The definitions to apply to text storages.
///
/// \seealso(SHDefinition)
@property (copy) NSArray *definitions;

///The attributes to apply to the text storage when there are no
///specialized attributes for the kind of definition that was matched.
@property (copy) NSDictionary *defaultAttributes;

///A dictionary whose keys correspond to SHDefinition names,
///and whose values are dictionaries who contain attributes
///as described by `NSAttributedString+AppKit`.
@property (copy) NSDictionary *attributes;

#pragma mark - Highlighting

///Apply syntax highlighting to a given text storage.
///
/// \param  textStorage The text storage to apply syntax highlighting to. Should not be nil.
///
- (void)highlightTextStorage:(NSTextStorage *)textStorage;

@end
