//
//  parser.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/13/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__parser__
#define __gfx__parser__

#include "base.h"
#include "array.h"
#include "exception.h"
#include "offset.h"
#include "expression.h"

namespace gfx {
    class String;
    class Number;
    class Word;
    class Annotation;
    
    ///The Parser class encapsulates a recursive descent parser that converts a string
    ///into instances of `gfx::Expression`, `gfx::Word`, `gfx::Number`, and `gfx::String`.
    ///The resulting objects are then executed by the `gfx::Interpreter` class.
    ///
    ///Parsers are one use, and should be stack allocated.
    class Parser : public Base
    {
        ///The string being parsed.
        const String *mString;
        
        ///The index within the string the parser is currently operating on.
        Index mCurrentIndex;
        
        ///The offset (line, column) within the string.
        Offset mOffset;
        
#pragma mark - Movement
        
        ///Moves the parser to the previous character, returning it.
        UniChar previous();
        
        ///Moves the parser to the next character, returning it.
        UniChar next();
        
        ///Returns the current character.
        ///
        /// \result A character, or 0 if the parser is outside the bounds of its string.
        ///
        UniChar current();
        
        ///Returns the character offset by a given delta from the current character.
        ///
        /// \param  delta   The amount to look before or after the current character.
        ///
        /// \result A character, or 0 if the parser is outside the bounds of its string.
        ///
        UniChar peek(Index delta);
        
        ///Returns a bool indicating whether or not there is more content available.
        bool more();
        
        
        ///Moves the parser to the next occurrance of a given
        ///character, raising if the end of the string is reached.
        void moveToNext(UniChar c);
        
        ///Moves the parser forward, collecting each character into a string,
        ///until a given predicate functor returns false, returning the string.
        ///
        /// \param  predicate   A function that will read a given character and return a bool
        ///                     indicating whether or not the accumulation should continue.
        ///
        /// \result The characters accumulated while `predicate` returned true.
        ///
        const String *accumulateWhile(std::function<bool(UniChar c, bool isFirstCharacter)> predicate);
        
#pragma mark - Parsers
        
        ///Asserts that a given condition is true, throwing a `gfx::Parser::ParsingException`
        ///with a given reason if it is false.
        ///
        /// \param  condition   The condition that must be true.
        /// \param  reason      The reason the condition being false is a failure.
        ///
        void requireCondition(bool condition, const String *reason);
        
        ///Raises a `gfx::Parser::ParsingException` with a given reason.
        void fail(const String *reason);
        
#pragma mark -
        
        ///Parses a word at the current parser position, returning it.
        Word *parseWord();
        
        ///Parses a quoted string at the current parser position, returning it.
        String *parseString();
        
        ///Parses a number at the current parser position, returning it.
        Number *parseNumber();
        
        ///Parses a comment, disguarding its contents.
        void parseComment();
        
        ///Parses an annotation at the current parser position, returning it.
        Annotation *parseAnnotation();
        
        ///Parses a subexpression at the current position.
        ///
        /// \param  type    The type of expression to parse.
        /// \param  start   The opening character of the subexpression, if applicable.
        /// \param  end     The closing character of the subexpression, if applicable.
        Base *parseSubexpression(Expression::Type type, UniChar start, UniChar end);
        
        ///Parses a single expression object, returning it.
        ///
        /// \param  exprStack   Accumulator for all expressions parsed.
        /// \param  terminator  A character to terminate expression parsing when encountered. Default is 0.
        bool parseExpression(Array<Base> *exprStack, UniChar terminator = 0);
        
    public:
        ///Constructs the parser with a given source string.
        ///
        /// \result string  The source string to parse. Should not be null.
        ///
        explicit Parser(const String *string);
        
        ///The destructor.
        ~Parser();
        
        ///Parses the contents of the parser's string into an array of expressions.
        ///
        /// \throws `gfx::Parser::ParsingException` upon parsing errors.
        Array<Base> *parse();
        
#pragma mark - Errors
        
        class ParsingException : public Exception
        {
        public:
            using Exception::Exception;
        };
    };
}

#endif /* defined(__gfx__parser__) */
