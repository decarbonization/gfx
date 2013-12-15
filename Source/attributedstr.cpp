//
//  attributedstr.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "attributedstr.h"
#include <CoreText/CoreText.h>

#include "number.h"
#include "font.h"
#include "color.h"
#include "exception.h"

#include "context.h"

#include "graphics.h"
#include "stackframe.h"

namespace gfx {
    const String *const kFontAttributeName = new String("kFontAttributeName");
    const String *const kForegroundColorAttributeName = new String("kForegroundColorAttributeName");
    
    static CFStringRef GfxAttributeNameToCTAttribute(const String *name)
    {
        if(!name)
            return NULL;
        
        if(name->isEqual(kFontAttributeName))
            return kCTFontAttributeName;
        else if(name->isEqual(kForegroundColorAttributeName))
            return kCTForegroundColorAttributeName;
        else
            throw Exception((String::Builder() << "Unknown attribute with name '" << name << "'."), nullptr);
    }
    
    static Base *GfxObjectFromNativeType(CFTypeRef value)
    {
        if(!value)
            return nullptr;
        
        if(CFGetTypeID(value) == CGColorGetTypeID()) {
            return make<Color>(CGColorRef(value));
        } else if(CFGetTypeID(value) == CTFontGetTypeID()) {
            return make<Font>(CTFontRef(value));
        } else if(CFGetTypeID(value) == CFStringGetTypeID()) {
            return make<String>(CFStringRef(value));
        } else if(CFGetTypeID(value) == CFNumberGetTypeID()) {
            double numberValue = 0.0;
            CFNumberGetValue(CFNumberRef(value), kCFNumberDoubleType, &numberValue);
            return make<Number>(numberValue);
        } else {
            throw Exception(str("Unsupported attribute type"), nullptr);
        }
    }
    
#pragma mark - Internal
    
    static CFMutableAttributedStringRef MakeMutableAttributedStringWithAttributes(const String *string, const Dictionary<const String, Base> *attributes)
    {
        gfx_assert_param(string);
        
        cf::MutableDictionaryAutoRef mappedAttributes = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        
        if(attributes) {
            attributes->iterate([mappedAttributes](const String *key, Base *value) {
                if(key->isEqual(kFontAttributeName)) {
                    auto font = dynamic_cast<Font *>(value);
                    gfx_assert(font != nullptr, str("kFontAttributeName with non-font object."));
                    CFDictionarySetValue(mappedAttributes, kCTFontAttributeName, font->get());
                } else if(key->isEqual(kForegroundColorAttributeName)) {
                    auto color = dynamic_cast<Color *>(value);
                    gfx_assert(color != nullptr, str("kCTForegroundColorAttributeName with non-color object."));
                    CFDictionarySetValue(mappedAttributes, kCTForegroundColorAttributeName, color->get());
                } else {
                    std::cerr << "Unknown and unsupported text attribute named " << key << "given. Ignoring. This may be a hard error in the future.";
                }
            });
        }
        
        auto attributedString = CFAttributedStringCreateMutable(kCFAllocatorDefault, string->length());
        CFAttributedStringReplaceString(attributedString, CFRangeMake(0, CFAttributedStringGetLength(attributedString)), string->getStorage());
        CFAttributedStringSetAttributes(attributedString, CFRangeMake(0, CFAttributedStringGetLength(attributedString)), mappedAttributes, true);
        return attributedString;
    }
    
#pragma mark - Lifecycle
    
    AttributedString::AttributedString(const String *string, const Dictionary<const String, Base> *attributes) :
        Base(),
        mStorage(MakeMutableAttributedStringWithAttributes(string, attributes))
    {
        
    }
    
    AttributedString::AttributedString(const AttributedString *other) :
        Base(),
        mStorage(CFAttributedStringCreateMutableCopy(kCFAllocatorDefault, 0, other->get()))
    {
    }
    
    AttributedString::~AttributedString()
    {
        if(mStorage) {
            CFRelease(mStorage);
            mStorage = NULL;
        }
    }
    
#pragma mark - Identity
    
    HashCode AttributedString::hash() const
    {
        return CFHash(get());
    }
    
    bool AttributedString::isEqual(const AttributedString *other) const
    {
        if(!other)
            return false;
        
        return CFEqual(get(), other->get());
    }
    
    bool AttributedString::isEqual(const Base *other) const
    {
        if(other && other->isKindOfClass<AttributedString>())
            return this->isEqual((const AttributedString *)other);
        
        return false;
    }
    
