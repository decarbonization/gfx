//
//  AppDelegate.m
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "AppDelegate.h"

#import <gfx/gfx.h>

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

#pragma mark - Actions

- (IBAction)run:(id)sender
{
    NSString *sourceCode = [self.inputTextView string];
    NSError *error = nil;
    if(![self.graphicsView runString:sourceCode error:&error]) {
        
    }
}

@end
