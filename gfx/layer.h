//
//  layer.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__layer__
#define __gfx__layer__

#include "base.h"
#include "types.h"
#include "array.h"
#include "signal.h"

namespace gfx {
    class LayerBacking;
    class Context;
    
    ///The Layer class manages rasterized contents, and encapsulates rendering
    ///trees of them into `gfx::Context` objects. The Layer class is a core
    ///component of the animation architecture of the Gfx language and graphics
    ///stack.
    ///
    ///Layer may be backed by multiple different representations on a given platform.
    ///For example, on OS X and iOS, a Layer may be backed by a CGLayer, or by a CALayer
    ///depending on the usage of the Gfx stack.
    class Layer : public Base
    {
    public:
        
        ///A function used to render the contents of the layer.
        ///
        /// \param  layer   The layer being rendered.
        /// \param  rect    The bounding rect of the layer.
        ///
        ///The result of the function is cached, however, the function
        ///should still attempt to complete its work as quickly as possible.
        typedef std::function<void(Layer *layer, Rect rect)> DrawFunctor;
        
        enum RenderOptions : int
        {
            None = 0,
            RenderBorder = (1 << 1),
        };
        
    protected:
        
        ///The function that will be used to render the contents of the layer.
        DrawFunctor mDrawFunctor;
        
        ///The superlayer of the layer.
        Layer *mSuperlayer;
        
        ///The sublayers of the layer.
        Array<Layer> *mSublayers;
        
        ///The backing of the Layer.
        ///
        ///A layer's backing will vary depending on target platform,
        ///and sometimes on the same platform bsaed on compile-time
        ///options set in the `gfx_defines` header.
        LayerBacking *mBacking;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Constructs a new Layer from a given size and draw functor.
        ///
        /// \param  frame       The frame of the layer.
        /// \param  drawFunctor The function that will render the contents of the layer.
        /// \param  scale       The scale of the layer. Pass 0.0 to use the default scale.
        ///
        ///This constructor requires that there be an active context on the stack
        ///at the time of creation, otherwise an exception will be raised.
        Layer(Rect frame, DrawFunctor drawFunctor, Float scale = 0.0);
        
        ///The destructor.
        ~Layer();
        
#pragma mark - Introspection
        
        ///Sets the draw functor to use for the layer.
        ///
        /// \param  functor The new functor to use to populate the layer's contents.
        ///
        ///This method marks the layer as needing display.
        virtual void setDrawFunctor(const DrawFunctor &functor);
        
        ///Returns the draw functor of the layer.
        virtual const DrawFunctor &drawFunctor() const;
        
        ///Returns the scale factor of the layer.
        virtual Float scale() const;
        
#pragma mark - Metrics
        
        ///Sets the frame of the layer.
        ///
        /// \param  frame   A rectangle in the coordinate system of the receiver's superlayer.
        virtual void setFrame(Rect frame);
        
        ///Returns the frame of the layer.
        virtual Rect frame() const;
        
#pragma mark - Layers and Sublayers
        
        ///Returns the parent layer of the receiver, if there is one.
        virtual Layer *superlayer() const;
        
#pragma mark -
        
        ///Adds a given layer as a child of the receiver.
        ///
        /// \param  layer   The layer to have as a child of the receiver. Required.
        ///
        ///If the given `layer` already has a superlayer, this method
        ///first removes it from its current superlayer, and then adds
        ///it as a child of the receiver.
        virtual void addSublayer(Layer *layer);
        
        ///Inserts a given layer as a child of the receiver.
        ///
        /// \param  offset  The offset of the layer. Must be within {0, sublayers().count()}.
        /// \param  layer   The layer to add as a child of the receiver. Required.
        ///
        ///If the given `layer` already has a superlayer, this method
        ///first removes it from its current superlayer, and then adds
        ///it as a child of the receiver.
        virtual void insertSublayer(Index offset, Layer *layer);
        
        ///Removes the receiver from its superlayer, if it has one.
        virtual void removeFromSuperlayer();
        
        ///Returns the sublayers of the receiver.
        virtual const Array<Layer> *sublayers() const;
        
#pragma mark - Rendering
        
        ///Informs the layer that its draw functor will soon be invoked.
        virtual void willDisplay();
        
        ///Draws the contents of the receiver into the current context.
        ///
        ///The default implementation of this method invokes the layer's.
        virtual void draw(Rect rect);
        
        ///Informs the layer that its draw functor was invoked.
        ///
        ///This method broadcasts the `gfx::Layer::DidDisplaySignal`.
        virtual void didDisplay();
        
        ///Marks the layer's contents as invalid, eventually invoking
        ///the receiver's draw functor.
        ///
        ///Depending on the implementation of `gfx::Layer` in use, this method
        ///will either synchronously invoke the draw functor function,
        ///or the draw functor will be called on the next run loop cycle.
        ///Call sites should take this into account as needed.
        virtual void setNeedsDisplay();
        
#pragma mark -
        
        ///Renders the contents of the receiver and its children into a given Context.
        ///
        /// \param  context The context to render the contents into. Required.
        ///
        virtual void render(Context *context, RenderOptions renderOptions = RenderOptions::None);
        
#pragma mark - Signals
        
        ///A signal that broadcasts whenever the Layer's contents will be redrawn.
        ///
        ///The signal parameter is the Layer that sent the signal.
        Signal<Layer *> WillDisplaySignal;
        
        ///A signal that broadcasts whenever the Layer's contents are redrawn.
        ///
        ///The signal parameter is the Layer that sent the signal.
        Signal<Layer *> DidDisplaySignal;
    };
}

#endif /* defined(__gfx__layer__) */
