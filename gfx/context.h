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
    class Context : public Base
    {
    public:
        
        typedef CGContextRef NativeType;
        
    protected:
        
        NativeType mContext;
        bool mOwnsContext;
        
    public:
        
#pragma mark - Context Stack
        
        static void pushContext(Context *context);
        static void popContext();
        static Context *currentContext();
        
#pragma mark - Lifecycle
        
        static Context *bitmapContextWith(Size size, Float scale = 1.0);
        
#pragma mark -
        
        Context(NativeType context, bool ownsContext = true);
        virtual ~Context();
        
#pragma mark - Identity
        
        virtual HashCode hash() const override;
        virtual bool isEqual(const Context *other) const;
        virtual bool isEqual(const Base *other) const override;
        
#pragma mark - Introspection
        
        bool ownsContext() const;
        NativeType get() const;
        CGImageRef createImage() const;
        Rect boundingRect() const;
        
#pragma mark - Saving/Restoring State
        
        void save();
        void restore();
        void transaction(std::function<void(Context *context)> transactionFunctor);
    };
}

#endif /* GFX_Include_GraphicsStack */

#endif /* defined(__gfx__context__) */
