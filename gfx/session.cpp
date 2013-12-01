//
//  session.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "session.h"
#include "string.h"

namespace gfx {
    
#pragma mark - Singleton
    
    static Session *gSharedSession = nullptr;
    
    void Session::init(int argc, const char *argv[])
    {
        AutoreleasePool pool;
        
        gSharedSession = new Session(argc, argv);
    }
    
    Session *Session::shared()
    {
        if(!gSharedSession) {
            throw Exception("Session::init must be called before Session::shared."_gfx, nullptr);
        }
        
        return gSharedSession;
    }
    
#pragma mark - Accessing Data
    
    const String *Session::name() const
    {
        return retained_autoreleased(mName);
    }
    
    const Array<String> *Session::arguments() const
    {
        return retained_autoreleased(mArguments);
    }
    
    const Array<Argument> *Session::parsedArguments() const
    {
        return retained_autoreleased(mParsedArguments);
    }
    
#pragma mark - Parsing Arguments
    
    void Session::parseArguments()
    {
        static const String *const FlagPrefix = "-"_gfx;
        static const String *const ParameterPrefix = "--"_gfx;
        
        AutoreleasePool pool;
        
        mParsedArguments = new Array<Argument>();
        
        Argument::Type type = Argument::Type::Flag;
        String *label = nullptr;
        String *value = nullptr;
        bool isLookingForSecondHalf = false;
        for (String *string : mArguments) {
            if(isLookingForSecondHalf) {
                value = string;
                isLookingForSecondHalf = false;
            } else {
                if(string->hasPrefix(ParameterPrefix)) {
                    label = string->substring(Range(ParameterPrefix->length(), string->length() - ParameterPrefix->length()));
                    isLookingForSecondHalf = true;
                    type = Argument::Type::Parameter;
                } else if(string->hasPrefix(FlagPrefix)) {
                    label = string->substring(Range(FlagPrefix->length(), string->length() - FlagPrefix->length()));
                    value = nullptr;
                    type = Argument::Type::Flag;
                } else {
                    label = nullptr;
                    value = string;
                    type = Argument::Type::Text;
                    
                    mHasTextArguments = true;
                }
            }
            
            if(!isLookingForSecondHalf) {
                mParsedArguments->append(make<Argument>(type, label, value));
            }
        }
    }
    
#pragma mark - Lifecycle
    
    Session::Session(int argc, const char *argv[]) :
        mName(nullptr),
        mArguments(new Array<String>()),
        mHasTextArguments(false)
    {
        if(argc > 0)
            mName = make<String>(argv[0]);
        
        for (int i = 1; i < argc; i++)
            mArguments->append(make<String>(argv[i]));
        
        parseArguments();
    }
    
    Session::~Session()
    {
        released(mName);
        released(mArguments);
    }
}
