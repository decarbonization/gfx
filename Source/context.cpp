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

#include "stackframe.h"
#include "graphics.h"
#include "file.h"

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
    
    void Context::emptyContextStack()
    {
        gContextStack->removeAll();
    }
    
    Context *Context::currentContext()
    {
        Context *context = gContextStack->last();
        if(!context) {
            context = Context::bitmapContextWith({1.0, 1.0});
            Context::pushContext(context);
        }
        return context;
    }
    
#pragma mark - Lifecycle
    
    Float Context::defaultScale()
    {
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
        CGDirectDisplayID mainDisplay = CGMainDisplayID();
        CGDisplayModeRef displayMode = CGDisplayCopyDisplayMode(mainDisplay);
        
        size_t scale = CGDisplayModeGetPixelHeight(displayMode) / CGDisplayModeGetHeight(displayMode);
        
        CGDisplayModeRelease(displayMode);
        
        return scale;
#else
#warning Current platform does not have a functional implementation for `Context::defaultScale`.
        return 1.0
#endif
    }
    
    Context *Context::bitmapContextWith(Size size, Float scale)
    {
        if(!scale)
            scale = Context::defaultScale();
        
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
        
        return make<Context>(imageContext, scale);
    }
    
#pragma mark -
    
    Context::Context(NativeType context, Float scale, bool ownsContext) :
        Base(),
        mContext(context),
        mScale(scale),
        mOwnsContext(ownsContext)
    {
        gfx_assert_param(context);
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
    
    Float Context::scale() const
    {
        return mScale;
    }
    
    Context::NativeType Context::get() const
    {
        return mContext;
    }
    
    Image *Context::makeImage() const
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
    
#pragma mark - Functions
    
    void Context::clear(Rect rect)
    {
        CGContextClearRect(get(), rect);
    }
    
    static void ctx_begin(StackFrame *stack)
    {
        auto sizeVector = stack->popType<Array<Base>>();
        auto size = VectorToSize(sizeVector);
        Context::pushContext(Context::bitmapContextWith(size));
    }
    
    static void ctx_end(StackFrame *stack)
    {
        Context::popContext();
    }
    
    static void ctx_size(StackFrame *stack)
    {
        auto ctxSize = Context::currentContext()->boundingRect().size;
        stack->push(VectorFromSize(ctxSize));
    }
    
#if GFX_Language_SupportsFiles
    static void ctx_save(StackFrame *stack)
    {
        auto path = stack->popString();
        
        auto image = Context::currentContext()->makeImage();
        auto data = image->makeRepresentation(Image::RepresentationType::PNG);
        
        auto file = make<File>(path, File::Mode::Write);
        file->write(data);
    }
#endif /* GFX_Language_SupportsFiles */
    
#pragma mark -
    
    void Context::addTo(StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        frame->createFunctionBinding(str("ctx/begin"), &ctx_begin);
        frame->createFunctionBinding(str("ctx/end"), &ctx_end);
        frame->createFunctionBinding(str("ctx/size"), &ctx_size);
#if GFX_Language_SupportsFiles
        frame->createFunctionBinding(str("ctx/save"), &ctx_save);
#endif /* GFX_Language_SupportsFiles */
    }
}

#endif /* GFX_Include_GraphicsStack */
