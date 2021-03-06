//
//  GFXLayer.m
//  gfx
//
//  Created by Kevin MacWhinnie on 12/8/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Layer_Use_CA

#import "GFXLayer.h"
#import "GFXInterpreter.h"
#import "GFXHelpers.h"

#include "layer.h"
#include "color.h"
#include "path.h"
#include "context.h"

@interface GFXLayer () {
    ///The underlying gfx::Layer of the wrapper.
    gfx::Layer *_gfxLayer;
}

///The result of parsing the layer's code string.
@property (strong) GFXValue *parsedCode;

#pragma mark - readwrite

@property (readwrite, strong) GFXInterpreter *interpreter;

@end

#pragma mark -

@implementation GFXLayer

#pragma mark - Lifecycle

- (void)dealloc
{
    if(_gfxLayer) {
        _gfxLayer->setDrawFunctor(nullptr);
        released(_gfxLayer);
        _gfxLayer = nullptr;
    }
    
    [_interpreter release];
    _interpreter = nil;
    
    [_parsedCode release];
    _parsedCode = nil;
    
    [_code release];
    _code = nil;
    
    [super dealloc];
}

+ (instancetype)layerWithInterpreter:(GFXInterpreter *)interpreter
{
    return [[[self alloc] initWithInterpreter:interpreter] autorelease];
}

- (instancetype)initWithInterpreter:(GFXInterpreter *)interpreter
{
    NSParameterAssert(interpreter);
    
    if((self = [super init])) {
        self.interpreter = interpreter;
        
        auto drawFunctor = [self](gfx::Layer *layer, gfx::Rect rect) {
            [self drawGraphicsLayer:layer inRect:rect];
        };
        auto exceptionHandler = [self](const gfx::Exception &e) {
            [self handleRenderTimeError:NSErrorFromGFXException(GFXErrorRenderingDidFail, e)];
            return true;
        };
        _gfxLayer = new gfx::Layer(self.frame, drawFunctor, self.contentsScale);
        _gfxLayer->setDrawExceptionHandler(exceptionHandler);
        [self addSublayer:_gfxLayer->CALayer()];
        
        _wantsEmptyPlaceholder = YES;
    }
    
    return self;
}

#pragma mark -

+ (id)layer
{
    [self doesNotRecognizeSelector:_cmd];
    return nil;
}

- (id)init
{
    [self doesNotRecognizeSelector:_cmd];
    return nil;
}

#pragma mark - Callbacks

- (void)drawGraphicsLayer:(gfx::Layer *)layer inRect:(gfx::Rect)rect
{
    if(_parsedCode) {
        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
            [self.renderDelegate layerDidBeginRendering:self];
        }];
        
        NSError *error = nil;
        GFXValue *stackFrame = [_interpreter emptyStackFrameWithParentFrame:[_interpreter rootStackFrame]];
        BOOL success = [_interpreter evaluate:_parsedCode withStackFrame:stackFrame error:&error];
        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
            if(success)
                [self.renderDelegate layerDidFinishRendering:self];
            else
                [self handleRenderTimeError:error];
        }];
    } else if(self.wantsEmptyPlaceholder) {
        gfx::Color::white()->set();
        gfx::Path::fillRect(rect);
    }
}

- (void)handleRenderTimeError:(NSError *)error
{
    self.parsedCode = nil;
    
    [self.renderDelegate layer:self didEncounterError:error];
}

#pragma mark - Properties

- (void)setCode:(NSString *)code
{
    [_code autorelease];
    _code = nil;
    
    if(code) {
        NSError *error = nil;
        GFXValue *parsedCode = nil;
        if((parsedCode = [self.interpreter parseString:code error:&error])) {
            _code = [code copy];
            self.parsedCode = parsedCode;
        } else {
            self.parsedCode = nil;
            [self handleRenderTimeError:error];
        }
    } else {
        self.parsedCode = nil;
    }
    
    _gfxLayer->setNeedsDisplay();
}

- (void)setWantsEmptyPlaceholder:(BOOL)wantsEmptyPlaceholder
{
    if(_wantsEmptyPlaceholder != wantsEmptyPlaceholder) {
        _wantsEmptyPlaceholder = wantsEmptyPlaceholder;
        
        _gfxLayer->setNeedsDisplay();
    }
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    
    CGRect graphicsLayerFrame = {CGPointZero, frame.size};
    _gfxLayer->setFrame(graphicsLayerFrame);
}

@end

#endif /* GFX_Layer_Use_CA */
