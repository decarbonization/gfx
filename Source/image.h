//
//  image.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__image__
#define __gfx__image__

#include "base.h"
#include "types.h"

namespace gfx {
    class Blob;
    class File;
    
    class Image : public Base
    {
    public:
        
        typedef CGImageRef NativeType;
        
        enum class RepresentationType
        {
            PNG,
            JPEG,
        };
        
    protected:
        
        NativeType mImage;
        bool mOwnsImage;
        
    public:
        
#pragma mark - Lifecycle
        
        static Image *withFile(File *file);
        
#pragma mark -
        
        Image(NativeType image, bool ownsImage = true);
        Image(const Image *image);
        ~Image();
        
#pragma mark - Introspection
        
        NativeType get() const;
        Size size() const;
        
#pragma mark - Drawing
        
        void drawInRect(Rect rect);
        void tileInRect(Rect rect);
        
#pragma mark - Converting to Data
        
        Blob *makeRepresentation(RepresentationType type) const;
    };
}

#endif /* defined(__gfx__image__) */
