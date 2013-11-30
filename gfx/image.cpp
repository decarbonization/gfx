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

#include <ImageIO/ImageIO.h>

namespace gfx {
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
        Context::currentContext()->transaction([this, rect](Context *context){
            CGContextDrawImage(context->get(), rect, this->get());
        });
    }
    
    void Image::tileInRect(Rect rect)
    {
        Context::currentContext()->transaction([this, rect](Context *context){
            CGContextDrawTiledImage(context->get(), rect, this->get());
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
        CGImageDestinationRef destination = CGImageDestinationCreateWithData(data, RepresentationTypeToUTType(type), 1, NULL);
        CGImageDestinationAddImage(destination, get(), NULL);
        
        if(!CGImageDestinationFinalize(destination)) {
            
        }
        
        CFRelease(destination);
        
        return make<Blob>(data);
    }
}
