//
//  parser.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/13/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "parser.h"
#include "expression.h"
#include "word.h"
#include "number.h"

namespace gfx {
    
#pragma mark - Tools
    
    enum Tokens : UniChar {
        kCommentBegin = '(',
        kCommentEnd = ')',
        
        kVectorBegin = '[',
        kVectorEnd = ']',
        
        kFunctionBegin = '{',
        kFunctionEnd = '}',
        
        kStringBegin = '"',
        kStringEnd = '"',
        kStringEscapeSigil = '\\',
        
        kDecimalSeparator = '.',
        kNumberDivider = '_',
    };
    
    static bool is_newline(UniChar c)
    {
        return (c == '\n' || c == '\r');
    }
    
    static bool is_whitespace(UniChar c)
    {
        return (c == ' ' || c == '\t' || is_newline(c));
    }
    
    static bool is_word(UniChar c, bool isFirstCharacter)
    {
        return (!(isFirstCharacter && isnumber(c)) &&
                !is_whitespace(c) &&
                c != kFunctionBegin &&
                c != kFunctionEnd &&
                c != kVectorBegin &&
                c != kVectorEnd);
    }
    
    static bool is_number(UniChar c, bool isFirstCharacter)
    {
        return (isnumber(c) || (!isFirstCharacter && (c == kDecimalSeparator || c == kNumberDivider)));
    }
    
    static bool is_string(UniChar c, bool isFirstCharacter)
    {
        return (isFirstCharacter && c == kStringBegin);
    }
    
    static bool is_vector(UniChar c)
    {
        return (c == kVectorBegin);
    }
    
    static bool is_comment(UniChar c)
    {
        return (c == kCommentBegin);
    }
    
    static bool is_function(UniChar c)
    {
        return (c == kFunctionBegin);
    }
    
#pragma mark - Lifecycle
    
    Parser::Parser(const String *string) :
        Base(),
        mString(retained(string)),
        mCurrentIndex(0),
        mOffset{1, 0}
    {
        
    }
    
    Parser::~Parser()
    {
        mString->release();
    }
    
#pragma mark - Movement
    
    UniChar Parser::previous()
    {
        if(mCurrentIndex == 0)
            return -1;
        
        mCurrentIndex--;
        
        return this->current();
    }
    
    UniChar Parser::next()
    {
        mCurrentIndex++;
        mOffset.offset++;
        
        UniChar current = this->current();
        if(is_newline(current)) {
            mOffset.line++;
            mOffset.offset = 0;
        }
        
        return current;
    }
    
    UniChar Parser::current()
    {
        return this->peek(0);
    }
    
    UniChar Parser::peek(Index delta)
    {
        Index offset = mCurrentIndex + delta;
        if(offset < mString->length())
            return mString->at(offset);
        else
            return 0;
    }
    
    bool Parser::more()
    {
        return (mCurrentIndex < mString->length());
    }
    
    void Parser::moveToNext(UniChar c)
    {
        while (this->more() && this->current() != c) {
            this->next();
        }
        
        requireCondition(this->current() == c, (String::Builder() << "expected '" << c << "', found end of file."));
    }
    
    const String *Parser::accumulateWhile(std::function<bool(UniChar c, bool isFirstCharacter)> predicate)
    {
        String::Builder accumulator;
        
        bool isFirstCharacter = true;
        while (this->more() && predicate(this->current(), isFirstCharacter)) {
            accumulator << this->current();
            
            isFirstCharacter = false;
            this->next();
        }
        
        return accumulator;
    }
    
#pragma mark - Parsers
    
    void Parser::requireCondition(bool condition, const String *reason)
    {
        if(!condition) fail(reason);
    }
    
    void Parser::fail(const String *reason)
    {
        throw ParsingException(reason, nullptr);
    }
    
    Word *Parser::parseWord()
    {
        Offset offset = mOffset;
        return make<Word>(this->accumulateWhile(&is_word), offset);
    }
    
    String *Parser::parseString()
    {
        String::Builder accumulator;
        
        while (this->more() && this->next() != kStringEnd) {
            if(this->current() == kStringEscapeSigil) {
                switch (this->next()) {
                    case 'a':
                        accumulator << "\a";
                        break;
                        
                    case 'b':
                        accumulator << "\b";
                        break;
                        
                    case 'f':
                        accumulator << "\f";
                        break;
                        
                    case 'n':
                        accumulator << "\n";
                        break;
                        
                    case 'r':
                        accumulator << "\r";
                        break;
                        
                    case 't':
                        accumulator << "\t";
                        break;
                        
                    case 'v':
                        accumulator << "\v";
                        break;
                        
                    case '\'':
                        accumulator << "\'";
                        break;
                        
                    case '"':
                        accumulator << "\"";
                        break;
                        
                    case '\\':
                        accumulator << "\\";
                        break;
                        
                    case '?':
                        accumulator << "\?";
                        break;
                        
                    case '%':
                        accumulator << "%";
                        break;
                        
                    default:
                        fail("unexpected escape character"_gfx);
                        break;
                }
            } else {
                accumulator << this->current();
            }
        }
        
        this->next();
        
        return accumulator;
    }
    
    Number *Parser::parseNumber()
    {
        auto numberString = this->accumulateWhile(&is_number);
        return make<Number>(numberString->doubleValue());
    }
    
    Expression *Parser::parseSubexpression(Expression::Type type, ParserMode parserMode, UniChar start, UniChar end)
    {
        Offset offset = mOffset;
        auto accumulator = make<Array<Base>>();
        while (this->more() && this->next() != end) {
            if(is_whitespace(current()))
                continue;
            
            auto expression = this->parseExpression(parserMode);
            if(expression) {
                accumulator->append(expression);
                this->previous();
            }
        }
        
        requireCondition(this->current() == end, (String::Builder() << "expected '" << end << "', found end of file."));
        
        this->next();
        
        return make<Expression>(offset, type, accumulator);
    }
    
#pragma mark - Parsing
    
    Base *Parser::parseExpression(Parser::ParserMode mode)
    {
        while (this->more()) {
            UniChar c = this->current();
            
            if(is_whitespace(c)) {
                this->next();
            } else if(is_comment(c)) {
                this->moveToNext(kCommentEnd);
                this->next();
            } else if(is_vector(c)) {
                return this->parseSubexpression(Expression::Type::Vector, ParserMode::Vector, kVectorBegin, kVectorEnd);
            } else if(is_function(c)) {
                return this->parseSubexpression(Expression::Type::Function, ParserMode::Function, kFunctionBegin, kFunctionEnd);
            } else if(is_string(c, true)) {
                return this->parseString();
            } else if(is_word(c, true)) {
                return this->parseWord();
            } else if(is_number(c, true)) {
                return this->parseNumber();
            } else {
                fail((String::Builder() << "unexpected character " << c));
            }
        }
        
        return nullptr;
    }
    
    Array<Base> *Parser::parse()
    {
        auto expressionStack = make<Array<Base>>();
        
        while (this->more()) {
            auto expression = this->parseExpression(ParserMode::Normal);
            expressionStack->append(expression);
        }
        
        return expressionStack;
    }
}
