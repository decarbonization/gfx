//
//  font.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/1/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "font.h"
#include "str.h"

#include "stackframe.h"
#include "number.h"

namespace gfx {
    
#pragma mark - Vending Fonts
    
    Font *Font::withName(const String *name, Float size)
    {
        gfx_assert_param(name);
        
        cf::AutoRef<CTFontRef> nativeFont = CTFontCreateWithName(name->getStorage(), size, NULL);
        return make<Font>(nativeFont);
    }
    
#pragma mark -
    
    Font *Font::regular(Float size)
    {
        return Font::withName(str("HelveticaNeue"), size);
    }
    
    Font *Font::bold(Float size)
    {
        return Font::withName(str("HelveticaNeue-Bold"), size);
    }
    
    Font *Font::italic(Float size)
    {
        return Font::withName(str("HelveticaNeue-Italic"), size);
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
        return CFHash(get());
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
    
    Float Font::ascent() const
    {
        return CTFontGetAscent(get());
    }
    
    Float Font::descent() const
    {
        return CTFontGetDescent(get());
    }
    
    Float Font::leading() const
    {
        return CTFontGetLeading(get());
    }
    
    Float Font::lineHeight() const
    {
        return ascent() + descent() + leading();
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
    
#pragma mark - Functions
    
    static void font_make(StackFrame *stack)
    {
        /* str num -- font */
        auto size = stack->popNumber();
        auto name = stack->popString();
        stack->push(Font::withName(name, size->value()));
    }
    
    static void font_regular(StackFrame *stack)
    {
        /* num -- font */
        auto size = stack->popNumber();
        stack->push(Font::regular(size->value()));
    }
    
    static void font_bold(StackFrame *stack)
    {
        /* num -- font */
        auto size = stack->popNumber();
        stack->push(Font::bold(size->value()));
    }
    
    static void font_italic(StackFrame *stack)
    {
        /* num -- font */
        auto size = stack->popNumber();
        stack->push(Font::italic(size->value()));
    }
    
#pragma mark -
    
    void Font::AddTo(gfx::StackFrame *frame)
    {
        gfx_assert_param(frame);
        
        frame->createFunctionBinding(str("font"), &font_make);
        frame->createFunctionBinding(str("font/regular"), &font_regular);
        frame->createFunctionBinding(str("font/bold"), &font_bold);
        frame->createFunctionBinding(str("font/italic"), &font_italic);
    }
}
