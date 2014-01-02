//
//  assertions.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/20/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "assertions.h"
#include "str.h"
#include "exception.h"
#include "file.h"

namespace gfx {
    bool PrintAssertions = true;
    
    void handleAssertionHandler(const char *function, const char *file, long lineNumber, const String *reason)
    {
        if(PrintAssertions) {
            String::Builder message;
            message << "*** Assertion failure in function "<< function << " from file " << file << " on line " << lineNumber << ": " << reason;
            File::consoleError()->writeLine(message);
        }
        
        throw Exception(reason, nullptr);
    }
}
