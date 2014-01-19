//
//  filepolicy.h
//  gfx
//
//  Created by Kevin MacWhinnie on 1/19/14.
//  Copyright (c) 2014 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__filepolicy__
#define __gfx__filepolicy__

#include "file.h"

namespace gfx {
    ///The FilePolicy class is an abstraction layer between the gfx::File class
    ///and the core functions component of the Gfx language. It allows subclasses
    ///of gfx::File to be used throughout the runtime, and enables fine-grained
    ///control of the interpreter's file system access.
    ///
    ///The root FilePolicy class is a thin wrapper around the gfx::File class.
    ///A single subclass is provided, gfx::FilePolicyNoOpen, which disables all
    ///file system access for the Gfx runtime.
    ///
    ///#Important:
    ///The FilePolicy also applies to the import functionality in the interpreter.
    class FilePolicy : public Base
    {
    public:
        
#pragma mark - Lifecycle
        
        ///The constructor.
        explicit FilePolicy()
        {
        }
        
        ///The destructor.
        virtual ~FilePolicy()
        {
        }
        
#pragma mark - Introspecting Paths
        
        ///Returns a bool indicating whether or not a given path can be opened.
        ///
        /// \param  path    The path a File will be opened for. Required.
        ///
        /// \result true if the file can be opened; false otherwise.
        ///
        ///If this method returns false, calling `FilePolicy::openFileAtPath`
        ///will raise an exception.
        ///
        ///This is a primitive method that should be overriden by subclasses of FilePolicy.
        virtual bool canOpenPath(const String *path);
        
        ///Returns a bool indicating whether or not a given path exists.
        ///
        /// \param  path    The path to verify the existence of. Required.
        ///
        /// \result true if the file exists; false otherwise.
        ///
        ///This is a primitive method that should be overriden by subclasses of FilePolicy.
        virtual bool pathExists(const String *path);
        
        ///Returns a bool indicating whether or not a given path is a directory.
        ///
        /// \param  path    The path to check. Required.
        ///
        /// \result true if path is a directory; false otherwise.
        ///
        ///This is a primitive method that should be overriden by subclasses of FilePolicy.
        virtual bool isPathDirectory(const String *path);
        
#pragma mark - Opening
        
        ///Attempts to open a file for reading at a given path.
        ///
        /// \param  path    The path to read. Required.
        /// \param  mode    The mode to read the file with. Required.
        ///
        /// \result An open File ready for use.
        ///
        ///This method will raise an exception if there is an access violation
        ///in attempting to open the path, or an issue occurs when opening the file.
        virtual File *openFileAtPath(const String *path, File::Mode mode);
        
#pragma mark - Current File Policy
        
        ///Returns the active file policy. Defaults to FilePolicy.
        static FilePolicy *ActiveFilePolicy();
        
        ///Sets the active file policy.
        ///
        /// \param  filePolicy  The file policy to use. May not be null.
        ///
        ///This method is not thread-safe. The file policy should only be changed once.
        static void SetActiveFilePolicy(FilePolicy *filePolicy);
    };
    
    ///The FilePolicyNoOpen class blocks all file system operations
    ///(minus stdin/err/out) in the Gfx interpreter.
    class FilePolicyNoOpen : public FilePolicy
    {
    public:
        virtual bool canOpenPath(const String *path) override;
        virtual bool pathExists(const String *path) override;
        virtual bool isPathDirectory(const String *path) override;
        virtual File *openFileAtPath(const String *path, File::Mode mode) override;
    };
}

#endif /* defined(__gfx__filepolicy__) */
