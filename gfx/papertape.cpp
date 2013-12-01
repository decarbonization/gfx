//
//  papertape.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/15/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "papertape.h"
#include "str.h"
#include "file.h"

namespace gfx {
    static PaperTape *gCurrentPaperTape = new StdioPaperTape;
    
    void PaperTape::SetCurrentPaperTape(PaperTape *paperTape)
    {
        autoreleased(gCurrentPaperTape);
        gCurrentPaperTape = retained(paperTape);
    }
    
    PaperTape *PaperTape::CurrentPaperTape()
    {
        return gCurrentPaperTape;
    }
    
    void PaperTape::WriteLine(const String *data)
    {
        PaperTape::CurrentPaperTape()->writeLine(data);
    }
    
    const String *PaperTape::ReadLine()
    {
        return PaperTape::CurrentPaperTape()->readLine();
    }
    
#pragma mark - 
    
    void StdioPaperTape::writeLine(const String *data)
    {
        File::consoleOut()->writeLine(data);
    }
    
    const String *StdioPaperTape::readLine()
    {
        return File::consoleIn()->readLine();
    }
}
