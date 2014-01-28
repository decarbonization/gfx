//
//  json.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 1/27/14.
//  Copyright (c) 2014 Roundabout Software, LLC. All rights reserved.
//

#include "json.h"
#include "exception.h"
#include "null.h"

namespace gfx {
    namespace json {
        
        enum Tokens : UniChar
        {
            kObjectBegin = '{',
            kObjectEnd = '}',
            kObjectKeyValueSeparator = ':',
            
            kArrayBegin = '[',
            kArrayEnd = ']',
            
            kStatementSeparator = ',',
            
            kStringBegin = '"',
            kStringEnd = '"',
            kStringEscape = '\\',
        };
        
        static const String *gTrueString = new String("true");
        static const String *gFalseString = new String("false");
        static const String *gNullString = new String("null");
    
#pragma mark - Predicates
        
        static bool is_newline(UniChar c)
        {
            return (c == '\n' || c == '\r');
        }
        
        static bool is_whitespace(UniChar c)
        {
            return (c == ' ' || c == '\t' || is_newline(c));
        }
        
        static bool is_number(UniChar c, bool isFirstCharacter)
        {
            return (isnumber(c) || (!isFirstCharacter && c == '.'));
        }
        
#pragma mark - Lifecycle
        
        Reader::Reader(const String *string) :
            Base(),
            mString(retained(string)),
            mCurrentIndex(0),
            mOffset{0, 0}
        {
            
        }
        
        Reader::~Reader()
        {
            mString->release();
        }
        
#pragma mark - Movement
        
        UniChar Reader::previous()
        {
            if(mCurrentIndex == 0)
                return -1;
            
            mCurrentIndex--;
            
            return this->current();
        }
        
        UniChar Reader::next()
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
        
        void Reader::fastForward(Index amount)
        {
            for (Index counter = amount; counter >= 0; counter--)
                next();
        }
        
        UniChar Reader::current()
        {
            return this->peek(0);
        }
        
        UniChar Reader::peek(Index delta)
        {
            Index offset = mCurrentIndex + delta;
            if(offset >= 0 && offset < mString->length())
                return mString->at(offset);
            else
                return 0;
        }
        
        String *Reader::borrow(Index amount)
        {
            auto subrange = Range(mCurrentIndex, amount);
            if(amount > 0 && subrange.max() < mString->length()) {
                auto substring = mString->substring(subrange);
                return substring;
            } else {
                return str("");
            }
        }
        
        bool Reader::more()
        {
            return (mCurrentIndex < mString->length());
        }
        
        void Reader::moveToNext(UniChar c)
        {
            while (this->more() && this->current() != c) {
                this->next();
            }
            
            requireCondition(this->current() == c, (String::Builder() << "expected '" << c << "', found end of file."));
        }
        
        void Reader::skipWhitespace()
        {
            while (is_whitespace(current())) {
                next();
            }
        }
        
        String *Reader::accumulateWhile(Predicate predicate)
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
        
#pragma mark - Parsing
        
        void Reader::requireCondition(bool condition, const String *reason)
        {
            if(!condition) fail(reason);
        }
        
        void Reader::fail(const String *reason)
        {
            auto userInfo = autoreleased(new Dictionary<const String, Base>{
                { kUserInfoKeyOffsetLine, make<Number>(mOffset.line) },
                { kUserInfoKeyOffsetLine, make<Number>(mOffset.column) },
            });
            throw Exception(reason, userInfo);
        }
        
#pragma mark -
        
        String *Reader::parseString()
        {
            String::Builder accumulator;
            
            while (this->more() && this->next() != kStringEnd) {
                if(this->current() == kStringEscape) {
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
        
        Number *Reader::parseNumber()
        {
            auto numberString = this->accumulateWhile(&is_number);
            return make<Number>(numberString->doubleValue());
        }
        
        Dictionary<Base, Base> *Reader::parseObject()
        {
            next(); // {
            
            auto object = make<Dictionary<Base, Base>>();
            
            while (this->more() && current() != kObjectEnd) {
                skipWhitespace();
                
                auto key = parseExpression();
                skipWhitespace();
                if(current() == kObjectKeyValueSeparator) {
                    next();
                    skipWhitespace();
                } else {
                    fail(String::Builder() << "expected ':', got '" << current()  << "'.");
                }
                
                auto value = parseExpression();
                
                object->set(key, value);
                
                skipWhitespace();
                if(current() == kStatementSeparator)
                    next();
                skipWhitespace();
            }
            
            requireCondition(more(), str("expected '}', got end of file."));
            requireCondition(current() == kObjectEnd, (String::Builder() << "expected '}', got '" << current() << "'."));
            
            next(); // }
            
            return object;
        }
        
        Array<Base> *Reader::parseArray()
        {
            next(); // [
            
            auto array = make<Array<Base>>();
            
            while (this->more() && current() != kArrayEnd) {
                auto value = parseExpression();
                array->append(value);
                
                skipWhitespace();
                if(current() == kStatementSeparator)
                    next();
                skipWhitespace();
            }
            
            requireCondition(more(), str("expected ']', got end of file."));
            requireCondition(current() == kArrayEnd, (String::Builder() << "expected ']', got '" << current() << "'."));
            
            next(); // ]
            
            return array;
        }
        
        Base *Reader::parseExpression()
        {
            skipWhitespace();
            
            auto c = current();
            if(c == kObjectBegin) {
                return parseObject();
            } else if(c == kArrayBegin) {
                return parseArray();
            } else if(c == kStringBegin) {
                return parseString();
            } else if(is_number(c, true)) {
                return parseNumber();
            } else if(borrow(gTrueString->length())->isEqual(gTrueString)) {
                fastForward(gTrueString->length() - 1);
                return Number::True();
            } else if(borrow(gFalseString->length())->isEqual(gFalseString)) {
                fastForward(gFalseString->length() - 1);
                return Number::False();
            } else if(borrow(gNullString->length())->isEqual(gNullString)) {
                fastForward(gNullString->length() - 1);
                return Null::shared();
            } else {
                fail(String::Builder() << "unexpected '" << c << "'.");
            }
            
            return nullptr;
        }
        
#pragma mar -
        
        Base *Reader::parse()
        {
            auto result = parseExpression();
            requireCondition(!more(), str("expected end of file"));
            return result;
        }
    }
}
