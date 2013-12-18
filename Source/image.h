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
    
    class StackFrame;
    
    ///The Image class encapsulates the loading and drawing of images in the Gfx graphics stack.
    class Image final : public Base
    {
    public:
        
        ///The underlying native type used by the Image class.
        typedef CGImageRef NativeType;
        
        ///The different representations that may be extracted from an Image.
        enum class RepresentationType
        {
            ///A PNG data blob.
            PNG,
            
            ///A JPEG data blob.
            JPEG,
        };
        
    protected:
        
        ///The underlying native type.
        NativeType mImage;
        
        ///Whether or not underlying native type is owned
        ///by the instance and should be cleaned up.
        bool mOwnsImage;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Attempts to create a new image object with a given `gfx::File` object.
        ///
        /// \param  file    The file to attempt to load. Required.
        ///
        /// \result A new autoreleased Image if the file could be loaded; null otherwise.
        ///
        static Image *withFile(File *file);
        
#pragma mark -
        
        ///Constructs an image with an instance of the underlying type.
        ///
        /// \param  image       The underlying image. Should not be null.
        /// \param  ownsImage   Whether or not the instance owns the underlying image type. Default value is true.
        ///
        Image(NativeType image, bool ownsImage = true);
        
        ///Constructs an image by performing a shallow copy of another images backing.
        ///
        /// \param  image   The other image. Should not be null.
        ///
        Image(const Image *image);
        
        ///The destructor.
        ~Image();
        
#pragma mark - Introspection
        
        ///Returns the underlying native type.
        NativeType get() const;
        
        ///Returns the size of the image.
        Size size() const;
        
#pragma mark - Drawing
        
        ///Renders the contents of the image into a given rectangle.
        ///
        /// \param  rect    The rectangle to render the image into.
        ///
        ///The image does not proportionally scale itself into the rectangle,
        ///it is up to callers to ensure images do not look stretched.
        void drawInRect(Rect rect);
        
        ///Repeatedly renders the image to fill a given rectangle.
        ///
        /// \param  rect    The rectangle to draw the image into.
        ///
        void tileInRect(Rect rect);
        
#pragma mark - Converting to Data
        
        ///Creates a binary blob representation of the image.
        ///
        /// \param  type    The format to use for the binary blob.
        ///
        /// \result A new autoreleased Blob instance.
        Blob *makeRepresentation(RepresentationType type) const;
        
#pragma mark - Functions
        
        ///Adds the image suite to a given stack frame.
        static void addTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__image__) */
