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
#include "threading.h"

@interface GFXLayerBackingDelegateAdaptor : NSObject

@property (nonatomic) gfx::LayerBacking *layerBacking;

@end

@implementation GFXLayerBackingDelegateAdaptor

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx
{
    try {
        gfx::AutoreleasePool pool;
        
        gfx::Context *context = gfx::make<gfx::Context>(ctx, layer.contentsScale, false);
        gfx::Context::pushContext(context);
        
        self.layerBacking->display();
        
        gfx::Context::popContext();
    } catch (gfx::Exception e) {
        @throw [NSException exceptionWithName:@"gfx::Exception"
                                       reason:(NSString *)e.reason()->getStorage()
                                     userInfo:nil];
    }
}

@end

#pragma mark -

namespace gfx {
    bool const LayerBacking::RendersOwnSublayers = true;
    
    LayerBacking::LayerBacking(Layer *layer, Rect frame, Float scale) :
        mTexture([CATiledLayer new]),
        mDelegateAdaptor([GFXLayerBackingDelegateAdaptor new]),
        mLayer(layer)
    {
#if !TARGET_OS_IPHONE
        mTexture.geometryFlipped = YES;
#endif /* !TARGET_OS_IPHONE */
        mTexture.frame = frame;
        mTexture.tileSize = frame.size;
        mTexture.needsDisplayOnBoundsChange = YES;
        
        mDelegateAdaptor.layerBacking = this;
        mTexture.delegate = mDelegateAdaptor;
    }
    
    LayerBacking::~LayerBacking()
    {
        mDelegateAdaptor.layerBacking = nullptr;
        
        [mDelegateAdaptor release];
        mDelegateAdaptor = nil;
        
        mTexture.delegate = nil;
        [mTexture release];
        mTexture = nil;
    }
    
#pragma mark - Properties
    
    Layer *LayerBacking::layer() const
    {
        return mLayer;
    }
    
    CALayer *LayerBacking::texture() const
    {
        return [[mTexture retain] autorelease];
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
        mTexture.tileSize = frame.size;
    }
    
    Rect LayerBacking::frame() const
    {
        return mTexture.frame;
    }
    
#pragma mark - Drawing
    
    void LayerBacking::display()
    {
        /* This will be called from a background thread. */
        
        mLayer->willDisplay();
        
        Context *layerContext = Context::currentContext();
        Rect boundingRect{ Point{}, frame().size };
        layerContext->clear(boundingRect);
        
        layerContext->transaction([this, boundingRect](Context *context) {
            mLayer->draw(boundingRect);
        });
        
        mLayer->didDisplay();
    }
    
    void LayerBacking::setNeedsDisplay()
    {
        //It appears that CATiledLayer does not actually invalidate
        //its contents if -setNeedsDisplay is called from a background
        //thread. So we force the call onto the main thread and hope
        //for the best.
        if(threading::isMainThread()) {
            [mTexture setNeedsDisplay];
        } else {
            threading::performOnMainThread([this] {
                [mTexture setNeedsDisplay];
            });
        }
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
