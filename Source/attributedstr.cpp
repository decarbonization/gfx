//
//  attributedstr.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "attributedstr.h"
#include <CoreText/CoreText.h>
#include <algorithm>

#include "number.h"
#include "font.h"
#include "color.h"
#include "exception.h"

#include "context.h"

#include "graphics.h"
#include "stackframe.h"

namespace gfx {
    
#pragma mark - • TextAttributes
    
    TextAttributes::TextAttributes() :
        Base(),
        mFont(nullptr),
        mForegroundColor(nullptr)
    {
    }
    
    TextAttributes::TextAttributes(const TextAttributes *attributes) :
        Base(),
        mFont(retained(attributes->font())),
        mForegroundColor(retained(attributes->foregroundColor()))
    {
    }
    
    TextAttributes::TextAttributes(CFDictionaryRef dictionary) :
        TextAttributes()
    {
        if(dictionary) {
            auto font = (CTFontRef)CFDictionaryGetValue(dictionary, kCTFontAttributeName);
            if(font)
                this->setFont(make<Font>(font));
            
            auto foregroundColor = (CGColorRef)CFDictionaryGetValue(dictionary, kCTForegroundColorAttributeName);
            if(foregroundColor)
                this->setForegroundColor(make<Color>(foregroundColor));
        }
    }
    
    TextAttributes::~TextAttributes()
    {
        released(mFont);
        mFont = nullptr;
        
        released(mForegroundColor);
        mForegroundColor = nullptr;
    }
    
#pragma mark - Identity
    
    HashCode TextAttributes::hash() const
    {
        HashCode hash = (HashCode)this;
        
        if(mFont) hash ^= mFont->hash();
        if(mForegroundColor) hash ^= mForegroundColor->hash();
        
        return hash;
    }
    
    bool TextAttributes::isEqual(const TextAttributes *other) const
    {
        if(!other)
            return false;
        
        return ((other->mFont && other->mFont->isEqual(mFont)) &&
                (other->mForegroundColor && other->mForegroundColor->isEqual(mForegroundColor)));
    }
    
    bool TextAttributes::isEqual(const Base *other) const
    {
        if(other && other->isKindOfClass<TextAttributes>())
            return this->isEqual((const TextAttributes *)other);
        
        return false;
    }
    
    const String *TextAttributes::description() const
    {
        String::Builder descriptionString;
        
        descriptionString << "<" << this->className() << ":" << (void *)this << " ";
        descriptionString << "font => '" << mFont << "', ";
        descriptionString << "foregroundColor => '" << mForegroundColor << "'";
        descriptionString << ">";
        
        return descriptionString;
    }
    
#pragma mark - Attributes
    
    void TextAttributes::setFont(Font *font)
    {
        autoreleased(mFont);
        mFont = retained(font);
    }
    
    Font *TextAttributes::font() const
    {
        return retained_autoreleased(mFont);
    }
    
#pragma mark -
    
    void TextAttributes::setForegroundColor(Color *color)
    {
        autoreleased(mForegroundColor);
        mForegroundColor = retained(color);
    }
    
    Color *TextAttributes::foregroundColor() const
    {
        return retained_autoreleased(mForegroundColor);
    }
    
#pragma mark -
    
    CFDictionaryRef TextAttributes::copyAttributesDictionary() const
    {
        CFMutableDictionaryRef attributes = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                      &kCFTypeDictionaryKeyCallBacks,
                                                                      &kCFTypeDictionaryValueCallBacks);
        
        auto font = this->font();
        if(font)
            CFDictionarySetValue(attributes, kCTFontAttributeName, font->get());
        
        auto foregroundColor = this->foregroundColor();
        if(foregroundColor)
            CFDictionarySetValue(attributes, kCTForegroundColorAttributeName, foregroundColor->get());
        
        return attributes;
    }
    
