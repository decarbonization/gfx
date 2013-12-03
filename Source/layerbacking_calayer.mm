//
//  layerbacking_calayer.mm
//  gfx
//
//  Created by Kevin MacWhinnie on 12/2/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Layer_Use_CA

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

#include "layerbacking_calayer.h"
#include "layer.h"
#include "context.h"

@interface GFXLayerBackingDelegateAdaptor : NSObject

@property (nonatomic) gfx::LayerBacking *layerBacking;

@end

@implementation GFXLayerBackingDelegateAdaptor

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx
{
    gfx::AutoreleasePool pool;
    
    gfx::Context *context = gfx::make<gfx::Context>(ctx, layer.contentsScale, false);
    gfx::Context::pushContext(context);
    
    self.layerBacking->display();
    
    gfx::Context::popContext();
}

@end

#pragma mark -

namespace gfx {
    bool const LayerBacking::RendersOwnSublayers = true;
    
    LayerBacking::LayerBacking(Layer *layer, Rect frame, Float scale) :
        mTexture([CALayer new]),
        mDelegateAdaptor([GFXLayerBackingDelegateAdaptor new]),
        mLayer(layer)
    {
    }
    
    LayerBacking::~LayerBacking()
    {
        [mDelegateAdaptor release];
        mDelegateAdaptor = nil;
        
        [mTexture release];
        mTexture = nil;
    }
    
#pragma mark - Properties
    
    Layer *LayerBacking::layer() const
    {
        return mLayer;
    }
    
#pragma mark -
    
    void LayerBacking::setScale(Float scale)
    {
        mTexture.contentsScale = scale;
    }
    
    Float LayerBacking::scale() const
    {
        return mTexture.contentsScale;
    }
    
    void LayerBacking::setFrame(Rect frame)
    {
        mTexture.frame = frame;
    }
    
    Rect LayerBacking::frame() const
    {
        return mTexture.frame;
    }
    
#pragma mark - Drawing
    
    void LayerBacking::display()
    {
        mLayer->willDisplay();
        
        Context *layerContext = Context::currentContext();
        Rect boundingRect = { {}, frame().size };
        layerContext->clear(boundingRect);
        
        layerContext->transaction([this, boundingRect](Context *context) {
            mLayer->draw(boundingRect);
        });
        
        mLayer->didDisplay();
    }
    
    void LayerBacking::setNeedsDisplay()
    {
        display();
    }
    
    void LayerBacking::render(Context *context)
    {
        gfx_assert_param(context);
        
        [mTexture renderInContext:context->get()];
    }
    
#pragma mark - Hooks
    
    void LayerBacking::layerDidInsertSublayer(Index offset, Layer *newSublayer)
    {
        [mTexture insertSublayer:newSublayer->mBacking->mTexture atIndex:(unsigned int)offset];
    }
    
    void LayerBacking::layerDidRemoveFromSuperlayer()
    {
        [mTexture removeFromSuperlayer];
    }
}


#endif /* GFX_Layer_Use_CA */
