//
//  word.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_word_h
#define gfx_word_h

#include "base.h"
#include "str.h"
#include "offset.h"

namespace gfx {
    ///The Word class encapsulates words as they are defined by the Gfx Forth-derived language.
    class Word : public Base
    {
        ///The underlying string of the word.
        const String *mString;
        
        ///Where the word originated.
        Offset mOffset;
        
    public:
        
        ///Constructs the receiver with a given string, and offset origin.
        Word(const String *inString, Offset offset) :
            mString(retained(inString)),
            mOffset(offset)
        {
        }
        
        ///The destructor.
        virtual ~Word()
        {
            released(mString);
        }
        
        ///Returns the string of the receiver.
        const String *string() const
        {
            return retained_autoreleased(mString);
        }
        
        ///Returns the offset of the receiver.
        Offset offset() const
        {
            return mOffset;
        }
        
#pragma mark - Identity
        
        virtual HashCode hash() const override
        {
            return mString->hash() + 'word';
        }
        
        virtual bool isEqual(const Word *other) const
        {
            if(other)
                return other->string()->isEqual(other->string());
            
            return false;
        }
        
        virtual bool isEqual(const Base *other) const override
        {
            if(!other)
                return false;
            
            if(other->isKindOfClass<Word>()) {
                return this->isEqual((Word *)other);
            }
            
            return false;
        }
        
        virtual const String *description() const override
        {
            return retained_autoreleased(mString);
        }
    };
}

#endif
