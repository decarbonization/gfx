//
//  textline.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/1/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__textline__
#define __gfx__textline__

#include "base.h"
#include "dictionary.h"
#include <CoreText/CoreText.h>

namespace gfx {
    
    ///The font to use when rendering a string in a TextLine.
    ///Must be an instance of `gfx::Font` or an exception is raised.
    extern const String *const kFontAttributeName;
    
    ///The foreground color to use when rendering a string in a TextLine.
    ///Must be an instance of `gfx::Color` or an exception is raised.
    extern const String *const kForegroundColorAttributeName;
    
    ///The TextLine class encapsulates a single line of text.
    class TextLine : public Base
    {
    public:
        
        ///The underlying native type that backs TextLine instances.
        typedef CTLineRef NativeType;
        
    protected:
        
        ///The underlying native object.
        NativeType mLine;
        
    public:
        
#pragma mark - Vending Lines
        
        ///Creates a new autoreleased TextLine object using a given string and dictionary of rendering attributes.
        ///
        /// \param  string      The string to create the line from. Required.
        /// \param  attributes  The rendering attributes to apply to the line. Required.
        ///
        /// \result A new autoreleased TextLine object ready for drawing; null if there was an issue.
        ///
        ///This method should be preferred over available constructors.
        static TextLine *withString(const String *string, const Dictionary<const String, Base> *attributes);
        
#pragma mark - Lifecycle
        
        ///Constructs a new text line with a given native object.
        ///
        /// \param  line    The native object to back the new text line with. Should not be null.
        ///
        ///This constructor generally should not be used directly. To create
        ///a new TextLine, use the `gfx::TextLine::withString` static method.
        TextLine(NativeType line);
        
        ///The destructor.
        ~TextLine();
        
#pragma mark - Introspection
        
        ///Returns the underlying native line object.
        NativeType get() const;
        
        ///Calculates and returns the drawing bounds for the receiver's contents.
        ///
        /// \throws If there is no current context.
        /// \result A rectangle that describes the area of the line; or a null rect if there is a calculation error.
        Rect bounds() const;
        
#pragma mark - Drawing
        
        ///Renders the contents of the line at a specified location in the current coordinate system.
        ///
        /// \param  point   A point in the current coordinate system.
        ///
        void drawAtPoint(Point point);
        
        ///Renders the contents of the line within a specified rectangle in the current coordinate system.
        ///
        /// \param  rect    The rectangle to render the text within.
        ///
        void drawInRect(Rect rect);
    };
}

#endif /* defined(__gfx__textline__) */
