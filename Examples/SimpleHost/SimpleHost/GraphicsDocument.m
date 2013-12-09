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

@interface GraphicsDocument () <GFXViewDelegate>

///The last string that was evaluated.
///
///We store this so that we don't perform redundant
///evaluations when the user makes a bunch of changes,
///and then immediately undoes them. A semi edge case
///that can save some time.
@property (copy) NSString *lastStringRun;

///A timer that is scheduled after each key stroke in
///the input text view. If the timer is allowed to finish
///without the user inputting another character, the gfx
///code in the input text view is evaluated and displayed.
@property (weak) NSTimer *runDelayTimer;

@end

#pragma mark -

@implementation GraphicsDocument

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSString *)windowNibName
{
    return @"GraphicsDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];
    
    //This is not particularly elegant. We have to configure the input and output
    ///console text views to behave like code editors and not text editors.
    self.inputTextView.backgroundColor = [NSColor colorWithCalibratedRed:0.15 green:0.16 blue:0.18 alpha:1.00];
    self.inputTextView.selectedTextAttributes = @{NSBackgroundColorAttributeName: [NSColor blackColor]};
    self.inputTextView.insertionPointColor = [NSColor whiteColor];
    self.inputTextView.automaticQuoteSubstitutionEnabled = NO;
    self.inputTextView.enabledTextCheckingTypes = kNilOptions;
    self.consoleTextView.backgroundColor = [NSColor colorWithCalibratedRed:0.15 green:0.16 blue:0.18 alpha:1.00];
    
    
    //If this were a more 'serious' host, these colors and fonts would
    //be user configurable, and wouldn't be loaded on the spot here.
    NSArray *definitions = [SHDefinition definitionsNamed:@"GFX"];
    SyntaxHighligher *highlighter = [[SyntaxHighligher alloc] initWithDefinitions:definitions];
    
    //The default attributes are applied before any syntax specific is.
    //This is how we give the whole document a font and sensible default
    //text color.
    highlighter.defaultAttributes = @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.82 green:0.82 blue:0.82 alpha:1.00],
                                      NSFontAttributeName: [NSFont userFixedPitchFontOfSize:11.0]};
    highlighter.attributes = @{
        //What strings should look like.
        SHDefinitionNameStrings: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.62 green:0.78 blue:0.45 alpha:1.00]},

        //What numbers should look like.
        SHDefinitionNameNumbers: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.93 green:0.54 blue:0.28 alpha:1.00]},

        //What colors (#ffffff, white, etc) should look like.
        SHDefinitionNameColors: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.55 green:0.69 blue:0.79 alpha:1.00]},

        //What keywords should look like.
        SHDefinitionNameKeywords: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.78 green:0.63 blue:0.78 alpha:1.00]},

        //What core functions should look like.
        SHDefinitionNameCoreFunctions: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.98 green:0.80 blue:0.40 alpha:1.00]},

        //What comments should look like
        SHDefinitionNameComments: @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.65 green:0.66 blue:0.65 alpha:1.00]}
    };
    self.inputTextView.syntaxHighlighter = highlighter;
    
    
    //We give the graphics view a sensible default size. The actual size
    //is determined by an annotation at the header of the SimpleHost gfx code.
    self.graphicsView = [[GFXView alloc] initWithFrame:NSMakeRect(0.0, 0.0, 300.0, 300.0)];
    self.graphicsView.delegate = self;
    
    //Annotations are read stream style. We have to respond to them as they become available.
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(interpreterFoundAnnotation:)
                                                 name:GFXInterpreterFoundAnnotationNotification
                                               object:self.graphicsView.interpreter];
    
    //The canvas text view has a special clip view that centers its contents and adds a drop shadow.
    [self.canvasScrollView setDocumentView:self.graphicsView];
    NSColor *backgroundColor = [NSColor colorWithPatternImage:[NSImage imageNamed:@"CheckerBackground"]];
    [self.canvasScrollView setBackgroundColor:backgroundColor];
    
    
    //Give us some sensible defaults.
    NSURL *exampleLocation = [[NSBundle mainBundle] URLForResource:@"Example" withExtension:@"shgfx"];
    self.inputTextView.string = [NSString stringWithContentsOfURL:exampleLocation encoding:NSUTF8StringEncoding error:NULL];
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
    self.graphicsView.code = sourceCode;
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

