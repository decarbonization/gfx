//
//  session.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 11/30/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#include "session.h"
#include "str.h"
#include "file.h"

#if TARGET_OS_MAC
#   include <dispatch/dispatch.h>
#endif /* TARGET_OS_MAC */

namespace gfx {
    
#pragma mark - Singleton
    
    static Session *gSharedSession = nullptr;
    
    void Session::init(int argc, const char *argv[])
    {
        AutoreleasePool pool;
        
        gSharedSession = new Session(argc, argv);
    }
    
    Session *Session::shared()
    {
        if(!gSharedSession) {
            throw Exception("Session::init must be called before Session::shared."_gfx, nullptr);
        }
        
        return gSharedSession;
    }
    
#pragma mark - Resources
    
    const String *Session::resourcesPath() const
    {
        static const String *sharedResourcesPath = nullptr;
        
#if GFX_TARGET_OSX_FRAMEWORK
        
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            AutoreleasePool pool;
            
            CFBundleRef frameworkBundle = CFBundleGetBundleWithIdentifier(CFSTR("com.roundabout.gfx"));
            gfx_assert(frameworkBundle != NULL, "Could not find framework bundle."_gfx);
            cf::AutoRef<CFURLRef> resourcesURL = CFBundleCopyResourcesDirectoryURL(frameworkBundle);
            
            UInt8 pathBuffer[PATH_MAX];
            CFURLGetFileSystemRepresentation(resourcesURL, true, pathBuffer, sizeof(pathBuffer));
            sharedResourcesPath = new String((const char *)pathBuffer);
        });
        
#elif GFX_TARGET_IOS_LIBRARY
        
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            AutoreleasePool pool;
            
            CFBundleRef mainBundle = CFBundleGetMainBundle();
            gfx_assert(mainBundle != NULL, "Could not find main bundle."_gfx);
            cf::AutoRef<CFURLRef> resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
            
            UInt8 pathBuffer[PATH_MAX];
            CFURLGetFileSystemRepresentation(resourcesURL, true, pathBuffer, sizeof(pathBuffer));
            sharedResourcesPath = new String((const char *)pathBuffer);
        });
        
#elif GFX_TARGET_SYS_LIBRARY
        
        if(!sharedResourcesPath) {
            sharedResourcesPath = (GFX_TARGET_INSTALLATION_DIR "/share/gfx"_gfx);
        }
        
#endif /* GFX_TARGET */
        
        return sharedResourcesPath;
    }
    
    const String *Session::pathForResource(const String *resourceName) const
    {
        return FilePaths::combinePaths(this->resourcesPath(), resourceName);
    }
    
#pragma mark - Parsing Arguments
    
    void Session::parseArguments()
    {
        static const String *const FlagPrefix = "-"_gfx;
        static const String *const ParameterPrefix = "--"_gfx;
        
        AutoreleasePool pool;
        
        mParsedArguments = new Array<Argument>();
        
        Argument::Type type = Argument::Type::Flag;
        String *label = nullptr;
        String *value = nullptr;
        bool isLookingForSecondHalf = false;
        for (String *string : mArguments) {
            if(isLookingForSecondHalf) {
                value = string;
                isLookingForSecondHalf = false;
            } else {
                if(string->hasPrefix(ParameterPrefix)) {
                    label = string->substring(Range(ParameterPrefix->length(), string->length() - ParameterPrefix->length()));
                    isLookingForSecondHalf = true;
                    type = Argument::Type::Parameter;
                } else if(string->hasPrefix(FlagPrefix)) {
                    label = string->substring(Range(FlagPrefix->length(), string->length() - FlagPrefix->length()));
                    value = nullptr;
                    type = Argument::Type::Flag;
                } else {
                    label = nullptr;
                    value = string;
                    type = Argument::Type::Text;
                    
                    mHasTextArguments = true;
                }
            }
            
            if(!isLookingForSecondHalf) {
                mParsedArguments->append(make<Argument>(type, label, value));
            }
        }
    }
    
#pragma mark - Lifecycle
    
    Session::Session(int argc, const char *argv[]) :
        mName(nullptr),
        mArguments(new Array<String>()),
        mHasTextArguments(false)
    {
        if(argc > 0)
            mName = make<String>(argv[0]);
        
        for (int i = 1; i < argc; i++)
            mArguments->append(make<String>(argv[i]));
        
        parseArguments();
    }
    
    Session::~Session()
    {
        released(mName);
        released(mArguments);
    }
}
