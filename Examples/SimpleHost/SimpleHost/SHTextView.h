//
//  SHTextView.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class SyntaxHighligher;

@interface SHTextView : NSTextView <NSTextStorageDelegate>

#pragma mark - Properties

@property (nonatomic) SyntaxHighligher *syntaxHighlighter;

@end
