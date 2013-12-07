//
//  annotation.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/6/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_annotation_h
#define gfx_annotation_h

#include "base.h"
#include "str.h"
#include "offset.h"

namespace gfx {
    ///The Annotation class encapsulates the raw data from
    ///occurrances of annotation comments in Gfx source code.
    class Annotation : public Base
    {
        ///The contents of the annotation.
        const String *mContents;
        
        ///The origin of the contents.
        Offset mOffset;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Constructs an annotation with a given string.
        ///
        /// \param  string  The string of the annotation. Should not be nil.
        ///
        explicit Annotation(Offset offset, const String *string) :
            mOffset(offset),
            mContents(retained(string))
        {
        }
        
        ///The destructor.
        virtual ~Annotation()
        {
            released(mContents);
        }
        
#pragma mark - Identity
        
        virtual HashCode hash() const override
        {
            return mContents->hash() >> 1;
        }
        
        virtual bool isEqual(const Base *other) const override
        {
            if(other && other->isKindOfClass<Annotation>()) {
                return this->contents()->isEqual(static_cast<const Annotation *>(other)->contents());
            }
            
            return false;
        }
        
        virtual const String *description() const override
        {
            return String::Builder() << "(%" << contents() << "%)";
        }
        
#pragma mark - Introspection
        
        ///Returns the contents string of the annotation.
        const String *contents() const { return retained_autoreleased(mContents); }
    };
}

#endif
