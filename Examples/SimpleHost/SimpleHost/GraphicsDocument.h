//
//  GraphicsDocument.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class GFXPortalView, SHTextView;

@interface GraphicsDocument : NSDocument <NSTextViewDelegate>

#pragma mark - Outlets

@property (assign) IBOutlet NSScrollView *canvasScrollView;

#pragma mark -

@property (assign) IBOutlet SHTextView *inputTextView;
@property (assign) IBOutlet NSTextView *consoleTextView;

#pragma mark - Properties

@property GFXPortalView *graphicsView;

#pragma mark - Actions

- (IBAction)clear:(id)sender;
- (IBAction)run:(id)sender;

@end
