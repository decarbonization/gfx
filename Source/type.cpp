//
//  type.cpp
//  gfx
//
//  Created by Kevin MacWhinnie on 1/24/14.
//  Copyright (c) 2014 Roundabout Software, LLC. All rights reserved.
//

#include "type.h"

#include "str.h"
#include "number.h"
#include "array.h"
#include "dictionary.h"
#include "blob.h"
#include "file.h"
#include "function.h"

namespace gfx {
#pragma mark - Type
    
    Type::Type(Type *parent, const String *name) :
        mParent(retained(parent)),
        mName(copy(name))
    {
    }
    
    Type::~Type()
    {
        released(mParent);
        mParent = nullptr;
        
        released(mName);
        mName = nullptr;
    }
    
#pragma mark - Identity
    
    HashCode Type::hash() const
    {
        return mParent->hash() ^ mName->hash();
    }
    
    bool Type::isEqual(const Base *other) const
    {
        if(other->isKindOfClass<Type>()) {
            auto otherType = static_cast<const Type *>(other);
            bool parentEqual = (parent() == otherType->parent() ||
                                (parent() && parent()->isEqual(otherType->parent())));
            bool nameEqual = name()->isEqual(otherType->name());
            return parentEqual && nameEqual;
        }
        
        return false;
    }
    
    const String *Type::description() const
    {
        return name();
    }
    
#pragma mark - Introspection
    
    const Type *Type::parent() const
    {
        return retained_autoreleased(mParent);
    }
    
    const String *Type::name() const
    {
        return retained_autoreleased(mName);
    }
    
    bool Type::isKindOf(const Type *other) const
    {
        if(other) {
            if(this->isEqual(other))
                return true;
            
            if(mParent)
                return this->parent()->isKindOf(other);
        }
        
        return false;
    }
    
#pragma mark - Common Types
    
    Type *Type::BaseType()
    {
        static Type *baseType = new Type(nullptr, str("<val>"));
        return baseType;
    }
    
    Type *Type::TypeType()
    {
        static Type *typeType = new Type(nullptr, str("<type>"));
        return typeType;
    }
    
#pragma mark - TypeResolutionMap
    
    TypeResolutionMap::TypeResolutionMap() :
        mStorage()
    {
    }
    
    TypeResolutionMap::~TypeResolutionMap()
    {
        for (auto pair : mStorage) {
            released(pair.second);
        }
    }
    
#pragma mark - Types
    
    void TypeResolutionMap::registerType(const std::type_info &info, Type *type)
    {
        mStorage.emplace(info.name(), retained(type));
    }
    
    const Type *TypeResolutionMap::lookupType(const std::type_info &info) const
    {
        try {
            return mStorage.at(info.name());
        } catch (std::out_of_range) {
            return nullptr;
        }
    }
    
    const Type *TypeResolutionMap::lookupTypeByName(const String *name) const
    {
        for (auto pair : mStorage) {
            if(pair.second->name()->isEqual(name))
                return pair.second;
        }
        
        return nullptr;
    }
    
#pragma mark - Common Maps
    
    TypeResolutionMap *TypeResolutionMap::CreateCoreResolutionMap()
    {
        auto map = new TypeResolutionMap();
        
        AutoreleasePool pool;
        
        auto baseType = Type::BaseType();
        map->registerType(typeid(Base), baseType);
        map->registerType(typeid(Type), Type::TypeType());
        
        map->registerType(typeid(String), make<Type>(baseType, str("<str>")));
        map->registerType(typeid(Number), make<Type>(baseType, str("<num>")));
        map->registerType(typeid(Array<Base>), make<Type>(baseType, str("<vec>")));
        map->registerType(typeid(Dictionary<Base, Base>), make<Type>(baseType, str("<hash>")));
        map->registerType(typeid(Blob), make<Type>(baseType, str("<blob>")));
        map->registerType(typeid(Function), make<Type>(baseType, str("<func>")));
        
        return map;
    }
}
