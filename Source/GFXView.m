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
    
    self.interpreter = nil;
    
    [super dealloc];
}

- (instancetype)initWithFrame:(NSRect)frameRect interpreter:(GFXInterpreter *)interpreter
{
    NSParameterAssert(interpreter);
    
    if((self = [super initWithFrame:frameRect])) {
        self.interpreter = interpreter;
        
        [self setWantsLayer:YES];
        self.layer = [GFXLayer layerWithInterpreter:_interpreter];
        self.layer.delegate = self;
        
        self.layer.backgroundColor = [NSColor whiteColor].CGColor;
    }
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    return [self initWithFrame:frameRect
                   interpreter:[[GFXInterpreter new] autorelease]];
}

#pragma mark - Layout

- (BOOL)isFlipped
{
    return NO;
}

#pragma mark - Properties

@dynamic layer;

+ (NSSet *)keyPathsForValuesAffectingCode
{
    return [NSSet setWithObjects:@"layer.code", nil];
}

- (void)setCode:(NSString *)code
{
    self.layer.code = code;
}

- (NSString *)code
{
    return self.layer.code;
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
