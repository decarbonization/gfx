//
//  GFXPortalView.m
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Layer_Use_CA

#import "GFXPortalView.h"
#import <QuartzCore/QuartzCore.h>

#import "interpreter.h"
#import "parser.h"
#import "function.h"
#import "stackframe.h"

#import "graphics.h"
#import "context.h"
#import "layer.h"
#import "color.h"
#import "path.h"

@interface GFXPortalView ()

#pragma mark - readwrite

@property (nonatomic, readwrite, assign) CALayer *contentLayer;
@property (nonatomic, readwrite) gfx::Interpreter *interpreter;
@property (nonatomic, readwrite) gfx::Layer *gfxLayer;

@end

#pragma mark -

@implementation GFXPortalView

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    _gfxLayer->setDrawFunctor(nullptr);
    released(_gfxLayer);
    
    [super dealloc];
}

- (instancetype)initWithSize:(CGSize)size interpreter:(gfx::Interpreter *)interpreter
{
    if((self = [super initWithFrame:CGRectMake(0.0, 0.0, size.width, size.height)])) {
        [self setWantsLayer:YES];
        self.layer.backgroundColor = [NSColor whiteColor].CGColor;
        
        [self setPostsFrameChangedNotifications:YES];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(layoutSubviews)
                                                     name:NSViewFrameDidChangeNotification
                                                   object:self];
        
        
        self.interpreter = interpreter;
        
        auto drawFunctor = [self](gfx::Layer *layer, gfx::Rect rect) {
            [self drawGraphicsLayer:layer inRect:rect];
        };
        auto exceptionHandler = [self](const gfx::Exception &e) {
            NSError *error = [NSError errorWithDomain:@"GFXPortalViewRenderErrorDomain"
                                                 code:'!ren'
                                             userInfo:@{NSLocalizedDescriptionKey: (NSString *)e.reason()->getStorage()}];
            [self handleRenderTimeError:error];
            return true;
        };
        self.gfxLayer = new gfx::Layer(self.frame, drawFunctor, [self layer].contentsScale);
        self.gfxLayer->setDrawExceptionHandler(exceptionHandler);
        
        self.contentLayer = self.gfxLayer->CALayer();
        self.contentLayer.shadowColor = [NSColor blackColor].CGColor;
        self.contentLayer.shadowOpacity = 0.3;
        self.contentLayer.shadowRadius = 10.0;
        [self.layer addSublayer:self.contentLayer];
    }
    
    return self;
}

- (id)initWithFrame:(NSRect)frameRect
{
    gfx::Interpreter *newInterpreter = gfx::make<gfx::Interpreter>();
    if((self = [self initWithSize:frameRect.size interpreter:newInterpreter])) {
        self.frameOrigin = frameRect.origin;
    }
    
    return self;
}

#pragma mark - Layout

- (void)layoutSubviews
{
    [CATransaction begin];
    [CATransaction setAnimationDuration:0.0];
    
    CGRect bounds = self.bounds;
    
    CGRect contentLayerFrame = _contentLayer.bounds;
    contentLayerFrame.origin.x = round(CGRectGetMidX(bounds) - CGRectGetWidth(contentLayerFrame) / 2.0);
    contentLayerFrame.origin.y = round(CGRectGetMidY(bounds) - CGRectGetHeight(contentLayerFrame) / 2.0);
    _contentLayer.frame = contentLayerFrame;
    
    [CATransaction commit];
}

#pragma mark - Callbacks

- (void)drawGraphicsLayer:(gfx::Layer *)layer inRect:(gfx::Rect)rect
{
    if(_function) {
        gfx::StackFrame *frame = self.interpreter->currentFrame();
        frame->push(gfx::VectorFromRect(rect));
        _function->apply(frame);
        frame->safeDrop();
    } else {
        gfx::Color::white()->set();
        gfx::Path::fillRect(rect);
    }
    
    [_delegate portalViewDidDraw:self];
}

- (void)handleRenderTimeError:(NSError *)error
{
    self.function = nil;
    [self.delegate portalView:self didEncounterError:error];
}

#pragma mark - Properties

- (void)setInterpreter:(gfx::Interpreter *)interpreter
{
    autoreleased(_interpreter);
    _interpreter = retained(interpreter);
}

- (void)setFunction:(gfx::Function *)function
{
    gfx::autoreleased(_function);
    _function = retained(function);
    
    self.gfxLayer->setNeedsDisplay();
}

#pragma mark - Evaluation

- (BOOL)runString:(NSString *)string error:(NSError **)error
{
    if(string.length > 0) {
        NSString *enclosedString = [NSString stringWithFormat:@"{ %@ }", string];
        try {
            auto expressions = gfx::Parser(gfx::make<gfx::String>((CFStringRef)enclosedString)).parse();
            _interpreter->eval(expressions);
            
            self.function = _interpreter->currentFrame()->popFunction();
        } catch (gfx::Parser::ParsingException e) {
            if(error) *error = [NSError errorWithDomain:@"gfx::Parser::ErrorDomain"
                                                   code:'pars'
                                               userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"Parsing error. %@", e.reason()->getStorage()]}];
            
            return NO;
        } catch (gfx::Exception e) {
            if(error) *error = [NSError errorWithDomain:@"gfx::Interpreter::ErrorDomain"
                                                   code:'pars'
                                               userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"Error running code. %@", e.reason()->getStorage()]}];
            
            return NO;
        }
        
        return YES;
    } else {
        self.function = nullptr;
        return YES;
    }
}

@end

#endif /* GFX_Layer_Use_CA */
