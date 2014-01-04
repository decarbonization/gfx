//
//  image.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "image.h"
#include "context.h"
#include "blob.h"
#include "exception.h"
#include "file.h"

#include <ImageIO/ImageIO.h>

#include "graphics.h"
#include "stackframe.h"

namespace gfx {
    
#pragma mark - Lifecycle
    
    Image *Image::withFile(File *file)
    {
        gfx_assert_param(file);
        
        Blob *fileContents = file->read(file->length());
        cf::AutoRef<CGDataProviderRef> dataProvider = CGDataProviderCreateWithCFData(fileContents->getStorage());
        gfx_assert(*dataProvider, str("Could not create image data provider."));
        
        cf::AutoRef<CGImageSourceRef> source = CGImageSourceCreateWithDataProvider(dataProvider, NULL);
        gfx_assert(*source, str("Could not create image source."));
        
        if(CGImageSourceGetCount(source) > 0) {
            CGImageRef image = CGImageSourceCreateImageAtIndex(source, 0, NULL);
            return make<Image>(image, true);
        } else {
            return nullptr;
        }
    }
    
#pragma mark -
    
    Image::Image(NativeType image, bool ownsImage) :
        Base(),
        mImage(image),
        mOwnsImage(ownsImage)
    {
    }
    
    Image::Image(const Image *image) :
        Image(CGImageRetain(image->get()), true)
    {
    }
    
    Image::~Image()
    {
        if(mOwnsImage) {
            CFRelease(mImage);
            mImage = NULL;
        }
    }
    
#pragma mark - Introspection
    
    Image::NativeType Image::get() const
    {
        return mImage;
    }
    
    Size Image::size() const
    {
        return Size{ (Float)CGImageGetWidth(get()), (Float)CGImageGetHeight(get()) };
    }
    
#pragma mark - Drawing
    
    void Image::drawInRect(Rect rect)
    {
        Context::currentContext()->transaction([this, rect](Context *context) {
            Rect scaledRect = rect;
            scaledRect.origin.x *= context->scale();
            scaledRect.origin.y *= context->scale();
            scaledRect.size.width *= context->scale();
            scaledRect.size.height *= context->scale();
            CGContextDrawImage(context->get(), scaledRect, this->get());
        });
    }
    
    void Image::tileInRect(Rect rect)
    {
        Context::currentContext()->transaction([this, rect](Context *context) {
            Rect scaledRect = rect;
            scaledRect.origin.x *= context->scale();
            scaledRect.origin.y *= context->scale();
            scaledRect.size.width *= context->scale();
            scaledRect.size.height *= context->scale();
            CGContextDrawTiledImage(context->get(), scaledRect, this->get());
        });
    }
    
#pragma mark - Converting to Data
    
    static CFStringRef RepresentationTypeToUTType(Image::RepresentationType type)
    {
        switch (type) {
            case Image::RepresentationType::PNG:
                return CFSTR("public.png");
                
            case Image::RepresentationType::JPEG:
                return CFSTR("public.jpeg");
        }
    }
    
    Blob *Image::makeRepresentation(RepresentationType type) const
    {
        cf::AutoRef<CFMutableDataRef> data = CFDataCreateMutable(kCFAllocatorDefault, 0);
        cf::AutoRef<CGImageDestinationRef> destination = CGImageDestinationCreateWithData(data, RepresentationTypeToUTType(type), 1, NULL);
        CGImageDestinationAddImage(destination, get(), NULL);
        
        if(!CGImageDestinationFinalize(destination)) {
            throw Exception(str("Could not create image blob representation"), nullptr);
        }
        
        return make<Blob>(CFDataRef(data));
    }
    
#pragma mark - Functions
    
    static void image_fromFile(StackFrame *stack)
    {
        /* file -- image */
        auto file = stack->popType<File>();
        stack->push(Image::withFile(file));
    }
    
#pragma mark -
    
    static void image_size(StackFrame *stack)
    {
        /* image -- vec */
        auto image = stack->popType<Image>();
        stack->push(VectorFromSize(image->size()));
    }
    
    static void image_drawIn(StackFrame *stack)
    {
        /* image vec -- */
        auto rect = VectorToRect(stack->popType<Array<Base>>());
        auto image = stack->popType<Image>();
        image->drawInRect(rect);
    }
    
#pragma mark -
    
    static void image_saveTo(StackFrame *stack)
    {
        /* image file -- */
        auto file = stack->popType<File>();
        auto image = stack->popType<Image>();
        
        auto imageBlob = image->makeRepresentation(Image::RepresentationType::PNG);
        file->write(imageBlob);
    }
    
#pragma mark -
    
    void Image::addTo(StackFrame *frame)
    {
        frame->createVariableBinding(str("<image>"), str("gfx::Image"));
        
        frame->createFunctionBinding(str("image/from-file"), &image_fromFile);
        
        frame->createFunctionBinding(str("image/size"), &image_size);
        frame->createFunctionBinding(str("image/draw-in"), &image_drawIn);
        
        frame->createFunctionBinding(str("image/save-to"), &image_saveTo);
    }
}
