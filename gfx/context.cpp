//
//  context.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/23/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#if GFX_Include_GraphicsStack

#include "context.h"
#include "array.h"
#include "image.h"

namespace gfx {
    
#pragma mark - Context Stack
    
    static auto gContextStack = new Array<Context>();
    
    void Context::pushContext(Context *context)
    {
        gfx_assert_param(context);
        
        gContextStack->append(context);
    }
    
    void Context::popContext()
    {
        gContextStack->removeLast();
    }
    
    Context *Context::currentContext()
    {
        Context *context = gContextStack->last();
        gfx_assert(context != nullptr, "There are no context on the context stack."_gfx);
        return context;
    }
    
#pragma mark - Lifecycle
    
    Context *Context::bitmapContextWith(Size size, Float scale)
    {
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef imageContext = CGBitmapContextCreate(/* in data */ NULL,
                                                          /* in width */ size.width * scale,
                                                          /* in height */ size.height * scale,
                                                          /* in bitsPerComponent */ 8,
                                                          /* in bytesPerRow */ 0,
                                                          /* in colorSpace */ colorSpace,
                                                          /* in bitmapInfo */ (CGBitmapInfo)kCGImageAlphaPremultipliedLast);
        CGColorSpaceRelease(colorSpace);
        
        CGContextTranslateCTM(imageContext, 0.0, size.height * scale);
        CGContextScaleCTM(imageContext, scale, -scale);
        
        return make<Context>(imageContext);
    }
    
#pragma mark -
    
    Context::Context(Context::NativeType context, bool ownsContext) :
        Base(),
        mContext(context),
        mOwnsContext(ownsContext)
    {
    }
    
    Context::~Context()
    {
        if(mContext && mOwnsContext) {
            CFRelease(mContext);
            mContext = NULL;
        }
    }
    
#pragma mark - Identity
    
    HashCode Context::hash() const
    {
        return CFHash(mContext);
    }
    
    bool Context::isEqual(const Context *other) const
    {
        if(!other)
            return NULL;
        
        return CFEqual(get(), other->get());
    }
    
    bool Context::isEqual(const Base *other) const
    {
        if(!other)
            return nullptr;
        
        if(other->isKindOfClass<Context>())
            return isEqual((const Context *)other);
        
        return false;
    }
    
#pragma mark - Introspection
    
    bool Context::ownsContext() const
    {
        return mOwnsContext;
    }
    
    Context::NativeType Context::get() const
    {
        return mContext;
    }
    
    Image *Context::createImage() const
    {
        return make<Image>(CGBitmapContextCreateImage(get()), true);
    }
    
    Rect Context::boundingRect() const
    {
        return CGContextGetClipBoundingBox(get());
    }
    
#pragma mark - Saving/Restoring State
    
    void Context::save()
    {
        CGContextSaveGState(get());
    }
    
    void Context::restore()
    {
        CGContextRestoreGState(get());
    }
    
    void Context::transaction(std::function<void(Context *context)> transactionFunctor)
    {
        this->save();
        try {
            transactionFunctor(this);
        }
        catch (...) {
            restore();
            throw;
        }
        
        restore();
    }
    
#pragma mark - Transforms
    
    Transform2D Context::currentTransformationMatrix() const
    {
        return CGContextGetCTM(get());
    }
    
    void Context::concatTransformationMatrix(const Transform2D &transform)
    {
        CGContextConcatCTM(get(), transform);
    }
    
#pragma mark - Clearing
    
    void Context::clear(Rect rect)
    {
        CGContextClearRect(get(), rect);
    }
}

#endif /* GFX_Include_GraphicsStack */
