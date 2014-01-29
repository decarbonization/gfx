//
//  file.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/18/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "file.h"
#include "str.h"
#include "exception.h"
#include "blob.h"

#include <unistd.h>
#include <sys/stat.h>

namespace gfx {
    
    File *File::consoleIn()
    {
        static File *sharedConsoleIn = new File(stdin, false);
        return sharedConsoleIn;
    }
    
    File *File::consoleOut()
    {
        static File *sharedConsoleOut = new File(stdout, false);
        return sharedConsoleOut;
    }
    
    File *File::consoleError()
    {
        static File *sharedConsoleError = new File(stderr, false);
        return sharedConsoleError;
    }
    
#pragma mark -
    
    String *File::readFileAtPath(const String *path)
    {
        auto file = make<File>(path, File::Mode::Read);
        return file->readString(file->length());
    }
    
    void File::writeFileAtPath(const String *path, const String *contents)
    {
        AutoreleasePool pool;
        
        auto file = make<File>(path, File::Mode::Write);
        file->writeString(contents);
    }
    
#pragma mark -
    
    bool File::exists(const String *path)
    {
        return (access(path->getCString(), F_OK) != -1);
    }
    
    bool File::isDirectory(const String *path)
    {
        struct stat info = {};
        if(lstat(path->getCString(), &info) == 0) {
            return S_ISDIR(info.st_mode);
        } else {
            throw Exception(str("Could not lookup info on path."), nullptr);
        }
    }
    
#pragma mark - Lifecycle
    
    File::File(std::FILE *file, bool takesOwnership) :
        mFile(file),
        mHasOwnership(takesOwnership)
    {
    }
    
    File::File(const String *path, File::Mode mode) :
        mFile(std::fopen(path->getCString(), ModeToString(mode))),
        mHasOwnership(true)
    {
        if(!mFile) {
            throw Exception((String::Builder() << "opening file failed with error " << stderr), nullptr);
        }
    }
    
    File::~File()
    {
        this->close();
    }
    
#pragma mark -
    
    void File::close()
    {
        if(mHasOwnership && mFile) {
            std::fclose(mFile);
            mFile = nullptr;
        }
    }
    
#pragma mark - Identity
    
    HashCode File::hash() const
    {
        return (HashCode)mFile;
    }
    
    bool File::isEqual(const File *otherFile) const
    {
        if(!otherFile)
            return false;
        
        return mFile == otherFile->mFile;
    }
    
    bool File::isEqual(const Base *other) const
    {
        if(!other)
            return false;
        
        if(other->isKindOfClass<File>())
            return this->isEqual((const File *)other);
        
        return false;
    }
    
    const String *File::description() const
    {
        String::Builder description;
        
        description << "<";
        description << this->className() << ":" << (void *)this << " ";
        description << "length => " << this->length() << ", ";
        description << "position => " << this->position() << ">";
        
        return description;
    }
    
#pragma mark - Introspection
    
    size_t File::length() const
    {
        size_t oldPosition = std::ftell(mFile);
        std::fseek(mFile, 0, SEEK_END);
        size_t length = std::ftell(mFile);
        std::fseek(mFile, oldPosition, SEEK_SET);
        
        return length;
    }
    
    bool File::isAtEndOfFile() const
    {
        return std::feof(mFile);
    }
    
    void File::setPosition(size_t position)
    {
        std::fseek(mFile, position, SEEK_SET);
        
        if(long errorCode = std::ferror(mFile)) {
            throw Exception((String::Builder() << "setting position failed with error " << errorCode), nullptr);
        }
    }
    
    size_t File::position() const
    {
        return std::ftell(mFile);
    }
    
#pragma mark - Reading and Writing
    
    size_t File::read(UInt8 *outBuffer, size_t bufferSize)
    {
        size_t amount = std::fread(outBuffer, 1, bufferSize, mFile);
        if(long errorCode = std::ferror(mFile)) {
            throw Exception((String::Builder() << "reading file failed with error " << errorCode), nullptr);
        }
        return amount;
    }
    
    Blob *File::read(size_t amountToRead)
    {
        auto blob = make<Blob>(amountToRead);
        size_t amountRead = read(blob->bytes(), blob->length());
        blob->setLength(amountRead);
        return blob;
    }
    
    String *File::readString(size_t amountToRead)
    {
        UInt8 buffer[amountToRead];
        size_t length = this->read(buffer, amountToRead);
        char bufferWithNul[length + 1];
        memmove(bufferWithNul, buffer, length);
        bufferWithNul[length] = '\0';
        return make<String>(bufferWithNul);
    }
    
    String *File::readLine()
    {
        String::Builder line;
        
        UInt8 buffer[1];
        while (!this->isAtEndOfFile() && this->read(buffer, sizeof(buffer)) != 0) {
            if(buffer[0] == '\n')
                break;
            else
                line << (UniChar)buffer[0];
        }
        
        return line;
    }
    
#pragma mark -
    
    size_t File::write(const UInt8 *buffer, size_t bufferSize)
    {
        size_t amount = std::fwrite(buffer, 1, bufferSize, mFile);
        if(long errorCode = std::ferror(mFile)) {
            throw Exception((String::Builder() << "writing to file failed with error " << errorCode), nullptr);
        }
        return amount;
    }
    
    size_t File::write(const Blob *blob)
    {
        return write(blob->bytes(), blob->length());
    }
    
    size_t File::writeString(const String *string)
    {
        return this->write((const UInt8 *)string->getCString(), string->length());
    }
    
    size_t File::writeLine(const String *string)
    {
        return this->writeString(String::Builder() << string << str("\n"));
    }
}
