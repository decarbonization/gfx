//
//  AppDelegate.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class GFXPortalView;

@interface AppDelegate : NSObject <NSApplicationDelegate>

#pragma mark - Outlets

@property (assign) IBOutlet NSWindow *canvasWindow;
@property (assign) IBOutlet GFXPortalView *graphicsView;

#pragma mark -

@property (assign) IBOutlet NSWindow *inputWindow;
@property (assign) IBOutlet NSTextView *inputTextView;
@property (assign) IBOutlet NSTextView *consoleTextView;

#pragma mark - Actions

- (IBAction)clear:(id)sender;
- (IBAction)run:(id)sender;

@end
