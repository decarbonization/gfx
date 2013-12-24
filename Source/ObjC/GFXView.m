//
//  GFXPortalView.m
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "GFXView.h"
#import "GFXInterpreter.h"
#import "GFXLayer.h"

@interface GFXView () <GFXLayerDelegate>

#pragma mark - readwrite

@property (nonatomic, readwrite) GFXInterpreter *interpreter;

@end

#pragma mark -

@implementation GFXView

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
#if TARGET_OS_IPHONE
    [_graphicsLayer release];
    _graphicsLayer = nil;
#endif /* TARGET_OS_IPHONE */
    
    self.interpreter = nil;
    
    [super dealloc];
}

- (instancetype)initWithFrame:(CGRect)frameRect interpreter:(GFXInterpreter *)interpreter
{
    NSParameterAssert(interpreter);
    
    if((self = [super initWithFrame:frameRect])) {
        self.interpreter = interpreter;
        
#if TARGET_OS_IPHONE
        _graphicsLayer = [[GFXLayer layerWithInterpreter:_interpreter] retain];
        _graphicsLayer.frame = self.bounds;
        _graphicsLayer.delegate = self;
        [self.layer addSublayer:_graphicsLayer];
        
        self.backgroundColor = [UIColor whiteColor];
#else
        [self setWantsLayer:YES];
        self.layer = [GFXLayer layerWithInterpreter:_interpreter];
        self.layer.delegate = self;
        
        self.layer.backgroundColor = [NSColor whiteColor].CGColor;
        
        _graphicsLayer = (GFXLayer *)self.layer;
#endif /* TARGET_OS_IPHONE */
    }
    
    return self;
}

- (instancetype)initWithFrame:(CGRect)frameRect
{
    return [self initWithFrame:frameRect
                   interpreter:[[GFXInterpreter new] autorelease]];
}

#pragma mark - Layout

#if TARGET_OS_IPHONE

- (void)layoutSubviews
{
    [super layoutSubviews];
    
    _graphicsLayer.frame = self.bounds;
}

#else

- (BOOL)isFlipped
{
    return NO;
}

#endif /* TARGET_OS_IPHONE */

#pragma mark - Properties

+ (NSSet *)keyPathsForValuesAffectingCode
{
    return [NSSet setWithObjects:@"graphicsLayer.code", nil];
}

- (void)setCode:(NSString *)code
{
    self.graphicsLayer.code = code;
}

- (NSString *)code
{
    return self.graphicsLayer.code;
}

#pragma mark - <GFXLayerDelegate>

- (void)layerDidFinishRendering:(GFXLayer *)layer
{
    [self.delegate graphicsViewDidFinishRendering:self];
}

- (void)layer:(GFXLayer *)layer didEncounterError:(NSError *)error
{
    [self.delegate graphicsView:self didEncounterError:error];
}

@end
