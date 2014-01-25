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
#include "annotation.h"

#include "papertape.h"

namespace gfx {
    
#pragma mark - Tools
    
    enum Tokens : UniChar {
        kCommentAnnotationBegin = '(',
        kCommentAnnotationEnd = ')',
        kCommentMarker = '*',
        kAnnotationMarker = '%',
        
        kHashMarker = '#',
        kHashBegin = '(',
        kHashEnd = ')',
        
        kVectorBegin = '[',
        kVectorEnd = ']',
        
        kFunctionBegin = '{',
        kFunctionEnd = '}',
        
        kWordApplyBegin = '(',
        kWordApplyEnd = ')',
        
        kStringBegin = '"',
        kStringEnd = '"',
        kStringEscapeSigil = '\\',
        
        kDecimalSeparator = '.',
        kNumberDivider = '_',
        kPercentageMarker = '%',
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
                c != kVectorEnd &&
                c != kWordApplyBegin &&
                c != kWordApplyEnd);
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
    
    static bool is_function(UniChar c)
    {
        return (c == kFunctionBegin);
    }
    
#pragma mark - Lifecycle
    
    Parser::Parser(const String *string) :
        Base(),
        mString(retained(string)),
        mCurrentIndex(0),
        mOffset{0, 0}
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
        mOffset.column++;
        
        UniChar current = this->current();
        if(is_newline(current)) {
            mOffset.line++;
            mOffset.column = 0;
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
        if(offset >= 0 && offset < mString->length())
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
    
    String *Parser::accumulateWhile(Predicate predicate)
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
        auto userInfo = autoreleased(new Dictionary<const String, Base>{
            { kUserInfoKeyOffsetLine, make<Number>(mOffset.line) },
            { kUserInfoKeyOffsetLine, make<Number>(mOffset.column) },
        });
        throw ParsingException(reason, userInfo);
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
                        fail(str("unexpected escape character"));
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
        auto number = make<Number>(numberString->doubleValue());
        if(this->current() == kPercentageMarker) {
            number = make<Number>(number->value() / 100.0);
            this->next();
        }
        
        return number;
    }
    
    void Parser::parseComment()
    {
        // (* this is a comment *)
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // ^ <- Parsing starts here.
        
        next(); //(
        
        Index nestedParenCount = 0;
        while (more() && next() != 0) {
            UniChar c = current();
            if(c == kCommentAnnotationBegin) {
                nestedParenCount++;
                continue;
            } else if(c == kCommentAnnotationEnd) {
                if(nestedParenCount > 0)
                    nestedParenCount--;
                else if(peek(-1) == kCommentMarker)
                    break;
            }
        }
        
        requireCondition(this->current() == kCommentAnnotationEnd, str("expected ')', found end of file."));
        
        next(); //)
    }
    
    Annotation *Parser::parseAnnotation()
    {
        // (% this is an annotation %)
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // ^ <- Parsing starts here.
        
        next(); //(
        next(); //%
        
        const String *contents = accumulateWhile([](UniChar c, bool isFirst) { return (c != kAnnotationMarker); });
        
        next(); //%
        next(); //)
        
        return make<Annotation>(mOffset, contents);
    }
    
    Array<Base> *Parser::accumulateSubexpressions(UniChar start, UniChar end)
    {
        auto accumulator = make<Array<Base>>();
        while (this->more() && this->next() != end) {
            if(is_whitespace(current()))
                continue;
            
            if(this->parseExpression(accumulator))
                this->previous();
        }
        
        requireCondition(this->current() == end, (String::Builder() << "expected '" << end << "', found end of file."));
        
        this->next();
        
        return accumulator;
    }
    
    Expression *Parser::parseSubexpression(Expression::Type type, UniChar start, UniChar end)
    {
        Offset offset(mOffset);
        auto exprs = this->accumulateSubexpressions(start, end);
        return make<Expression>(offset, type, exprs);
    }
    
#pragma mark - Parsing
    
    bool Parser::parseExpression(Array<Base> *exprAccumulator)
    {
        Base *result = nullptr;
        while (this->more()) {
            UniChar c = this->current();
            
            if(is_whitespace(c)) {
                this->next();
                continue;
            } else if(c == kCommentAnnotationBegin && peek(1) == kAnnotationMarker) {
                    result = this->parseAnnotation();
            } else if(c == kCommentAnnotationBegin && peek(1) == kCommentMarker) {
                this->parseComment();
                continue;
            } else if(c == kWordApplyBegin) {
                Index i = -1;
                for (;;) {
                    auto c = peek(i);
                    if(c == 0) {
                        break;
                    } else if(is_whitespace(c)) {
                        i--;
                        continue;
                    } else if(is_word(c, false)) {
                        fail(str("Unexpected whitespace between parentheses and word."));
                        break;
                    }
                }
                
                fail(str("Expressions enclosed in free-standing parentheses are not supported."));
            } else if(c == kHashMarker && peek(1) == kHashBegin) {
                next(); // kHashMarker
                result = this->parseSubexpression(Expression::Type::Hash, kHashBegin, kHashEnd);
            } else if(is_vector(c)) {
                result = this->parseSubexpression(Expression::Type::Vector, kVectorBegin, kVectorEnd);
            } else if(is_function(c)) {
                result = this->parseSubexpression(Expression::Type::Function, kFunctionBegin, kFunctionEnd);
            } else if(is_string(c, true)) {
                result = this->parseString();
            } else if(is_word(c, true)) {
                auto word = this->parseWord();
                
                if(this->current() == kWordApplyBegin) {
                    if(this->peek(1) == kWordApplyEnd) {
                        this->next(); // (
                        this->next(); // )
                    } else {
                        auto exprs = this->accumulateSubexpressions(kWordApplyBegin, kWordApplyEnd);
                        exprAccumulator->appendArray(exprs);
                    }
                    
                    if(this->current() == kFunctionBegin || this->peek(1) == kFunctionBegin) {
                        this->moveToNext(kFunctionBegin);
                        exprAccumulator->append(this->parseSubexpression(Expression::Type::Function, kFunctionBegin, kFunctionEnd));
                    }
                    
                    exprAccumulator->append(word);
                    return true;
                } else {
                    result = word;
                }
            } else if(is_number(c, true)) {
                result = this->parseNumber();
            } else {
                fail((String::Builder() << "unexpected character " << c));
            }
            
            break;
        }
        
        if(result) {
            exprAccumulator->append(result);
            
            return true;
        } else {
            return false;
        }
    }
    
    Array<Base> *Parser::parse()
    {
        auto accumulator = make<Array<Base>>();
        
        while (this->more()) {
            this->parseExpression(accumulator);
        }
        
        return accumulator;
    }
}
