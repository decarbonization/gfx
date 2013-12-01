//
//  LayerBacking_cg.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__LayerBacking_cg__
#define __gfx__LayerBacking_cg__

#if GFX_Layer_Use_CG

#include "base.h"
#include "types.h"

namespace gfx {
    class Layer;
    class Context;
    
    ///The LayerBacking class encapsulates a platform and situation
    ///independent texture backing for the `gfx::Layer` class.
    ///
    ///LayerBacking does not extend `gfx::Base` as its lifecycle is
    ///entirely internal to the `gfx::Layer` class, and it is never
    ///exposed to the outside world. Additionally, it does not use
    ///virtual methods, as there is only ever one LayerBacking class
    ///in existence in a binary at a single time.
    class LayerBacking
    {
        ///The backing's backing.
        CGLayerRef mBacking;
        
        ///The layer the backing is associated with.
        ///
        ///The backing delegates actual drawing responsibility
        ///to the layer's draw functor, it simply manages setting
        ///up the context stack as appropriate.
        Layer *mLayer;
        
        ///The scale of the layer backing.
        Float mScale;
        
        ///The frame of the layer backing.
        Rect mFrame;
        
    public:
        
        ///Indicates whether or not the current backing class renders
        ///its own children in its `gfx::LayerBacking::render` method.
        static bool const RendersOwnSublayers;
        
        ///Constructs a backing with a given layer, frame, and scale.
        LayerBacking(Layer *layer, Rect frame, Float scale);
        
        ///The destructor.
        ~LayerBacking();
        
#pragma mark - Properties
        
        ///Returns the layer associated with the backing.
        Layer *layer() const;
        
#pragma mark -
        
        ///Sets the scale of the layer backing.
        ///
        ///Setting the scale to 0.0 does not currently
        ///default to using default scale.
        void setScale(Float scale);
        
        ///Returns the scale of the layer backing.
        Float scale() const;
        
        ///Sets the frame of the layer backing.
        void setFrame(Rect frame);
        
        ///Returns the frame of the layer backing.
        Rect frame() const;
        
#pragma mark - Drawing
        
        ///Setups the graphics stack, and invokes the layer's draw functor,
        ///capturing the contents into the receiver's backing texture.
        ///
        ///This method should never be invoked directly.
        ///
        ///It is the responsibility of the layer backing to invoke the following
        ///functions in order:
        /// - `gfx::Layer::willDisplay`
        /// - `gfx::Layer::draw`
        /// - `gfx::Layer::didDisplay`
        void display();
        
        ///Marks the receiver as needing display. The CGLayer implementation
        ///of this method immediately invokes display.
        void setNeedsDisplay();
        
        ///Render the contents of the layer into a given context.
        ///
        /// \param  context The context to render the layer's contents into. Required.
        ///
        ///This method will do nothing if the layer backing has never been displayed.
        void render(Context *context);
        
#pragma mark - Hooks
        
        ///A hook to inform the backing that a sublayer was inserted into the layer.
        ///
        /// \param  offset      The offset the sublayer was inserted at.
        /// \param  newSublayer The sublayer.
        ///
        ///This method is provided for layer backings that must keep shadow layer trees.
        void layerDidInsertSublayer(Index offset, Layer *newSublayer);
        
        ///A hook to inform the backing that a layer was removed from its superlayer.
        ///
        ///This method is provided for layer backings that must keep shadow layer trees.
        void layerDidRemoveFromSuperlayer();
    };
}

#endif /* GFX_Layer_Use_CG */

#endif /* defined(__gfx__LayerBacking_cg__) */
