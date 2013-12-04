//
//  GraphicsDocument.m
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "GraphicsDocument.h"
#import <gfx/gfx.h>
#import <QuartzCore/QuartzCore.h>

#import "SHTextView.h"
#import "SyntaxHighligher.h"
#import "SHDefinition.h"

@interface GraphicsDocument () <GFXPortalViewDelegate>

@property (copy) NSString *lastStringRun;

@property (weak) NSTimer *runDelayTimer;

@end

#pragma mark -

@implementation GraphicsDocument

- (id)init
{
    if((self = [super init])) {
        
    }
    
    return self;
}

- (NSString *)windowNibName
{
    return @"GraphicsDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];
    
    self.inputTextView.backgroundColor = [NSColor colorWithCalibratedRed:0.15 green:0.16 blue:0.18 alpha:1.00];
    self.inputTextView.selectedTextAttributes = @{NSBackgroundColorAttributeName: [NSColor blackColor]};
    self.inputTextView.insertionPointColor = [NSColor whiteColor];
    self.inputTextView.automaticQuoteSubstitutionEnabled = NO;
    self.inputTextView.enabledTextCheckingTypes = kNilOptions;
    self.consoleTextView.backgroundColor = [NSColor colorWithCalibratedRed:0.15 green:0.16 blue:0.18 alpha:1.00];
    
    NSArray *definitions = [SHDefinition definitionsNamed:@"GFX"];
    SyntaxHighligher *highlighter = [[SyntaxHighligher alloc] initWithDefinitions:definitions];
    highlighter.defaultAttributes = @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.82 green:0.82 blue:0.82 alpha:1.00],
                                      NSFontAttributeName: [NSFont userFixedPitchFontOfSize:11.0]};
    highlighter.attributes = @{SHDefinitionNameStrings: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.62 green:0.78 blue:0.45 alpha:1.00]},
                               SHDefinitionNameNumbers: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.93 green:0.54 blue:0.28 alpha:1.00]},
                               SHDefinitionNameColors: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.55 green:0.69 blue:0.79 alpha:1.00]},
                               SHDefinitionNameKeywords: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.78 green:0.63 blue:0.78 alpha:1.00]},
                               SHDefinitionNameCoreFunctions: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.98 green:0.80 blue:0.40 alpha:1.00]},
                               SHDefinitionNameComments: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.65 green:0.66 blue:0.65 alpha:1.00]}};
    self.inputTextView.syntaxHighlighter = highlighter;
    
    self.graphicsView = [[GFXPortalView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 300.0, 300.0)];
    self.graphicsView.delegate = self;
    
    [self.canvasScrollView setDocumentView:self.graphicsView];
    NSColor *backgroundColor = [NSColor colorWithPatternImage:[NSImage imageNamed:@"CheckerBackground"]];
    [self.canvasScrollView setBackgroundColor:backgroundColor];
    
    [self run:nil];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
    return [self.inputTextView.string dataUsingEncoding:NSUTF8StringEncoding];
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
    NSString *sourceCode = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    if(sourceCode != nil) {
        self.inputTextView.string = sourceCode;
        return YES;
    } else {
        return NO;
    }
}

+ (BOOL)autosavesInPlace
{
    return YES;
}

#pragma mark - Actions

- (IBAction)clear:(id)sender
{
    [self clearConsole];
}

- (IBAction)run:(id)sender
{
    NSString *sourceCode = [self.inputTextView string];
    if([sourceCode isEqualToString:self.lastStringRun])
        return;
    
    self.lastStringRun = sourceCode;
    
    NSError *error = nil;
    if(![self.graphicsView runString:sourceCode error:&error]) {
        [self writeErrorMessageToConsole:[error localizedDescription]];
    }
}

#pragma mark - <NSTextViewDelegate>

- (void)textDidChange:(NSNotification *)notification
{
    [self.runDelayTimer invalidate];
    self.runDelayTimer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                          target:self
                                                        selector:@selector(run:)
                                                        userInfo:nil
                                                         repeats:NO];
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
    [self clearConsole];
    NSDictionary *attributes = @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.93 green:0.54 blue:0.28 alpha:1.00],
                                 NSFontAttributeName: [NSFont userFixedPitchFontOfSize:11.0]};
    NSAttributedString *errorString = [[NSAttributedString alloc] initWithString:errorMessage
                                                                      attributes:attributes];
    [self.consoleTextView.textStorage appendAttributedString:errorString];
    [self writeNewlineToConsole];
}

- (void)writeStatusMessageToConsole:(NSString *)statusMessage
{
    NSDictionary *attributes = @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.82 green:0.82 blue:0.82 alpha:1.00],
                                 NSFontAttributeName: [NSFont userFixedPitchFontOfSize:11.0]};
    NSAttributedString *statusString = [[NSAttributedString alloc] initWithString:statusMessage
                                                                       attributes:attributes];
    [self.consoleTextView.textStorage appendAttributedString:statusString];
    [self writeNewlineToConsole];
}

@end
