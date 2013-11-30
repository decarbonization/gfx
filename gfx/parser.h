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
    
    class Parser : public Base
    {
        const String *mString;
        Index mCurrentIndex;
        Offset mOffset;
        
#pragma mark - Movement
        
        UniChar previous();
        UniChar next();
        UniChar current();
        UniChar peek(Index delta);
        bool more();
        
        void moveToNext(UniChar c);
        const String *accumulateWhile(std::function<bool(UniChar c, bool isFirstCharacter)> predicate);
        
#pragma mark - Parsers
        
        enum class ParserMode {
            Normal,
            Vector,
            Function,
        };
        
        static void require(bool condition, const String *reason);
        static void fail(const String *reason);
        
        Word *parseWord();
        String *parseString();
        Number *parseNumber();
        Expression *parseSubexpression(Expression::Type type, ParserMode parserMode, UniChar start, UniChar end);
        Base *parseExpression(Parser::ParserMode mode);
        
    public:
        explicit Parser(const String *string);
        ~Parser();
        
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
