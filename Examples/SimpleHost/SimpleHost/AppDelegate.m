//
//  AppDelegate.m
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "AppDelegate.h"

#import <gfx/gfx.h>
#import <QuartzCore/QuartzCore.h>

@interface AppDelegate () <GFXPortalViewDelegate>

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSColor *backgroundColor = [NSColor colorWithPatternImage:[NSImage imageNamed:@"CheckerBackground"]];
    self.graphicsView.layer.backgroundColor = backgroundColor.CGColor;
}

#pragma mark - Actions

- (IBAction)clear:(id)sender
{
    [self clearConsole];
}

- (IBAction)run:(id)sender
{
    NSString *sourceCode = [self.inputTextView string];
    NSError *error = nil;
    if(![self.graphicsView runString:sourceCode error:&error]) {
        [self writeErrorMessageToConsole:[error localizedDescription]];
    }
}

#pragma mark - <GFXPortalViewDelegate>

- (void)portalViewDidDraw:(GFXPortalView *)sender
{
    [self clearConsole];
    [self writeStatusMessageToConsole:@"Rendered."];
}

- (void)portalView:(GFXPortalView *)sender didEncounterError:(NSError *)error
{
    [self writeErrorMessageToConsole:error.localizedDescription];
}

#pragma mark - Console

- (void)clearConsole
{
    self.consoleTextView.string = @"";
}

- (void)scrollConsoleToBottom
{
    [self.consoleTextView scrollRangeToVisible:NSMakeRange(self.consoleTextView.string.length, 0)];
}

- (void)writeNewlineToConsole
{
    [self.consoleTextView.textStorage appendAttributedString:[[NSAttributedString alloc] initWithString:@"\n"]];
    [self scrollConsoleToBottom];
}

- (void)writeErrorMessageToConsole:(NSString *)errorMessage
{
    NSDictionary *attributes = @{NSForegroundColorAttributeName: [NSColor redColor],
                                 NSFontAttributeName: [NSFont boldSystemFontOfSize:12.0]};
    NSAttributedString *errorString = [[NSAttributedString alloc] initWithString:errorMessage
                                                                      attributes:attributes];
    [self.consoleTextView.textStorage appendAttributedString:errorString];
    [self writeNewlineToConsole];
}

- (void)writeStatusMessageToConsole:(NSString *)statusMessage
{
    NSDictionary *attributes = @{NSForegroundColorAttributeName: [NSColor grayColor],
                                 NSFontAttributeName: [NSFont systemFontOfSize:12.0]};
    NSAttributedString *statusString = [[NSAttributedString alloc] initWithString:statusMessage
                                                                       attributes:attributes];
    [self.consoleTextView.textStorage appendAttributedString:statusString];
    [self writeNewlineToConsole];
}

@end
