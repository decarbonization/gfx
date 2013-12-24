//
//  GFXPortalView.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if TARGET_OS_IPHONE
#   import <UIKit/UIKit.h>
#else
#   import <Cocoa/Cocoa.h>
#endif /* TARGET_OS_IPHONE */

@protocol GFXViewDelegate;
@class GFXLayer, GFXInterpreter;

///The GFXView class encapsulates a GFXLayer and GFXInterpreter
///and provides a very simple host to run gfx code within.
///
///On OS X, the `layer` of the view is an instance of `GFXLayer`.
///Changing the value of the layer property will break the view.
///On iOS, the GFXLayer is a child-layer of the view's root layer.
///On both platforms, it is not recommended that subviews be added
///to GFXView. Doing so will lead to undefined behavior.
#if TARGET_OS_IPHONE
@interface GFXView : UIView
#else
@interface GFXView : NSView
#endif /* TARGET_OS_IPHONE */

///Initialize the receiver with a frame and interpreter.
///
/// \param  frameRect   The frame to use for the view.
/// \param  interpreter The interpreter to evaluate gfx code with. Must not be nil.
///
/// \result A fully initialized GFXView.
///
///This is the designated initializer.
- (instancetype)initWithFrame:(CGRect)frameRect interpreter:(GFXInterpreter *)interpreter;

///Initialize the receiver with a frame and create a new interpreter.
- (instancetype)initWithFrame:(CGRect)frameRect;

#pragma mark - Properties

///The GFXLayer that is providing the contents of the view.
@property (nonatomic, readonly) GFXLayer *graphicsLayer;

#pragma mark -

///The delegate of the view.
@property (nonatomic, assign) IBOutlet id <GFXViewDelegate> delegate;

///The interpreter of the view.
@property (nonatomic, readonly) GFXInterpreter *interpreter;

#pragma mark -

///The code to run to populate the contents of the view.
///
///When this property is set, the contents of the string are
///immediately compiled into an abstract syntax tree. If there
///are any syntax errors in the string, this property will
///become nil and the delegate will be informed of the issue.
@property (nonatomic, copy) NSString *code;

@end

#pragma mark -

///The methods a delegate of GFXView must implement.
@protocol GFXViewDelegate <NSObject>

///The specified view has finished rendering.
///
/// \param  sender  The view that has finished rendering.
///
- (void)graphicsViewDidFinishRendering:(GFXView *)sender;

///The specified view encountered an error at some point in the parsing-interpretation-rendering pipeline.
///
/// \param  sender  The view that encountered an issue.
/// \param  error   The error that occurred.
///
- (void)graphicsView:(GFXView *)sender didEncounterError:(NSError *)error;

@end
