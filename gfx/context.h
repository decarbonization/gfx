//
//  context.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__context__
#define __gfx__context__

#include <CoreGraphics/CoreGraphics.h>
#include "base.h"

namespace gfx {
    class Context : public Base
    {
        CGContextRef mContext;
        
    public:
        
#pragma mark - Context Stack
        
        static void pushContext(Context *context);
        static void popContext();
        static Context *currentContext();
        
#pragma mark - Lifecycle
        
        static Context *bitmapContextWith(CGSize size, CGFloat scale = 1.0);
        
#pragma mark -
        
        Context(CGContextRef context);
        virtual ~Context();
        
#pragma mark - Identity
        
        virtual CFHashCode hash() const override;
        virtual bool isEqual(const Context *other) const;
        virtual bool isEqual(const Base *other) const override;
        
#pragma mark - Introspection
        
        CGContextRef getContext() const;
        CGImageRef createImage() const;
        CGRect boundingRect() const;
        
#pragma mark - Saving/Restoring State
        
        void save();
        void restore();
        void transaction(std::function<void(Context *context)> transactionFunctor);
    };
}

#endif /* defined(__gfx__context__) */
