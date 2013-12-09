//
//  GraphicsDocument.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class GFXView, SHTextView;

///The GraphicsDocument class encapsulate an editor for editing and viewing SimpleHost-style
///gfx code. A document has a canvas, code editor, and run-log console. The code is run as
///the user types after a predetermined amount of time elapses after their last key stroke.
///
///This class demonstrates how to use the GFXView class to create a Mac host for the gfx language.
@interface GraphicsDocument : NSDocument <NSTextViewDelegate>

#pragma mark - Outlets

///The centering scroll view that hosts the GFXView, aka the canvas.
@property (assign) IBOutlet NSScrollView *canvasScrollView;

#pragma mark -

///The syntax highlighted text view that takes gfx code as input.
@property (assign) IBOutlet SHTextView *inputTextView;

///The color-coded text view that displays evaluation output for the gfx code inputted.
@property (assign) IBOutlet NSTextView *consoleTextView;

#pragma mark - Properties

///The view that parses and runs the gfx code inputted into the document.
@property GFXView *graphicsView;

#pragma mark - Actions

///Clears the contents of the output console text view.
- (IBAction)clear:(id)sender;

///Synchronously parse and evaluate the contents of the input text view.
- (IBAction)run:(id)sender;

@end