#pragma mark - Lifecycle
    
    AttributedString::AttributedString(const String *string, const TextAttributes *attributes) :
        Base(),
        mStorage(CFAttributedStringCreateMutable(kCFAllocatorDefault, 0))
    {
        cf::DictionaryAutoRef attributesDictionary = attributes->copyAttributesDictionary();
        CFAttributedStringReplaceString(mStorage, CFRangeMake(0, CFAttributedStringGetLength(mStorage)), string->getStorage());
        CFAttributedStringSetAttributes(mStorage, CFRangeMake(0, CFAttributedStringGetLength(mStorage)), attributesDictionary, true);
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
    
    TextAttributes *AttributedString::attributesAt(Index locationInString)
    {
        gfx_assert(locationInString < length(), str("Out of bounds access"));
        
        auto attributes = CFAttributedStringGetAttributes(get(), locationInString, NULL);
        return make<TextAttributes>(attributes);
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
                renderSize.width = std::max<CGFloat>(renderSize.width, CTLineGetTypographicBounds(line, NULL, NULL, NULL));
                
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
    
    static CGFloat TextAlignmentToFlush(TextAlignment alignment) {
        switch (alignment) {
            case TextAlignment::Left:
                return 0.0;
            case TextAlignment::Center:
                return 0.5;
            case TextAlignment::Right:
                return 1.0;
        }
    }
    
#pragma mark -
    
    Size AttributedString::size(LineBreakMode lineBreakMode, Size constraintSize)
    {
        CGSize renderSize = {};
        cf::ArrayAutoRef unusedLines = CreateLinesForString(get(), lineBreakMode, constraintSize, &renderSize);
        return { std::min(renderSize.width, constraintSize.width), std::min(renderSize.height, constraintSize.height) };
    }
    
    Size AttributedString::drawAtPoint(Point point, Float maximumWidth, LineBreakMode lineBreakMode, TextAlignment alignment)
    {
        CTFontRef font = (CTFontRef)CFAttributedStringGetAttribute(get(), 0, kCTFontAttributeName, NULL);
        gfx_assert(font, str("cannot render text without a font."));
        
        return drawInRect(Rect{{point.x, point.y}, {maximumWidth, xCTFontGetLineHeight(font)}}, lineBreakMode, alignment);
    }
    
    Size AttributedString::drawInRect(Rect rect, LineBreakMode lineBreakMode, TextAlignment alignment)
    {
        AutoreleasePool pool;
        
        CGSize renderSize = {};
        cf::ArrayAutoRef lines = CreateLinesForString(get(), lineBreakMode, rect.size, &renderSize);
        
        auto font = (CTFontRef)CFAttributedStringGetAttribute(get(), 0, kCTFontAttributeName, NULL);
        gfx_assert(font, str("cannot render text without a font."));
        
        Context::currentContext()->transaction([rect, lines, font, alignment](Context *ctx) {
            CGContextClipToRect(ctx->get(), rect);
            CGContextTranslateCTM(ctx->get(), rect.getMinX(), rect.getMinY() + CTFontGetAscent(font));
            CGContextSetTextMatrix(ctx->get(), ctx->currentTransformationMatrix());
            
            CGFloat lineOffset = 0.0;
            CGFloat lineHeight = xCTFontGetLineHeight(font);
            CGFloat flush = TextAlignmentToFlush(alignment);
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
        
        auto attributes = make<TextAttributes>();
        attributes->setFont(font);
        attributes->setForegroundColor(color);
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
    
    static void text_drawIn(StackFrame *stack)
    {
        /* text vec -- */
        auto rect = VectorToRect(stack->popType<Array<Base>>());
        auto text = stack->popType<AttributedString>();
        
        text->drawInRect(rect);
    }
    
#pragma mark -
    
    void AttributedString::addTo(gfx::StackFrame *frame)
    {
        frame->createFunctionBinding(str("text"), &text_make);
        frame->createFunctionBinding(str("text/size"), &text_size);
        frame->createFunctionBinding(str("text/draw-at"), &text_drawAt);
        frame->createFunctionBinding(str("text/draw-in"), &text_drawIn);
    }
}
