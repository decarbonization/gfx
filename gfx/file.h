//
//  file.h
//  gfx
//
//  Created by Kevin MacWhinnie on 11/18/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__file__
#define __gfx__file__

#include <cstdio>
#include "base.h"

namespace gfx {
    ///The File class is a simple wrapper around the std::FILE
    ///type that provides higher level String operations.
    class File : public Base
    {
        ///The wrapped FILE.
        std::FILE *mFile;
        
        ///Whether or not the File instance owns `mFile`.
        ///
        ///If this ivar is set to true, the std::FILE will be
        ///implicitly closed when the File is destructed.
        ///If this property is false, the `File::close` method
        ///becomes a no-op
        bool mHasOwnership;
        
    public:
        
        ///Returns a File wrapper around stdin.
        static File *consoleIn();
        
        ///Returns a File wrapper around stdout.
        static File *consoleOut();
        
        ///Returns a File wrapper around stderr.
        static File *consoleError();

#pragma mark -
        
        ///Synchronously read the contents of the file at a given path.
        ///
        /// \param  path    The path of the file to read. Required.
        ///
        /// \result The contents of path.
        /// \throws Exception
        ///
        static String *readFileAtPath(const String *path);
        
        ///Synchronously write the contents of a string to a file at a given path.
        ///
        /// \param  path        The path to write the file to. Required.
        /// \param  contents    The data to write to `path`. Required.
        ///
        /// \throws Exception
        ///
        static void writeFileAtPath(const String *path, const String *contents);
        
#pragma mark -
        
        ///Returns a bool indicating whether or not a file exists at a given path.
        static bool exists(const String *path);
        
#pragma mark - Lifecycle
        
        ///Construct a File with a given std::FILE and whether or not to take ownership.
        ///
        /// \param  file            The file to wrap. Required.
        /// \param  takesOwnership  Whether or not the File is taking ownership of the `file`.
        ///
        explicit File(std::FILE *file, bool takesOwnership = false);
        
        ///Construct a File by opening a new std::FILE with a given path and mode.
        ///
        /// \param  path    The path to open the file at. Required.
        /// \param  mode    "r" for reading, "w" for writing. Required.
        ///
        explicit File(const String *path, const char *mode);
        
        ///The destructor.
        virtual ~File();
        
#pragma mark -
        
        ///Closes the wrapped std::FILE of the receiver if the receiver has ownership.
        virtual void close();
        
#pragma mark - Identity
        
        virtual CFHashCode hash() const override;
        virtual bool isEqual(const File *otherFile) const;
        virtual bool isEqual(const Base *other) const override;
        virtual const String *description() const override;
        
#pragma mark - Introspection
        
        ///Returns the total length of the file.
        ///
        ///Calling this method on the result of one of
        ///the `File::console*` methods is undefined behavior.
        virtual size_t length() const;
        
        ///Returns a bool indicating whether or not the File is positioned at its end.
        virtual bool isAtEndOfFile() const;
        
        ///Sets the absolute position of the file's cursor.
        virtual void setPosition(size_t position);
        
        ///Returns the absolute position of the file's cursor.
        virtual size_t position() const;
        
#pragma mark - Reading and Writing
        
        ///Read a blob of data into a given buffer of a given size.
        ///
        /// \param  outBuffer   The buffer to write the data into. Required.
        /// \param  bufferSize  The size of the buffer, and the maximum amount of data to read.
        ///
        /// \result The actual amount of data written into `outBuffer`.
        ///
        virtual size_t read(UInt8 *outBuffer, size_t bufferSize);
        
        ///Read a blob of data into a new String instance and return it.
        ///
        /// \param  amountToRead    The maximum amount of data to read.
        ///
        /// \result A String of at max `amountToRead` characters.
        virtual String *read(size_t amountToRead);
        
        ///Read a single line of data into a new String instance and return it.
        virtual String *readLine();
        
#pragma mark -
        
        ///Write a blob of data of a given size into the receiver.
        ///
        /// \param  buffer      The data to write. Required.
        /// \param  bufferSize  The size of the data to write.
        ///
        /// \result The actual amount of data written.
        virtual size_t write(const UInt8 *buffer, size_t bufferSize);
        
        ///Write the data contained in a given String into the receiver.
        ///
        /// \param  string  The string to write. Required.
        ///
        /// \result The actual amount of data written.
        ///
        virtual size_t write(const String *string);
        
        ///Write the data contained in a given String plus a newline into the receiver.
        ///
        /// \param  string  The string to write. Required.
        ///
        /// \result The actual amount of data written.
        ///
        virtual size_t writeLine(const String *string);
    };
}

#endif /* defined(__gfx__file__) */
