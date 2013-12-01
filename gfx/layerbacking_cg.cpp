//
//  LayerBacking_cg.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Layer_Use_CG

#include "layerbacking_cg.h"
#include "layer.h"
#include "context.h"

namespace gfx {
    bool const LayerBacking::RendersOwnSublayers = false;
    
    LayerBacking::LayerBacking(Layer *layer, Rect frame, Float scale) :
        mBacking(),
        mLayer(layer),
        mFrame(frame),
        mScale(scale)
    {
        AutoreleasePool pool;
        
        Context *referenceContext = Context::bitmapContextWith(frame.size, scale);
        mBacking = CGLayerCreateWithContext(referenceContext->get(), referenceContext->boundingRect().size, NULL);
    }
    
    LayerBacking::~LayerBacking()
    {
        CGLayerRelease(mBacking);
    }
    
#pragma mark - Properties
    
    Layer *LayerBacking::layer() const
    {
        return mLayer;
    }
    
#pragma mark -
    
    void LayerBacking::setScale(Float scale)
    {
        mScale = scale;
    }
    
    Float LayerBacking::scale() const
    {
        return mScale;
    }
    
    void LayerBacking::setFrame(Rect frame)
    {
        if(frame.size.width != mFrame.size.width || frame.size.height != mFrame.size.height) {
            CGContextRef referenceContext = CGLayerGetContext(mBacking);
            CGLayerRef newBacking = CGLayerCreateWithContext(referenceContext, CGContextGetClipBoundingBox(referenceContext).size, NULL);
            
            CGLayerRelease(mBacking);
            mBacking = newBacking;
            
            setNeedsDisplay();
        }
        
        mFrame = frame;
    }
    
    Rect LayerBacking::frame() const
    {
        return mFrame;
    }
    
#pragma mark - Drawing
    
    void LayerBacking::display()
    {
        AutoreleasePool pool;
        
        mLayer->willDisplay();
        
        Context *layerContext = make<Context>(CGLayerGetContext(mBacking), scale(), false);
        Rect boundingRect = { {}, frame().size };
        layerContext->clear(boundingRect);
        
        Context::pushContext(layerContext);
        layerContext->transaction([this, boundingRect](Context *context) {
            mLayer->draw(boundingRect);
        });
        Context::popContext();
        
        mLayer->didDisplay();
    }
    
    void LayerBacking::setNeedsDisplay()
    {
        display();
    }
    
    void LayerBacking::render(Context *context)
    {
        gfx_assert_param(context);
        
        CGContextDrawLayerAtPoint(context->get(), frame().origin, mBacking);
    }
    
#pragma mark - Hooks
    
    void LayerBacking::layerDidInsertSublayer(Index offset, Layer *newSublayer)
    {
        //Do nothing.
    }
    
    void LayerBacking::layerDidRemoveFromSuperlayer()
    {
        //Do nothing.
    }
}

#endif /* GFX_Layer_Use_CG */
