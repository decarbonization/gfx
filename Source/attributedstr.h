//
//  attributedstr.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__attributedstr__
#define __gfx__attributedstr__

#include "base.h"

namespace gfx {
    class String;
    class StackFrame;
    class Font;
    class Color;
    
    ///The different ways text can be modified when it is drawn into a rectangle too small to contain it.
    enum class LineBreakMode {
        ///The text will wrap at the word boundary.
        WordWrap,
        
        ///The text will wrap at the character boundary.
        CharWrap,
        
        ///The text will be truncated at the beginning.
        TruncateHead,
        
        ///The text will be truncated in the middle.
        TruncateMiddle,
        
        ///The text will be truncated at the end.
        TruncateTail,
    };
    
    ///The different alignments that can be applied to text.
    enum class TextAlignment {
        ///Align the text to the left.
        Left,
        
        ///Center the text.
        Center,
        
        ///Align the text to the right.
        Right,
    };
    
#pragma mark -
    
    ///The TextAttributes class encapsulates all of the possible attributes that
    ///can be applied to an `gfx::AttributedString` object when it is rendered.
    class TextAttributes : public Base
    {
        ///The font of the attributes.
        Font *mFont;
        
        ///The foreground color of the attributes.
        Color *mForegroundColor;
        
    public:
        
        ///Constructs an empty text attributes object.
        TextAttributes();
        
        ///Constructs a text attributes object by copying a given text attributes object.
        ///
        /// \param  attributes  The attributes to copy. Should not be null.
        ///
        TextAttributes(const TextAttributes *attributes);
        
        ///Constructs a text attributes object by copying the contents of a native attributes container.
        TextAttributes(CFDictionaryRef dictionary);
        
        ///The destructor.
        ~TextAttributes();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const TextAttributes *other) const;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
#pragma mark - Attributes
        
        ///Sets the font. May not be null.
        virtual void setFont(Font *font);
        
        ///Returns the font.
        virtual Font *font() const;
        
#pragma mark -
        
        ///Sets the foreground color.
        virtual void setForegroundColor(Color *color);
        
        ///Returns the foreground color.
        virtual Color *foregroundColor() const;
        
#pragma mark -
        
    private:
        
        ///Creates a new attributes dictionary using the contents of the receiver, returning it.
        virtual CFDictionaryRef copyAttributesDictionary() const;
        
        ///For `gfx::TextAttributes::copyAttributesDictionary`.
        friend class AttributedString;
    };
    
#pragma mark -
    
    ///The AttributedString class encapsulates a string and collections of dictionaries
    ///containing attributes for ranges of said string. It is one of the core classes
    ///forming the basis of text in the Gfx graphics stack.
    ///
    /// \discussion This class is not as efficient as it could be. Specifically, it would
    ///             probably make sense to cache layout information across drawing operations.
    ///
    class AttributedString : public Base
    {
    public:
        
        ///The native type backing the AttributedString class.
        typedef CFMutableAttributedStringRef NativeType;
        
        ///The const variant of the native type backing the AttributedString class.
        typedef CFAttributedStringRef ConstNativeType;
        
    protected:
        
        ///The backing storage of the attributed string.
        NativeType mStorage;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Construct an attributed string with a given string and dictionary of attributes.
        ///
        /// \param  string      The string. Should not be null.
        /// \param  attributes  The attributes to apply to the full string. May be null.
        ///
        AttributedString(const String *string, const TextAttributes *attributes);
        
        ///Constructs an attributed string by copying the contents of another.
        ///
        /// \param  other   The other attributed string to copy. Should not be null.
        ///
        AttributedString(const AttributedString *other);
        
        ///The destructor.
        virtual ~AttributedString();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const AttributedString *other) const;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
#pragma mark - Storage
        
        ///Returns the underlying storage of the attributed string.
        NativeType get();
        
        ///Returns the underlying storage of the attributed string.
        ConstNativeType get() const;
        
#pragma mark - Introspection
        
        ///Returns the length of the attributed strings contents.
        virtual Index length() const;
        
        ///Returns the string of the attributed string.
        virtual const String *string() const;
        
#pragma mark - Attributes
        
        ///Returns the attributes at a specified location in the string.
        ///
        /// \param  locationInString    The location within the string to look for the attribute at.
        ///                             Must be within {0, length()}.
        /// \param  attributeName       The name of the attribute to lookup. May not be null.
        ///
        /// \result The attributes at the given location, or null if there were none.
        ///
        virtual TextAttributes *attributesAt(Index locationInString);
        
#pragma mark - Drawing
        
        ///Returns the estimated size for the attributed string.
        ///
        /// \param  lineBreakMode   The line break mode to apply to the text.
        /// \param  constraintSize  The maximum size the attributed string will be rendered within.
        ///                         Default value is `{GFX_FLOAT_MAX, GFX_FLOAT_MAX}`.
        ///
        /// \result The bounding box the attributed string uses when drawn.
        ///
        virtual Size size(LineBreakMode lineBreakMode,
                          Size constraintSize = {GFX_FLOAT_MAX, GFX_FLOAT_MAX});
        
        ///Draws the contents of the attributed string into the current context,
        ///positioned according to a given coordinate point, constrained to a
        ///given width.
        ///
        /// \param  point           The location to draw the text at.
        /// \param  maximumWidth    The maximum width the text may be. Default value is `GFX_FLOAT_MAX`.
        /// \param  lineBreakMode   Default value is `gfx::LineBreakMode::WordWrap`.
        /// \param  alignment       Default value is `gfx::TextAlignment::Left`.
        ///
        /// \result The size of the rendered text.
        ///
        virtual Size drawAtPoint(Point point,
                                 Float maximumWidth = GFX_FLOAT_MAX,
                                 LineBreakMode lineBreakMode = LineBreakMode::WordWrap,
                                 TextAlignment alignment = TextAlignment::Left);
        
        ///Draws the contents of the attributed string into the current context,
        ///positioned and constrained within a given rectangle.
        ///
        /// \param  rect            The bounding box of the text.
        /// \param  lineBreakMode   Default value is `gfx::LineBreakMode::WordWrap`.
        /// \param  alignment       Default value is `gfx::TextAlignment::Left`.
        ///
        /// \result The size of the rendered text.
        ///
        virtual Size drawInRect(Rect rect,
                                LineBreakMode lineBreakMode = LineBreakMode::WordWrap,
                                TextAlignment alignment = TextAlignment::Left);
        
#pragma mark - Functions
        
        ///Adds the text function suite to a given stack frame.
        static void AddTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__attributedstr__) */
