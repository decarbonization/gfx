//
//  layer.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "layer.h"
#include "context.h"
#include "color.h"
#include "path.h"

#if GFX_Layer_Use_CG
#   include "layerbacking_cg.h"
#endif /* GFX_Layer_Use_CG */

namespace gfx {
    
#pragma mark - Lifecycle
    
    Layer::Layer(Rect frame, DrawFunctor drawFunctor, Float scale) :
        Base(),
        mDrawFunctor(drawFunctor),
        mSuperlayer(nullptr),
        mSublayers(make<Array<Layer>>()),
        mBacking(new LayerBacking(this, frame, scale)),
        WillDisplaySignal("WillDisplaySignal"_gfx),
        DidDisplaySignal("DidDisplaySignal"_gfx)
    {
        setNeedsDisplay();
    }
    
    Layer::~Layer()
    {
        for (Layer *sublayer : mSublayers)
            sublayer->removeFromSuperlayer();
        
        delete mBacking;
    }
    
#pragma mark - Introspection
    
    void Layer::setDrawFunctor(const DrawFunctor &functor)
    {
        mDrawFunctor = functor;
        setNeedsDisplay();
    }
    
    const Layer::DrawFunctor &Layer::drawFunctor() const
    {
        return mDrawFunctor;
    }
    
    Float Layer::scale() const
    {
        return mBacking->scale();
    }
    
#pragma mark - Metrics
    
    void Layer::setFrame(Rect frame)
    {
        mBacking->setFrame(frame);
    }
    
    Rect Layer::frame() const
    {
        return mBacking->frame();
    }
    
#pragma mark - Layers and Sublayers
    
    Layer *Layer::superlayer() const
    {
        return mSuperlayer;
    }
    
#pragma mark -
    
    void Layer::addSublayer(Layer *layer)
    {
        gfx_assert_param(layer);
        
        insertSublayer(mSublayers->count(), layer);
    }
    
    void Layer::insertSublayer(Index offset, Layer *layer)
    {
        gfx_assert_param(layer);
        
        if(layer->superlayer() != nullptr)
            layer->removeFromSuperlayer();
        
        mSublayers->insertAt(layer, offset);
        layer->mSuperlayer = this;
        
        mBacking->layerDidInsertSublayer(offset, layer);
    }
    
    void Layer::removeFromSuperlayer()
    {
        if(mSuperlayer) {
            mSuperlayer->mSublayers->remove(this);
            mSuperlayer = nullptr;
            
            mBacking->layerDidRemoveFromSuperlayer();
        }
    }
    
    const Array<Layer> *Layer::sublayers() const
    {
        return retained_autoreleased(mSublayers);
    }
    
#pragma mark - Rendering
    
    void Layer::willDisplay()
    {
        WillDisplaySignal(this);
    }
    
    void Layer::draw(Rect rect)
    {
        mDrawFunctor(this, rect);
    }
    
    void Layer::didDisplay()
    {
        DidDisplaySignal(this);
    }
    
    void Layer::setNeedsDisplay()
    {
        mBacking->setNeedsDisplay();
    }
    
#pragma mark -
    
    void Layer::render(Context *context, RenderOptions renderOptions)
    {
        gfx_assert_param(context);
        
        mBacking->render(context);
        
        if((renderOptions & RenderOptions::RenderBorder) == RenderOptions::RenderBorder) {
            context->transaction([this](Context *context) {
                make<Color>(1.0, 1.0, 0.0, 1.0)->set();
                Path::setDefaultLineWidth(3.0);
                Path::strokeRect(this->frame());
            });
        }
        
        if(!LayerBacking::RendersOwnSublayers) {
            mSublayers->iterate([context, renderOptions](Layer *sublayer, Index index, bool *stop) {
                sublayer->render(context, renderOptions);
            });
        }
    }
}
