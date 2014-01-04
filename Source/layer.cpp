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
#elif GFX_Layer_Use_CA
#   include "layerbacking_calayer.h"
#endif /* GFX_Layer_Use_CG */

#include "stackframe.h"
#include "interpreter.h"
#include "graphics.h"
#include "function.h"

namespace gfx {
    
#pragma mark - Lifecycle
    
    Layer::Layer(Rect frame, DrawFunctor drawFunctor, Float scale) :
        Base(),
        mDrawFunctor(drawFunctor),
        mSuperlayer(nullptr),
        mSublayers(new Array<Layer>()),
        mBacking(new LayerBacking(this, frame, scale)),
        WillDisplaySignal(str("WillDisplaySignal")),
        DidDisplaySignal(str("DidDisplaySignal"))
    {
        setNeedsDisplay();
    }
    
    Layer::~Layer()
    {
        while (mSublayers->count() > 0)
            mSublayers->last()->removeFromSuperlayer();
        
        released(mSublayers);
        mSublayers = nullptr;
        
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
    
#if GFX_Layer_Use_CA
    
    CALayer *Layer::CALayer() const
    {
        return mBacking->texture();
    }
    
#endif /* GFX_Layer_Use_CA */
    
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
    
    void Layer::setDrawExceptionHandler(const DrawExceptionHandlerFunctor &handler)
    {
        mDrawExceptionHandler = handler;
    }
    
    const Layer::DrawExceptionHandlerFunctor &Layer::drawExceptionHandler() const
    {
        return mDrawExceptionHandler;
    }
    
#pragma mark -
    
    void Layer::willDisplay()
    {
        WillDisplaySignal(this);
    }
    
    void Layer::draw(Rect rect)
    {
        try {
            mDrawFunctor(this, rect);
        } catch (Exception e) {
            if(!mDrawExceptionHandler || !mDrawExceptionHandler(e))
                throw;
        }
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
    
    void Layer::render(Context *context)
    {
        gfx_assert_param(context);
        
        mBacking->render(context);
        
        if(!LayerBacking::RendersOwnSublayers) {
            mSublayers->iterate(mSublayers->all(), [context](Layer *sublayer, Index index, bool *stop) {
                sublayer->render(context);
            });
        }
    }
    
#pragma mark - Layer Functions
    
    static void layer_make(StackFrame *stack)
    {
        /* vec func -- Layer */
        Scoped<StackFrame> stackFrame = stack;
        Scoped<Function> drawFunction = stack->popFunction();
        auto frame = VectorToRect(stack->popType<Array<Base>>());
        stack->push(make<Layer>(frame, [stackFrame, drawFunction](Layer *layer, Rect rect) {
            stackFrame->push(VectorFromRect(rect));
            drawFunction->apply(stackFrame);
            stackFrame->safeDrop();
        }));
    }
    
    static void layer_frame(StackFrame *stack)
    {
        /* Layer -- vec */
        auto layer = stack->popType<Layer>();
        stack->push(VectorFromRect(layer->frame()));
    }
    
    static void layer_setFrame(StackFrame *stack)
    {
        /* layer vec -- */
        auto frameVector = stack->popType<Array<Base>>();
        auto layer = stack->popType<Layer>();
        layer->setFrame(VectorToRect(frameVector));
    }
    
    static void layer_display(StackFrame *stack)
    {
        /* layer -- */
        auto layer = stack->popType<Layer>();
        layer->setNeedsDisplay();
    }
    
    static void layer_render(StackFrame *stack)
    {
        /* layer -- */
        auto layer = stack->popType<Layer>();
        layer->render(Context::currentContext());
    }
    
    static void layer_addChild(StackFrame *stack)
    {
        /* layer(parent) layer(child) -- */
        auto child = stack->popType<Layer>();
        auto parent = stack->popType<Layer>();
        parent->addSublayer(child);
    }
    
    static void layer_removeAsChild(StackFrame *stack)
    {
        /* layer -- */
        auto layer = stack->popType<Layer>();
        layer->removeFromSuperlayer();
    }
    
    static void layer_parent(StackFrame *stack)
    {
        /* layer -- layer */
        auto layer = stack->popType<Layer>();
        stack->push(layer->superlayer());
    }
    
    static void layer_children(StackFrame *stack)
    {
        /* layer -- vec */
        auto layer = stack->popType<Layer>();
        stack->push(copy(layer->sublayers()));
    }
    
#pragma mark -
    
    void Layer::addTo(StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        frame->createVariableBinding(str("<layer>"), str("gfx::Layer"));
        
        frame->createFunctionBinding(str("layer"), &layer_make);
        frame->createFunctionBinding(str("layer/frame"), &layer_frame);
        frame->createFunctionBinding(str("layer/set-frame"), &layer_setFrame);
        frame->createFunctionBinding(str("layer/display"), &layer_display);
        frame->createFunctionBinding(str("layer/render"), &layer_render);
        
        frame->createFunctionBinding(str("layer/add-child"), &layer_addChild);
        frame->createFunctionBinding(str("layer/remove-as-child"), &layer_removeAsChild);
        frame->createFunctionBinding(str("layer/parent"), &layer_parent);
        frame->createFunctionBinding(str("layer/children"), &layer_children);
    }
}
