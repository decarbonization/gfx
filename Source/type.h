//
//  type.h
//  gfx
//
//  Created by Kevin MacWhinnie on 1/24/14.
//  Copyright (c) 2014 Roundabout Software, LLC. All rights reserved.
//

#ifndef __gfx__type__
#define __gfx__type__

#include "base.h"
#include <map>
#include <string>
#include <typeinfo>

namespace gfx {
    ///The Type class functions as something of a metaclass for types
    ///in the Gfx runtime. Each core class has an associated `Type` object.
    ///
    /// \seealso(gfx::TypeResolutionMap)
    class Type : public Base
    {
        ///The parent, or super, type.
        Type *mParent;
        
        ///The name of the type, always enclosed in angle brackets.
        const String *mName;
        
    public:
        
#pragma mark - Lifecycle
        
        ///Constructs a type.
        ///
        /// \param  parent  The parent type, if any.
        /// \param  name    The name of the type. Must always be enclosed in angle brackets.
        Type(Type *parent, const String *name);
        
        virtual ~Type();
        
#pragma mark - Identity
        
        HashCode hash() const override;
        bool isEqual(const Base *other) const override;
        const String *description() const override;
        
#pragma mark - Introspection
        
        ///Returns the parent type, if any.
        virtual const Type *parent() const;
        
        ///Returns the name of the type.
        virtual const String *name() const;
        
        ///Returns a bool indicating whether or not the type is the
        ///same as another type, or is descended from that type.
        virtual bool isKindOf(const Type *other) const;
        
#pragma mark - Common Types
        
        ///Returns the shared type for `gfx::Base`.
        static Type *BaseType();
        
        ///Returns the shared type for `gfx::Type`.
        static Type *TypeType();
    };
    
#pragma mark -
    
    ///The TypeResolutionMap class encapsulates a mapping of native C++ objects
    ///(through unsanitized typeid names) to `gfx::Type` instances. Type resolution
    ///maps are used throughout the type-system in other parts of the runtime.
    class TypeResolutionMap : public Base
    {
        ///The map storage.
        std::map<std::string, Type *> mStorage;
        
    public:
        
        TypeResolutionMap();
        ~TypeResolutionMap();
        
#pragma mark - Types
        
        ///Registers a mapping between a `std::type_info` object, and a `gfx::Type`.
        ///After registration it is possible to look up a type by its name,
        ///or by a native C++ object's typeid info.
        void registerType(const std::type_info &info, Type *type);
        
        ///Look up a Type by `std::type_info`.
        const Type *lookupType(const std::type_info &info) const;
        
        ///Look up a Type by name.
        const Type *lookupTypeByName(const String *name) const;
        
#pragma mark - Common Maps
        
        ///Creates a new core resolution map for use with a `gfx::Interpreter` object.
        ///The returned object can safely be extended within an interpreter.
        static TypeResolutionMap *CreateCoreResolutionMap();
    };
}

#endif /* defined(__gfx__type__) */
