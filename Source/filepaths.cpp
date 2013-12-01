//
//  filepaths.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 12/1/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "filepaths.h"

namespace gfx {
    //Everything is Unix-derived right now.
    const String *const FilePaths::kPathSeparatorToken = new String("/");
    const String *const FilePaths::kPathExtensionToken = new String(".");
}