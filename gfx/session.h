//
//  session.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__session__
#define __gfx__session__

#include "base.h"
#include "array.h"

namespace gfx {
    
    class Argument : public Base
    {
    public:
        
        enum class Type
        {
            Flag,
            Parameter,
            Text,
        };
        
    protected:
        
        Type mType;
        String *mLabel;
        String *mValue;
        
    public:
        
        Argument(Type type, String *label, String *value) :
            mType(type),
            mLabel(retained(label)),
            mValue(retained(value))
        {
            
        }
        
        ~Argument()
        {
            released(mLabel);
            released(mValue);
        }
        
#pragma mark -
        
        Type type() const { return mType; }
        const String *label() const { return retained_autoreleased(mLabel); }
        const String *value() const { return retained_autoreleased(mValue); }
    };
    
#pragma mark -
    
    ///The Session class encapsulates global information pertaining to the current Gfx host.
    ///This information includes command line arguments, environment variables, presence
    ///of an active run loop, and tools to assist in interpreting these values.
    ///
    ///`Session::init` should be called before using any gfx classes.
    class Session final : public Base
    {
        ///The name of the current program, as deduced by argv[0].
        String *mName;
        
        ///The command line arguments passed into the program.
        Array<String> *mArguments;
        
        ///The parsed command line arguments of the program.
        Array<Argument> *mParsedArguments;
        
        ///Whether or not there are text values in mParsedArguments.
        bool mHasTextArguments;
        
    public:
        
#pragma mark - Singleton
        
        ///Initialize the global session with a given argument vector.
        ///
        /// \param  argc    The number of arguments given.
        /// \param  argv    The arguments given to the program.
        ///
        ///`Session::shared` is available immediately after this method returns.
        static void init(int argc, const char *argv[]);
        
        ///Returns the shared Session object, raising if it has not been created yet.
        static Session *shared();
        
#pragma mark - Accessing Data
        
        ///Returns the name of the current program.
        const String *name() const;
        
        ///Returns the command line arguments.
        const Array<String> *arguments() const;
        
        ///Parses the arguments array of the session, returning
        ///an array of `gfx::Argument` objects describing the
        ///information passed into the program.
        const Array<Argument> *parsedArguments() const;
        
        ///Returns whether or not there are text arguments
        ///presence in the parsed arguments of the receiver.
        bool hasTextArguments() const { return mHasTextArguments; }
        
    private:
        
#pragma mark - Parsing Arguments
        
        ///Parses the command line arguments known to the session.
        void parseArguments();
        
#pragma mark - Lifecycle
        
        ///Constructs the session using the command line argument vector from `main`.
        Session(int argc, const char *argv[]);
        
        ///The destructor.
        ~Session();
    };
}

#endif /* defined(__gfx__session__) */
