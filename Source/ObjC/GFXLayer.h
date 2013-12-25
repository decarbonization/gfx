//
//  GFXLayer.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/8/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

@class GFXInterpreter;
@protocol GFXRenderLayerDelegate;

///The GFXLayer class is a layer that runs gfx code to populate its contents.
///
///The layer itself does not contain the rendered contents of the gfx code,
///but rather a special child layer does. The child layer is maintained by
///the internal GFX stack, and should be considered an implementation detail.
///
///__Important:__ `-[self layer]` and `-[self init]` are unavailable. Use the
///subclass-provided variants that take an interpreter parameter.
///The `GFXLayer` class is only available if `GFX_Layer_Use_CA` is `1`.
@interface GFXLayer : CALayer

#pragma mark - Lifecycle

///Returns a new autoreleased layer with a given interpreter.
///
/// \param  interpreter The interpreter that will run the code assigned to the layer. Must not be nil.
///
/// \result A new autoreleased GFXLayer ready for use.
+ (instancetype)layerWithInterpreter:(GFXInterpreter *)interpreter;

///Initialize the layer with an interpreter to run the gfx source code assigned to it.
///
/// \param  interpreter The interpreter that will run the code. Must not be nil.
///
/// \result A fully initialized GFXLayer instance.
///
///This is the designated initializer.
- (instancetype)initWithInterpreter:(GFXInterpreter *)interpreter;

#pragma mark -

///Unavailable. Use `+[self layerWithInterpreter:]`.
+ (id)layer UNAVAILABLE_ATTRIBUTE;

///Unavailable. Use `+[self initWithInterpreter:]`.
- (id)init UNAVAILABLE_ATTRIBUTE;

#pragma mark - Properties

///The interpreter that evaluates the code assigned to the layer.
@property (nonatomic, readonly, strong) GFXInterpreter *interpreter;

///The code to run to populate the contents of the layer.
///
///When this property is set, the contents of the string are
///immediately compiled into an abstract syntax tree. If there
///are any syntax errors in the string, this property will
///become nil and the delegate will be informed of the issue.
@property (nonatomic, copy) NSString *code;

///The delegate of the layer.
@property (assign) id <GFXRenderLayerDelegate> renderDelegate;

@end

#pragma mark -

///The GFXRenderLayerDelegate protocol describes the signals that
///GFXLayer produces during the course of rendering.
@protocol GFXRenderLayerDelegate <NSObject>

///The specified layer has finished rendering.
///
/// \param  layer   The layer that has finished rendering.
///
- (void)layerDidFinishRendering:(GFXLayer *)layer;

///The specified layer encountered an error at some point in the parsing-interpretation-rendering pipeline.
///
/// \param  layer   The layer that encountered an issue.
/// \param  error   The error that occurred.
///
- (void)layer:(GFXLayer *)layer didEncounterError:(NSError *)error;

@end
