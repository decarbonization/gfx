//
//  json.h
//  gfx
//
//  Created by Kevin MacWhinnie on 1/27/14.
//  Copyright (c) 2014 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__json__
#define __gfx__json__

#include "base.h"
#include "offset.h"

#include "dictionary.h"
#include "array.h"
#include <functional>

namespace gfx {
    class String;
    
    namespace json {
        class Reader : public Base
        {
            ///A predicate function used in simple parsing rules.
            typedef std::function<bool(UniChar c, bool isFirstCharacter)> Predicate;
            
            
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
            
            void fastForward(Index amount);
            
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
            
            String *borrow(Index amount);
            
            ///Returns a bool indicating whether or not there is more content available.
            bool more();
            
            
            ///Moves the parser to the next occurrance of a given
            ///character, raising if the end of the string is reached.
            void moveToNext(UniChar c);
            
            void skipWhitespace();
            
            ///Moves the parser forward, collecting each character into a string,
            ///until a given predicate functor returns false, returning the string.
            ///
            /// \param  predicate   A function that will read a given character and return a bool
            ///                     indicating whether or not the accumulation should continue.
            ///
            /// \result The characters accumulated while `predicate` returned true.
            ///
            String *accumulateWhile(Predicate predicate);
            
        public:
            
            Reader(const String *source);
            ~Reader();
            
#pragma mark - Parsing
            
        protected:
            
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
            
            Number *parseNumber();
            String *parseString();
            Dictionary<Base, Base> *parseObject();
            Array<Base> *parseArray();
            Base *parseExpression();
            
        public:
            
            Base *parse();
        };
    }
}

#endif /* defined(__gfx__json__) */