#pragma mark - Graphics Callbacks

///The gfx view tells us when it has rendered its contents
///using the code the user has provided. Just a simple
///feedback mechanism here.
- (void)graphicsViewDidFinishRendering:(GFXView *)sender
{
    [self writeStatusMessageToConsole:@"Rendered."];
}

///The gfx view tells us when it has an issue interpreting
///the code we've provided, pass that along using our simple
///console.
- (void)graphicsView:(GFXView *)sender didEncounterError:(NSError *)error
{
    [self writeErrorMessageToConsole:error.localizedDescription];
}

#pragma mark -

///The interpreter found an annotation. We look for a simple annotation
///to describe the size of the document. This allows everything to be
///contained within the code.
- (void)interpreterFoundAnnotation:(NSNotification *)notification
{
    NSString *contents = notification.userInfo[GFXAnnotationContentsUserInfoKey];
    
    //Annotations can potentially have leading and trailing whitespace,
    //we strip that out before trying to make sense of the annotation.
    NSString *trimmedContents = [contents stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    
    //We only know about annotations that start with 'size: '.
    //The full form is 'size: 500x500'.
    if([trimmedContents hasPrefix:@"size: "]) {
        //We expect there to be two numbers separated by an 'x' after the 'size: ' prefix.
        NSString *sizeSegment = [trimmedContents substringFromIndex:[@"size: " length]];
        NSArray *sizeParts = [sizeSegment componentsSeparatedByString:@"x"];
        if([sizeParts count] == 2) {
            CGSize size = CGSizeMake([sizeParts.firstObject floatValue], [sizeParts.lastObject floatValue]);
            [self.graphicsView setFrameSize:size];
        } else {
            //Inform the user if they inputted the size of the document incorrectly.
            [self writeErrorMessageToConsole:[NSString stringWithFormat:@"Malformed size %@", trimmedContents]];
        }
    } else {
        //Let the user know we couldn't figure out an annotation.
        [self writeErrorMessageToConsole:[NSString stringWithFormat:@"Unrecognized annotation '%@', ignoring.", contents]];
    }
}

#pragma mark - Console

///Clears the contents of the console.
- (void)clearConsole
{
    self.consoleTextView.string = @"";
}

///Scrolls the console to the bottom of its contents.
- (void)scrollConsoleToBottom
{
    [self.consoleTextView scrollRangeToVisible:NSMakeRange(self.consoleTextView.string.length, 0)];
}

///Writes a new line to the console.
- (void)writeNewlineToConsole
{
    //Just use whatever attributes are at the end of the text storage, it doesn't really matter.
    [self.consoleTextView.textStorage appendAttributedString:[[NSAttributedString alloc] initWithString:@"\n"]];
    [self scrollConsoleToBottom];
}

///Writes an error message to the console, followed by a newline.
- (void)writeErrorMessageToConsole:(NSString *)errorMessage
{
    NSDictionary *attributes = @{NSForegroundColorAttributeName: [NSColor colorWithCalibratedRed:0.93 green:0.54 blue:0.28 alpha:1.00],
                                 NSFontAttributeName: [NSFont userFixedPitchFontOfSize:11.0]};
    NSAttributedString *errorString = [[NSAttributedString alloc] initWithString:errorMessage
                                                                      attributes:attributes];
    [self.consoleTextView.textStorage appendAttributedString:errorString];
    [self writeNewlineToConsole];
}

///Writes a status message to the console, followed by a newline.
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
