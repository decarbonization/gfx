//
//  layer.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Layer_Use_CG

#include "layer.h"
#include "context.h"
#include "color.h"
#include "path.h"

namespace gfx {
    
    struct LayerImplementation
    {
        CGLayerRef mLayer;
        Point mOrigin;
        Float mScale;
        
        explicit LayerImplementation(Rect frame, Float scale) :
            mLayer(NULL),
            mOrigin(frame.origin),
            mScale(scale)
        {
            AutoreleasePool pool;
            
            Context *referenceContext = Context::bitmapContextWith(frame.size, scale);
            mLayer = CGLayerCreateWithContext(referenceContext->get(), referenceContext->boundingRect().size, NULL);
        }
        
        ~LayerImplementation()
        {
            CGLayerRelease(mLayer);
        }
    };
    
#pragma mark - Lifecycle
    
    Layer::Layer(Rect frame, DrawFunctor drawFunctor, Float scale) :
        Base(),
        mDrawFunctor(drawFunctor),
        mSuperlayer(nullptr),
        mSublayers(make<Array<Layer>>()),
        mStorage(new LayerImplementation(frame, scale)),
        DidDisplaySignal("DidDisplaySignal"_gfx)
    {
        setNeedsDisplay();
    }
    
    Layer::~Layer()
    {
        delete mStorage;
    }
    
#pragma mark - Introspection
    
    const Layer::DrawFunctor &Layer::drawFunctor() const
    {
        return mDrawFunctor;
    }
    
    Float Layer::scale() const
    {
        return mStorage->mScale;
    }
    
#pragma mark - Metrics
    
    void Layer::setSize(Size size)
    {
        Size currentSize = this->size();
        if(currentSize.width != size.width || currentSize.height != size.height) {
            CGContextRef referenceContext = CGLayerGetContext(mStorage->mLayer);
            CGLayerRef newLayer = CGLayerCreateWithContext(referenceContext, CGContextGetClipBoundingBox(referenceContext).size, NULL);
            
            CGLayerRelease(mStorage->mLayer);
            mStorage->mLayer = newLayer;
            
            setNeedsDisplay();
        }
    }
    
    Size Layer::size() const
    {
        return CGLayerGetSize(mStorage->mLayer);
    }
    
    void Layer::setOrigin(Point origin)
    {
        mStorage->mOrigin = origin;
    }
    
    Point Layer::origin() const
    {
        return mStorage->mOrigin;
    }
    
    void Layer::setFrame(Rect frame)
    {
        setOrigin(frame.origin);
        setSize(frame.size);
    }
    
    Rect Layer::frame() const
    {
        return { origin(), size() };
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
    }
    
    void Layer::removeFromSuperlayer()
    {
        if(mSuperlayer) {
            mSuperlayer->mSublayers->remove(this);
            mSuperlayer = nullptr;
        }
    }
    
    const Array<Layer> *Layer::sublayers() const
    {
        return retained_autoreleased(mSublayers);
    }
    
#pragma mark - Rendering
    
    void Layer::display()
    {
        AutoreleasePool pool;
        
        Context *layerContext = make<Context>(CGLayerGetContext(mStorage->mLayer), scale(), false);
        Rect boundingRect = { {}, size() };
        layerContext->clear(boundingRect);
        
        Context::pushContext(layerContext);
        layerContext->transaction([this, boundingRect](Context *context) {
            mDrawFunctor(this, boundingRect);
        });
        Context::popContext();
        
        DidDisplaySignal(this);
    }
    
    void Layer::setNeedsDisplay()
    {
        display();
    }
    
#pragma mark -
    
    void Layer::render(Context *context, RenderOptions renderOptions)
    {
        gfx_assert_param(context);
        
        CGContextDrawLayerAtPoint(context->get(), origin(), mStorage->mLayer);
        
        if((renderOptions & RenderOptions::RenderBorder) == RenderOptions::RenderBorder) {
            context->transaction([this](Context *context) {
                make<Color>(1.0, 1.0, 0.0, 1.0)->set();
                Path::setDefaultLineWidth(3.0);
                Path::strokeRect(this->frame());
            });
        }
        
        mSublayers->iterate([context, renderOptions](Layer *sublayer, Index index, bool *stop) {
            sublayer->render(context, renderOptions);
        });
    }
}

#endif /* GFX_Layer_Use_CG */
