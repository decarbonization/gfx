//
//  attributedstr.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__attributedstr__
#define __gfx__attributedstr__

#include "dictionary.h"

namespace gfx {
    class String;
    class StackFrame;
    
    ///The font to use when rendering a string in a TextLine.
    ///Must be an instance of `gfx::Font` or an exception is raised.
    extern const String *const kFontAttributeName;
    
    ///The foreground color to use when rendering a string in a TextLine.
    ///Must be an instance of `gfx::Color` or an exception is raised.
    extern const String *const kForegroundColorAttributeName;
    
    enum class LineBreakMode {
        WordWrap,
        CharWrap,
        TruncateHead,
        TruncateMiddle,
        TruncateTail,
    };
    
    enum class Alignment {
        Left,
        Center,
        Right,
    };
    
    ///The AttributedString class encapsulates a string and collections of dictionaries
    ///containing attributes for ranges of said string. It is one of the core classes
    ///forming the basis of text in the Gfx graphics stack.
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
        AttributedString(const String *string, const Dictionary<const String, Base> *attributes);
        
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
        
        ///Returns the value associated with a given attributed at a specified location in the string.
        ///
        /// \param  locationInString    The location within the string to look for the attribute at.
        ///                             Must be within {0, length()}.
        /// \param  attributeName       The name of the attribute to lookup. May not be null.
        ///
        /// \result The value for the attribute, or null if there was none.
        ///
        virtual Base *attributeValue(Index locationInString, const String *attributeName);
        
#pragma mark - Drawing
        
        ///Returns the estimated size for the attributed string.
        ///
        /// \param  lineBreakMode   The line break mode to apply to the text.
        /// \param  constraintSize  The maximum size the attributed string will be rendered within.
        ///
        /// \result The bounding box the attributed string uses when drawn.
        ///
        Size size(LineBreakMode lineBreakMode,
                  Size constraintSize = {GFX_FLOAT_MAX, GFX_FLOAT_MAX}) const;
        
        Size drawAtPoint(Point point,
                         Float maximumWidth = GFX_FLOAT_MAX,
                         LineBreakMode lineBreakMode = LineBreakMode::WordWrap,
                         Alignment alignment = Alignment::Left);
        
        Size drawInRect(Rect rect,
                        LineBreakMode lineBreakMode = LineBreakMode::WordWrap,
                        Alignment alignment = Alignment::Left);
        
#pragma mark - Functions
        
        ///Adds the text function suite to a given stack frame.
        static void AddTo(StackFrame *frame);
    };
}

#endif /* defined(__gfx__attributedstr__) */
