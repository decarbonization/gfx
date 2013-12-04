//
//  SyntaxHighligher.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SHDefinition;

@interface SyntaxHighligher : NSObject

- (instancetype)initWithDefinitions:(NSArray *)definitions;

#pragma mark - Properties

@property (copy) NSArray *definitions;
@property (copy) NSDictionary *defaultAttributes;
@property (copy) NSDictionary *attributes;

#pragma mark - Highlighting

- (void)highlightTextStorage:(NSTextStorage *)textStorage;

@end