    const String *AttributedString::description() const
    {
        String::Builder description;
        
        description << "<" << this->className() << ":" << (void *)this << " ";
        description << "'" << this->string() << "'";
        description << ">";
        
        return description;
    }
    
#pragma mark - Storage
    
    AttributedString::NativeType AttributedString::get()
    {
        return mStorage;
    }
    
    AttributedString::ConstNativeType AttributedString::get() const
    {
        return static_cast<ConstNativeType>(mStorage);
    }
    
#pragma mark - Introspection
    
    Index AttributedString::length() const
    {
        return CFAttributedStringGetLength(get());
    }
    
    const String *AttributedString::string() const
    {
        return make<String>(CFAttributedStringGetString(get()));
    }
    
#pragma mark - Attributes
    
    Base *AttributedString::attributeValue(Index locationInString, const String *attributeName)
    {
        gfx_assert(locationInString < length(), str("Out of bounds access"));
        gfx_assert_param(attributeName);
        
        auto attribute = CFAttributedStringGetAttribute(get(), locationInString, GfxAttributeNameToCTAttribute(attributeName), NULL);
        return GfxObjectFromNativeType(attribute);
    }
    
#pragma mark - Drawing
    
    static CTLineRef GetSharedElipsisLine(void)
    {
        static CTLineRef sharedElipsisLine = NULL;
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            CFAttributedStringRef elipsis = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("…"), NULL);
            sharedElipsisLine = CTLineCreateWithAttributedString(elipsis);
            CFRelease(elipsis);
        });
        
        return sharedElipsisLine;
    }
    
    static CGFloat xCTFontGetLineHeight(CTFontRef font)
    {
        return CTFontGetDescent(font) + CTFontGetAscent(font) + CTFontGetLeading(font);
    }
    
    static CFArrayRef CreateLinesForString(CFAttributedStringRef attributedString,
                                           LineBreakMode lineBreakMode,
                                           CGSize boundingSize,
                                           CGSize *outRenderSize)
    {
        CTFontRef font = (CTFontRef)CFAttributedStringGetAttribute(attributedString, 0, kCTFontAttributeName, NULL);
        gfx_assert(font, str("cannot render text without a font."));
        
        CFMutableArrayRef lines = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
        CTTypesetterRef typesetter = CTTypesetterCreateWithAttributedString(attributedString);
        
        CGSize renderSize = CGSizeZero;
        CGFloat lineHeight = xCTFontGetLineHeight(font);
        CFIndex stringLength = CFAttributedStringGetLength(attributedString);
        CFIndex stringOffset = 0;
        while (stringOffset < stringLength) {
            renderSize.height += lineHeight;
            
            CFIndex consumedCharacters = 0;
            CTLineRef line = NULL;
            
            switch (lineBreakMode) {
                case LineBreakMode::CharWrap: {
                    consumedCharacters = CTTypesetterSuggestClusterBreak(typesetter, stringOffset, boundingSize.width);
                    line = CTTypesetterCreateLine(typesetter, CFRangeMake(stringOffset, consumedCharacters));
                    
                    break;
                }
                    
                case LineBreakMode::WordWrap: {
                    consumedCharacters = CTTypesetterSuggestLineBreak(typesetter, stringOffset, boundingSize.width);
                    line = CTTypesetterCreateLine(typesetter, CFRangeMake(stringOffset, consumedCharacters));
                    
                    break;
                }
                    
                case LineBreakMode::TruncateHead:
                case LineBreakMode::TruncateMiddle:
                case LineBreakMode::TruncateTail: {
                    CTLineTruncationType truncationType;
                    if(lineBreakMode == LineBreakMode::TruncateHead)
                        truncationType = kCTLineTruncationStart;
                    else if(lineBreakMode == LineBreakMode::TruncateTail)
                        truncationType = kCTLineTruncationEnd;
                    else
                        truncationType = kCTLineTruncationMiddle;
                    
                    consumedCharacters = stringLength - stringOffset;
                    CTLineRef lineToTruncate = CTTypesetterCreateLine(typesetter, CFRangeMake(stringOffset, consumedCharacters));
                    line = CTLineCreateTruncatedLine(lineToTruncate, boundingSize.width, truncationType, GetSharedElipsisLine());
                    CFRelease(lineToTruncate);
                    
                    break;
                }
            }
            
            if(line) {
                renderSize.width = MAX(renderSize.width, CTLineGetTypographicBounds(line, NULL, NULL, NULL));
                
                CFArrayAppendValue(lines, line);
                CFRelease(line);
            }
            
            stringOffset += consumedCharacters;
            
            if(renderSize.height > boundingSize.height)
                break;
        }
        
        CFRelease(typesetter);
        
        if(outRenderSize) *outRenderSize = renderSize;
        
        return lines;
    }
    
