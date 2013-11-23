//
//  main.c
//  gfx
//
//  Created by Kevin MacWhinnie on 11/13/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include "gfx.h"
#include "graphics.h"

using namespace gfx;

int main(int argc, const char * argv[])
{
    AutoreleasePool pool;
    
    auto interpreter = make<Interpreter>();
    Graphics::addTo(interpreter->currentFrame());
    
    auto result = make<Parser>("[100 100] canvas/begin 255 255 0 rgb set-fill [0 0 100 100] path/oval path/fill \"/Users/km/Desktop/test.png\" canvas/save"_gfx)->parse();
    interpreter->eval(result);
    
    Base *lastValue = interpreter->lastValue();
    if(lastValue)
        PaperTape::WriteLine(lastValue->description());
    
    return 0;
}
