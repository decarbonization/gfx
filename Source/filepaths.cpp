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
    
    const String *FilePaths::normalizePath(const String *path)
    {
        gfx_assert_param(path);
        
        String *newPath = autoreleased(copy(path));
        if(!newPath->hasPrefix(kPathSeparatorToken))
            newPath->insert(kPathSeparatorToken, 0);
        
        if(newPath->hasSuffix(kPathSeparatorToken))
            newPath->deleteRange(Range(newPath->length() - kPathSeparatorToken->length(), kPathSeparatorToken->length()));
        
        return newPath;
    }
    
    const String *FilePaths::pathExtension(const String *path)
    {
        gfx_assert_param(path);
        
        Range locationOfSeparator = path->find(kPathExtensionToken, Range(0, path->length()), kCFCompareBackwards);
        if(locationOfSeparator.location != kCFNotFound)
            return path->substring(Range(locationOfSeparator.max(), path->length() - locationOfSeparator.location - locationOfSeparator.length));
        else
            return String::Empty;
    }
    
    const String *FilePaths::lastPathComponent(const String *path)
    {
        gfx_assert_param(path);
        
        Range locationOfSeparator = path->find(kPathSeparatorToken, Range(0, path->length()), kCFCompareBackwards);
        if(locationOfSeparator.location != kCFNotFound)
            return path->substring(Range(locationOfSeparator.max(), path->length() - locationOfSeparator.location - locationOfSeparator.length));
        else
            return path;
    }
    
    const String *FilePaths::combinePaths(const String *path1, const String *path2)
    {
        gfx_assert_param(path1);
        gfx_assert_param(path2);
        
        String::Builder result;
        
        result << FilePaths::normalizePath(path1);
        result << FilePaths::normalizePath(path2);
        
        return result;
    }
    
    const String *FilePaths::deleteLastPathComponent(const String *path)
    {
        gfx_assert_param(path);
        
        Range locationOfSeparator = path->find(kPathSeparatorToken, Range(0, path->length()), kCFCompareBackwards);
        if(locationOfSeparator.location != kCFNotFound)
            return path->substring(Range(0, locationOfSeparator.location));
        else
            return path;
    }
    
    const String *FilePaths::deletePathExtension(const String *path)
    {
        gfx_assert_param(path);
        
        Range locationOfSeparator = path->find(kPathExtensionToken, Range(0, path->length()), kCFCompareBackwards);
        if(locationOfSeparator.location != kCFNotFound)
            return path->substring(Range(0, locationOfSeparator.location));
        else
            return path;
    }
}