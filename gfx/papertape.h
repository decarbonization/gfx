//
//  papertape.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__papertape__
#define __gfx__papertape__

#include "base.h"

namespace gfx {
    class String;
    class File;
    
    ///The PaperTape interface-class is a thin wrapper class that allows
    ///console IO to be directed to places other than stdin/stdout.
    class PaperTape : public Base
    {
    public:
        
        ///Set the paper tape instance to use for all operations.
        ///
        /// \param  paperTape   The paper tape instance to use. Required.
        ///
        ///This method is not thread safe.
        static void SetCurrentPaperTape(PaperTape *paperTape);
        
        ///Returns the current paper tape instance to use for all console IO operations.
        static PaperTape *CurrentPaperTape();

#pragma mark -
        
        ///Writes a given string to the console.
        ///
        /// \param  data    The string to write. Required.
        ///
        ///This method uses the value of `PaperTape::CurrentPaperTape`.
        static void WriteLine(const String *data);
        
        ///Reads a line of data from the console.
        ///
        ///This method uses the value of `PaperTape::CurrentPaperTape`.
        static const String *ReadLine();
        
#pragma mark - Primitive Methods
        
        ///Writes a given string to the console.
        ///
        /// \param  data    The string to write. Required.
        ///
        virtual void writeLine(const String *data) = 0;
        
        ///Reads a line of data from the console.
        virtual const String *readLine() = 0;
    };
    
#pragma mark -
    
    ///The StdioPaperTape class implements PaperTape using `File::consoleIn()` and `File::consoleOut()`
    class StdioPaperTape : public PaperTape
    {
    public:
        virtual void writeLine(const String *data) override;
        virtual const String *readLine() override;
    };
}

#endif /* defined(__gfx__papertape__) */
