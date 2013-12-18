//
//  font.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/1/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__font__
#define __gfx__font__

#include "base.h"
#include <CoreText/CoreText.h>

namespace gfx {
    class StackFrame;
    
    ///The Font class encapsulates fonts for the Gfx graphics stack.
    class Font final : public Base
    {
    public:
        
        ///The underlying native type that is backing the Font class.
        typedef CTFontRef NativeType;
        
    protected:
        
        ///The backing font object.
        NativeType mFont;
        
    public:
        
#pragma mark - Vending Fonts
        
        ///Returns a new autoreleased font for a given name and size.
        ///
        /// \param  name    The name of the font to find. Required.
        /// \param  size    The size of the font.
        ///
        /// \result A new autoreleased Font instance if a font by `name` could be found; null otherwise.
        ///
        static Font *withName(const String *name, Float size);
        
#pragma mark -
        
        ///Returns a new autoreleased platform-specific regular weight font of a given size.
        ///
        /// \param  size    The size to use for the font.
        ///
        /// \result A new autoreleased Font instance.
        ///
        static Font *regular(Float size);
        
        ///Returns a new autoreleased platform-specific bold weight font of a given size.
        ///
        /// \param  size    The size to use for the font.
        ///
        /// \result A new autoreleased Font instance.
        ///
        static Font *bold(Float size);
        
        ///Returns a new autoreleased platform-specific italic weight font of a given size.
        ///
        /// \param  size    The size to use for the font.
        ///
        /// \result A new autoreleased Font instance.
        ///
        static Font *italic(Float size);
        
#pragma mark - Lifecycle
        
        ///Constructs a font by taking ownership of a given native object.
        Font(NativeType font);
        
        ///Constructs a font by copying another font.
        Font(const Font *font);
        
        ///The destructor.
        ~Font();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const Font *other) const;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
#pragma mark - Introspection
        
        ///Returns the underlying native object of the font.
        NativeType get() const;
        
        ///Returns the size of the font.
        Float size() const;
        
        ///Returns the matrix of the font.
        Transform2D matrix() const;
        
#pragma mark -
        
        ///Returns the display name of the font.
        const String *displayName() const;
        
        ///Returns the full name of the font.
        const String *fullName() const;
        
#pragma mark -
        
        ///Returns the top y-coordinate of the font's longest ascender.
        Float ascent() const;
        
        ///Returns the bottom y-coordinate of the font's longest ascender.
        Float descent() const;
        
        ///Returns the font's leading.
        Float leading() const;
        
        ///Returns the esimated line height for the font.
        Float lineHeight() const;
        
#pragma mark - Functions
        
        ///Adds the font function suite to a given stack frame.
        static void AddTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__font__) */
