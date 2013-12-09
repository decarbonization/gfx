//
//  SHTextView.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class SyntaxHighligher;

///The SHTextView class is a simple (and not very robust) subclass
///of NSTextView that adds support for syntax highlighting of code.
///
///An instance has a syntax highlighter object that analyzes the
///text storage of the text view, and applies coloring and other
///attributes to its contents.
@interface SHTextView : NSTextView <NSTextStorageDelegate>

#pragma mark - Properties

///The syntax highlighter of the text view.
@property (nonatomic) SyntaxHighligher *syntaxHighlighter;

@end
