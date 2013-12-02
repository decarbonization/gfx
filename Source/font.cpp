//
//  font.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/1/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "font.h"
#include "str.h"

namespace gfx {
    
#pragma mark - Vending Fonts
    
    Font *Font::withName(const String *name, Float size)
    {
        gfx_assert_param(name);
        
        cf::AutoRef<CTFontRef> nativeFont = CTFontCreateWithName(name->getStorage(), size, NULL);
        return make<Font>(nativeFont);
    }
    
#pragma mark - Lifecycle
    
    Font::Font(NativeType font) :
        Base(),
        mFont((NativeType)CFRetain(font))
    {
    }
    
    Font::Font(const Font *font) :
        Font(font->get())
    {
    }
    
    Font::~Font()
    {
        if(mFont) {
            CFRelease(mFont);
            mFont = NULL;
        }
    }
    
#pragma mark - Identity
    
    HashCode Font::hash() const
    {
        return CFHash(mFont);
    }
    
    bool Font::isEqual(const Font *other) const
    {
        if(!other)
            return false;
        
        return CFEqual(get(), other->get());
    }
    
    bool Font::isEqual(const Base *other) const
    {
        if(other && other->isKindOfClass<Font>())
            return this->isEqual((const Font *)other);
        
        return false;
    }
    
    const String *Font::description() const
    {
        String::Builder description;
        
        description << "<" << this->className() << ":" << (void *)this << " ";
        description << "name => " << this->displayName() << ", size => " << this->size();
        description << ">";
        
        return description;
    }
    
#pragma mark - Introspection
    
    Font::NativeType Font::get() const
    {
        return mFont;
    }
    
    Float Font::size() const
    {
        return CTFontGetSize(get());
    }
    
    Transform2D Font::matrix() const
    {
        return CTFontGetMatrix(get());
    }
    
#pragma mark -
    
    const String *Font::displayName() const
    {
        cf::StringAutoRef displayName = CTFontCopyFullName(get());
        return make<String>(displayName);
    }
    
    const String *Font::fullName() const
    {
        cf::StringAutoRef fullName = CTFontCopyFullName(get());
        return make<String>(fullName);
    }
}