#pragma mark -
    
    static CGFloat AlignmentToFlush(Alignment alignment) {
        switch (alignment) {
            case Alignment::Left:
                return 0.0;
            case Alignment::Center:
                return 0.5;
            case Alignment::Right:
                return 1.0;
        }
    }
    
#pragma mark -
    
    Size AttributedString::size(LineBreakMode lineBreakMode, Size constraintSize) const
    {
        CGSize renderSize = {};
        cf::ArrayAutoRef unusedLines = CreateLinesForString(get(), lineBreakMode, constraintSize, &renderSize);
        return { MIN(renderSize.width, constraintSize.width), MIN(renderSize.height, constraintSize.height) };
    }
    
    Size AttributedString::drawAtPoint(Point point, Float maximumWidth, LineBreakMode lineBreakMode, Alignment alignment)
    {
        CTFontRef font = (CTFontRef)CFAttributedStringGetAttribute(get(), 0, kCTFontAttributeName, NULL);
        gfx_assert(font, str("cannot render text without a font."));
        
        return drawInRect(Rect{{point.x, point.y}, {maximumWidth, xCTFontGetLineHeight(font)}}, lineBreakMode, alignment);
    }
    
    Size AttributedString::drawInRect(Rect rect, LineBreakMode lineBreakMode, Alignment alignment)
    {
        CGSize renderSize = {};
        cf::ArrayAutoRef lines = CreateLinesForString(get(), lineBreakMode, rect.size, &renderSize);
        
        CTFontRef font = (CTFontRef)CFAttributedStringGetAttribute(get(), 0, kCTFontAttributeName, NULL);
        gfx_assert(font, str("cannot render text without a font."));
        
        Context::currentContext()->transaction([rect, lines, font, alignment](Context *ctx) {
            CGContextClipToRect(ctx->get(), rect);
            CGContextTranslateCTM(ctx->get(), rect.getMinX(), rect.getMinY() + CTFontGetAscent(font));
            CGContextSetTextMatrix(ctx->get(), ctx->currentTransformationMatrix());
            
            CGFloat lineOffset = 0.0;
            CGFloat lineHeight = xCTFontGetLineHeight(font);
            CGFloat flush = AlignmentToFlush(alignment);
            for (CFIndex i = 0, c = CFArrayGetCount(lines); i < c; i++) {
                CTLineRef line = (CTLineRef)CFArrayGetValueAtIndex(lines, i);
                
                double penOffset = CTLineGetPenOffsetForFlush(line, flush, rect.getWidth());
                CGContextSetTextPosition(ctx->get(), penOffset, lineOffset);
                
                CTLineDraw(line, ctx->get());
                
                lineOffset += lineHeight;
            }
        });
        
        return renderSize;
    }
    
#pragma mark - Functions
    
    static void text_make(StackFrame *stack)
    {
        /* font color str -- text */
        auto string = stack->popString();
        auto color = stack->popType<Color>();
        auto font = stack->popType<Font>();
        
        auto attributes = make<Dictionary<const String, Base>>();
        attributes->set(kFontAttributeName, font);
        attributes->set(kForegroundColorAttributeName, color);
        stack->push(make<AttributedString>(string, attributes));
    }
    
    static void text_size(StackFrame *stack)
    {
        /* text -- vec */
        auto text = stack->popType<AttributedString>();
        auto vector = VectorFromSize(text->size(LineBreakMode::WordWrap, Context::currentContext()->boundingRect().size));
        stack->push(vector);
    }
    
    static void text_drawAt(StackFrame *stack)
    {
        /* text vec -- */
        auto point = VectorToPoint(stack->popType<Array<Base>>());
        auto text = stack->popType<AttributedString>();
        
        auto boundingRect = Context::currentContext()->boundingRect();
        Rect rect{ point, {boundingRect.getWidth() - point.x, boundingRect.getHeight() - point.y} };
        
        text->drawInRect(rect);
    }
    
#pragma mark -
    
    void AttributedString::AddTo(gfx::StackFrame *frame)
    {
        frame->createFunctionBinding(str("text"), &text_make);
        frame->createFunctionBinding(str("text/size"), &text_size);
        frame->createFunctionBinding(str("text/draw-at"), &text_drawAt);
    }
}
