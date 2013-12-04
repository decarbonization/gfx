//
//  GFXPortalView.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if __cplusplus
namespace gfx {
    class Interpreter;
    class Layer;
    class Function;
}
#endif /* __cplusplus */

@protocol GFXPortalViewDelegate;

@interface GFXPortalView : NSView

#if __cplusplus

- (instancetype)initWithSize:(CGSize)size interpreter:(gfx::Interpreter *)interpreter;

#pragma mark - Properties

@property (nonatomic, readonly) gfx::Interpreter *interpreter;
@property (nonatomic, readonly) gfx::Layer *gfxLayer;
@property (nonatomic) gfx::Function *function;

#endif /* __cplusplus */

@property (nonatomic, assign) IBOutlet id <GFXPortalViewDelegate> delegate;

#pragma mark - Evaluation

- (BOOL)runString:(NSString *)string error:(NSError **)error;

@end

#pragma mark -

@protocol GFXPortalViewDelegate <NSObject>

- (void)portalViewDidDraw:(GFXPortalView *)sender;
- (void)portalView:(GFXPortalView *)sender didEncounterError:(NSError *)error;

@end
