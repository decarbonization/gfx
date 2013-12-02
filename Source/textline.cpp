//
//  textline.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/1/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "textline.h"
#include "font.h"
#include "color.h"
#include "context.h"

namespace gfx {
    
    const String *const kFontAttributeName = new String("kFontAttributeName");
    const String *const kForegroundColorAttributeName = new String("kForegroundColorAttributeName");
    
#pragma mark - Vending Lines
    
    static CFAttributedStringRef MakeAttributedStringWithAttributes(const String *string, const Dictionary<const String, Base> *attributes)
    {
        gfx_assert_param(string);
        
        cf::MutableDictionaryAutoRef mappedAttributes = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        
        if(attributes) {
            attributes->iterate([mappedAttributes](const String *key, Base *value) {
                if(key->isEqual(kFontAttributeName)) {
                    auto font = dynamic_cast<Font *>(value);
                    gfx_assert(font != nullptr, "kFontAttributeName with non-font object."_gfx);
                    CFDictionarySetValue(mappedAttributes, kCTFontAttributeName, font->get());
                } else if(key->isEqual(kForegroundColorAttributeName)) {
                    auto color = dynamic_cast<Color *>(value);
                    gfx_assert(color != nullptr, "kCTForegroundColorAttributeName with non-color object."_gfx);
                    CFDictionarySetValue(mappedAttributes, kCTForegroundColorAttributeName, color->get());
                } else {
                    std::cerr << "Unknown and unsupported text attribute named " << key << "given. Ignoring. This may be a hard error in the future.";
                }
            });
        }
        
        return CFAttributedStringCreate(kCFAllocatorDefault, string->getStorage(), mappedAttributes);
    }
    
    TextLine *TextLine::withString(const String *string, const Dictionary<const String, Base> *attributes)
    {
        gfx_assert_param(string);
        
        cf::AutoRef<CFAttributedStringRef> attributedString = MakeAttributedStringWithAttributes(string, attributes);
        cf::AutoRef<CTLineRef> line = CTLineCreateWithAttributedString(attributedString);
        return make<TextLine>(line);
    }
    
#pragma mark - Lifecycle
    
    TextLine::TextLine(NativeType line) :
        Base(),
        mLine((CTLineRef)CFRetain(line))
    {
    }
    
    TextLine::~TextLine()
    {
        if(mLine) {
            CFRelease(mLine);
            mLine = NULL;
        }
    }
    
#pragma mark - Introspection
    
    TextLine::NativeType TextLine::get() const
    {
        return mLine;
    }
    
    Rect TextLine::bounds() const
    {
        return CTLineGetImageBounds(get(), Context::currentContext()->get());
    }
    
#pragma mark - Drawing
    
    void TextLine::drawAtPoint(Point point)
    {
        Context::currentContext()->transaction([this, point](Context *context) {
            CGContextSetTextPosition(context->get(), point.x, point.y);
            CTLineDraw(get(), context->get());
        });
    }
    
    void TextLine::drawInRect(Rect rect)
    {
        Context::currentContext()->transaction([this, rect](Context *context) {
            CGContextSetTextPosition(context->get(), rect.origin.x, rect.origin.y);
            CGContextClipToRect(context->get(), rect);
            CTLineDraw(get(), context->get());
        });
    }
}
