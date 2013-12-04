//
//  SHTextView.m
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "SHTextView.h"
#import "SyntaxHighligher.h"

@implementation SHTextView

- (void)commonInit
{
    [[self textStorage] setDelegate:self];
}

- (id)initWithFrame:(NSRect)frameRect textContainer:(NSTextContainer *)container
{
    if((self = [super initWithFrame:frameRect textContainer:container])) {
        [self commonInit];
    }
    
    return self;
}

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    [self commonInit];
}

#pragma mark - Properties

- (void)setSyntaxHighlighter:(SyntaxHighligher *)syntaxHighlighter
{
    _syntaxHighlighter = syntaxHighlighter;
    [_syntaxHighlighter highlightTextStorage:[self textStorage]];
}

#pragma mark - <NSTextContainerDelegate>

- (void)textStorageDidProcessEditing:(NSNotification *)notification
{
    [_syntaxHighlighter highlightTextStorage:[self textStorage]];
}

@end
