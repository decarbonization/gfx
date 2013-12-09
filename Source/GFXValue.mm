//
//  GFXValue.m
//  gfx
//
//  Created by Kevin MacWhinnie on 12/6/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "GFXValue.h"

#include "base.h"
#include "str.h"
#include "number.h"

@interface GFXValue ()

@property (readwrite) GFXValueType type;
@property (readwrite) GFXObjectRef object;
@property (readwrite) BOOL ownsObject;

@end

#pragma mark -

@implementation GFXValue

- (void)dealloc
{
    if(_ownsObject && _object) {
        released(_object);
        _object = nullptr;
    }
    
    [super dealloc];
}

+ (instancetype)valueWithObject:(GFXObjectRef)object takeOwnership:(BOOL)takeOwnership
{
    return [[[self alloc] initWithObject:object takeOwnership:takeOwnership] autorelease];
}

- (instancetype)initWithObject:(GFXObjectRef)object takeOwnership:(BOOL)takeOwnership
{
    NSParameterAssert(object);
    
    if((self = [super init])) {
        if(object->isKindOfClass<gfx::Number>())
            self.type = GFXValueTypeNumber;
        else if(object->isKindOfClass<gfx::String>())
            self.type = GFXValueTypeString;
        else
            self.type = GFXValueTypeOpaque;
        
        self.object = object;
        self.ownsObject = takeOwnership;
    }
    
    return self;
}

- (id)init
{
    [self doesNotRecognizeSelector:_cmd];
    return nil;
}

#pragma mark - Creating Values

+ (instancetype)valueWithNumber:(NSNumber *)number
{
    NSParameterAssert(number);
    
    auto object = new gfx::Number([number doubleValue]);
    return [self valueWithObject:object takeOwnership:YES];
}

+ (instancetype)valueWithString:(NSString *)string
{
    NSParameterAssert(string);
    
    auto object = new gfx::String((CFStringRef)string);
    return [self valueWithObject:object takeOwnership:YES];
}

#pragma mark - Identity

- (NSUInteger)hash
{
    return [[self class] hash] ^ _object->hash();
}

- (BOOL)isEqual:(id)object
{
    if([object isKindOfClass:[GFXValue class]]) {
        GFXValue *other = object;
        return other.object->isEqual(self.object);
    }
    
    return NO;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@:%p object => '%@'>", NSStringFromClass(self.class), self, _object->description()->getStorage()];
}

#pragma mark - Introspection

- (NSNumber *)numberValue
{
    if(self.type == GFXValueTypeNumber && _object->isKindOfClass<gfx::Number>()) {
        auto number = (const gfx::Number *)_object;
        return @(number->value());
    } else {
        return nil;
    }
}

- (NSString *)stringValue
{
    if(self.type == GFXValueTypeString && _object->isKindOfClass<gfx::String>()) {
        auto string = (const gfx::String *)_object;
        return [[(NSString *)string->getStorage() copy] autorelease];
    } else {
        return nil;
    }
}

@end
