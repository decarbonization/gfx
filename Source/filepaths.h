//
//  filepaths.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/1/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef gfx_filepaths_h
#define gfx_filepaths_h

#include "str.h"

namespace gfx {
    
    ///The FilePaths class encapsulates utility functions that operate on path Strings.
    class FilePaths final
    {
    public:
        ///The current platform's path separator token.
        static const String *const kPathSeparatorToken;
        
        ///The current platform's path extension token.
        static const String *const kPathExtensionToken;
        
        
        ///Normalizes the contents of a given path string.
        ///
        /// \param  path    The path to normalize. May not be null.
        ///
        /// \result A new copy of path conforming to the format `str("/path/components")`.
        ///
        static const String *normalizePath(const String *path)
        {
            gfx_assert_param(path);
            
            String *newPath = autoreleased(copy(path));
            if(!newPath->hasPrefix(kPathSeparatorToken))
                newPath->insert(kPathSeparatorToken, 0);
            
            if(newPath->hasSuffix(kPathSeparatorToken))
                newPath->deleteRange(Range(newPath->length() - kPathSeparatorToken->length(), kPathSeparatorToken->length()));
            
            return newPath;
        }
        
        ///Returns the extension of a given path string.
        ///
        /// \param  path    The path whose extension to return. May not be null.
        ///
        /// \result The extension of the path, or an empty string if there was no extension.
        ///
        static const String *pathExtension(const String *path)
        {
            gfx_assert_param(path);
            
            Range locationOfSeparator = path->find(kPathExtensionToken, Range(0, path->length()), kCFCompareBackwards);
            if(locationOfSeparator.location != kCFNotFound)
                return path->substring(Range(locationOfSeparator.max(), path->length() - locationOfSeparator.location - locationOfSeparator.length));
            else
                return String::Empty;
        }
        
        ///Returns the last component of a given path string.
        ///
        /// \param  path    The path whose last component is to be returned. May not be null.
        ///
        /// \result The last path component. Returns `path` if there are no path separators in the string.
        static const String *lastPathComponent(const String *path)
        {
            gfx_assert_param(path);
            
            Range locationOfSeparator = path->find(kPathSeparatorToken, Range(0, path->length()), kCFCompareBackwards);
            if(locationOfSeparator.location != kCFNotFound)
                return path->substring(Range(locationOfSeparator.max(), path->length() - locationOfSeparator.location - locationOfSeparator.length));
            else
                return path;
        }
        
        ///Returns a new path constructed from two other given paths.
        ///
        /// \param  path1   The first path. May not be null.
        /// \param  path2   The second path. May not be null.
        ///
        /// \result A string consisting of the combined and normalized contents of both given paths.
        ///
        static const String *combinePaths(const String *path1, const String *path2)
        {
            gfx_assert_param(path1);
            gfx_assert_param(path2);
            
            String::Builder result;
            
            result << FilePaths::normalizePath(path1);
            result << FilePaths::normalizePath(path2);
            
            return result;
        }
        
        ///Returns a new path by deleting the last path component of a given path.
        ///
        /// \param  path    May not be null.
        ///
        /// \result The path string passed, minus the last path component.
        ///         If there are no path separator tokens, this method returns the path.
        ///
        static const String *deleteLastPathComponent(const String *path)
        {
            gfx_assert_param(path);
            
            Range locationOfSeparator = path->find(kPathSeparatorToken, Range(0, path->length()), kCFCompareBackwards);
            if(locationOfSeparator.location != kCFNotFound)
                return path->substring(Range(0, locationOfSeparator.location));
            else
                return path;
        }
        
        ///Returns a new path by deleting the path extension of a given path.
        ///
        /// \param  path    May not be null.
        ///
        /// \result The path string passed, minus the path extension.
        ///         If there is no path extension, this method returns the path.
        ///
        static const String *deletePathExtension(const String *path)
        {
            gfx_assert_param(path);
            
            Range locationOfSeparator = path->find(kPathExtensionToken, Range(0, path->length()), kCFCompareBackwards);
            if(locationOfSeparator.location != kCFNotFound)
                return path->substring(Range(0, locationOfSeparator.location));
            else
                return path;
        }
        
    private:
        
        FilePaths();
        ~FilePaths();
    };
}

#endif
