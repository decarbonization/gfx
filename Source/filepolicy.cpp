//
//  filepolicy.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 1/19/14.
//  Copyright (c) 2014 Roundabout Software, LLC. All rights reserved.
//

#include "filepolicy.h"
#include "exception.h"

namespace gfx {
#pragma mark - • FilePolicy
    
#pragma mark - Introspecting Paths
    
    bool FilePolicy::canOpenPath(const String *path)
    {
        return true;
    }
    
    bool FilePolicy::pathExists(const String *path)
    {
        return File::exists(path);
    }
    
    bool FilePolicy::isPathDirectory(const String *path)
    {
        return File::isDirectory(path);
    }
    
#pragma mark - Opening
    
    File *FilePolicy::openFileAtPath(const String *path, File::Mode mode)
    {
        return make<File>(path, mode);
    }
    
#pragma mark - Current File Policy
    
    static FilePolicy *gActiveFilePolicy = new FilePolicy();
    
    FilePolicy *FilePolicy::ActiveFilePolicy()
    {
        return retained_autoreleased(gActiveFilePolicy);
    }
    
    void FilePolicy::SetActiveFilePolicy(FilePolicy *filePolicy)
    {
        gfx_assert_param(filePolicy);
        
        released(gActiveFilePolicy);
        gActiveFilePolicy = retained(filePolicy);
    }
    
#pragma mark - • FilePolicyNoOpen
    
    bool FilePolicyNoOpen::canOpenPath(const String *path)
    {
        return false;
    }
    
    bool FilePolicyNoOpen::pathExists(const String *path)
    {
        return false;
    }
    
    bool FilePolicyNoOpen::isPathDirectory(const String *path)
    {
        return false;
    }
    
#pragma mark -
    
    File *FilePolicyNoOpen::openFileAtPath(const String *path, File::Mode mode)
    {
        throw Exception(str("Opening files is currently illegal."), nullptr);
        
        return nullptr;
    }
}
