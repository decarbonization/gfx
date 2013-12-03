//
//  context.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__context__
#define __gfx__context__

#if GFX_Include_GraphicsStack

#include <CoreGraphics/CoreGraphics.h>
#include "base.h"

namespace gfx {
    class Image;
    
    ///The Context class encapsulates the core 2D drawing destination used by
    ///the gfx language and drawing stack.
    ///
    ///Context instances are drawn to by manipulating the current context stack,
    ///which is exposed through the static methods `gfx::Context::pushContext`,
    ///`gfx::Context::popContext`, and `gfx::Context::currentContext`. The stack
    ///is currently not thread safe, and should only be used from the main thread.
    ///
    ///Instances of Context are not thread-safe. Instances may be created on
    ///non-main threads, but they must never be shared between threads.
    ///
    ///The underlying type of Context is platform independent. On iOS and OS X
    ///it is an instance of the CGContext class-type.
    class Context : public Base
    {
    public:
        
        ///The underlying object used by the Context for its rendering operations.
        typedef CGContextRef NativeType;
        
    protected:
        
        ///The underlying context object.
        NativeType mContext;
        
        ///The scale factor of the context.
        Float mScale;
        
        ///Whether or not the context should destroy the context when it is destructed.
        bool mOwnsContext;
        
    public:
        
#pragma mark - Context Stack
        
        ///Makes the specified context the current context.
        ///
        /// \param  context The context to use for all drawing operations.
        ///
        static void pushContext(Context *context);
        
        ///Removes the current context from the top of the stack,
        ///making the previous context the current one.
        static void popContext();
        
        ///Removes all contexts from the stack, releasing them.
        static void emptyContextStack();
        
        ///Returns top most context on the stack.
        ///
        ///The current context is the target of all state-machine
        ///style operations defined on `gfx::Path`.
        static Context *currentContext();
        
#pragma mark - Lifecycle
        
        ///Returns the default scale to use for contexts. This corresponds
        ///to whatever the scale of the host's main screen is. If the main
        ///screen's scale cannot be determined, this method returns `1.0`.
        static Float defaultScale();
        
        ///Creates a new bitmap context with a given size and scale.
        ///
        /// \param  size    The size of the context to create.
        /// \param  scale   The number of hardware pixels per point in the returned context.
        ///                 Pass in 0.0 to use the `gfx::Context::defaultScale`.
        ///
        /// \result A new autoreleased Context object ready for use.
        ///
        static Context *bitmapContextWith(Size size, Float scale = 0.0);
        
#pragma mark -
        
        ///Constructs a context with a given native object.
        ///
        /// \param  context     The native object to construct the context with. Should not be null.
        /// \param  scale       The number of hardware pixels per point in the context. This value is
        ///                     used in drawing calculations in various parts of the drawing stack, if
        ///                     the scale is unknown, pass `1.0`.
        /// \param  ownsContext Whether or not the native object should be destroyed
        ///                     when the Context object is destructed. Default is true.
        ///
        ///Context objects are typically created through `gfx::Layer`
        ///instances, or through `gfx::Context::bitmapContextWith`.
        Context(NativeType context, Float scale, bool ownsContext = true);
        
        ///The destructor.
        virtual ~Context();
        
#pragma mark - Identity
        
        virtual HashCode hash() const override;
        virtual bool isEqual(const Context *other) const;
        virtual bool isEqual(const Base *other) const override;
        
#pragma mark - Introspection
        
        ///Returns a bool indicating whether or not the context owns its native object.
        bool ownsContext() const;
        
        ///Returns the underlying native object of the context.
        NativeType get() const;
        
        ///Returns the scale of the context.
        Float scale() const;
        
        ///Creates a new image from the contents of the context.
        ///
        /// \result A new autoreleased Image object encapsulating the context's contents.
        Image *makeImage() const;
        
        ///Returns the bounding rectangle of the context.
        Rect boundingRect() const;
        
#pragma mark - Saving/Restoring State
        
        ///Saves the current state of the graphics context, and creates
        ///a new context identical to the previously current state.
        void save();
        
        ///Removes the current state of the graphics context, and restores the
        ///state that was in place before the last call to `gfx::Context::Save`.
        void restore();
        
        ///Saves the state of the graphics context, invokes a given
        ///functor, and then restores the state to the saved snapshot.
        ///
        ///This method guarantees that the graphics state will be rolled
        ///back to pre-transaction state in the event of an exception. It
        ///should be preferred over a balanced set of calls to the
        ///`gfx::Context::save`, and `gfx::Context::restore` methods.
        void transaction(std::function<void(Context *context)> transactionFunctor);
        
#pragma mark - Transforms
        
        ///Returns the current transformation matrix of the context.
        Transform2D currentTransformationMatrix() const;
        
        ///Concats a given 2D transformation matrix onto the matrix of the receiver.
        ///
        /// \param  transform   The transform to concat.
        ///
        void concatTransformationMatrix(const Transform2D &transform);
        
#pragma mark - Clearing
        
        ///Clears a given section of the context.
        void clear(Rect rect);
    };
}

#endif /* GFX_Include_GraphicsStack */

#endif /* defined(__gfx__context__) */
