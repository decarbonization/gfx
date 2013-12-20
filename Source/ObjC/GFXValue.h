//
//  GFXValue.h
//  gfx
//
//  Created by Kevin MacWhinnie on 12/6/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

///A conditionally compiled type that makes this header
///safe for inclusion by ObjC and ObjC++ files.
#if __cplusplus

namespace gfx { class Base; }
typedef const gfx::Base * GFXObjectRef;

#else

typedef const void * GFXObjectRef;

#endif /* __cplusplus */

///The different types of objects a GFXValue may contain.
typedef NS_ENUM(NSUInteger, GFXValueType) {
    ///The contents are opaque to ObjC.
    GFXValueTypeOpaque,
    
    ///The contents are a number.
    GFXValueTypeNumber,
    
    ///The contents are a string.
    GFXValueTypeString,
};

///The GFXValue class encapsulates a generic `gfx::Base` instance
///into an Objective-C object that can be cleanly passed into and
///and out of GFX Objective-C APIs.
@interface GFXValue : NSObject

#pragma mark - Lifecycle

///Creates a new autoreleased GFXValue receiver with a given object.
///
/// \param  object          The object to store in the value. Not retained.
/// \param  takeOwnership   Whether or not the object should be released with the value wrapper.
///
/// \result A new autoreleased value object that may be passed around.
+ (instancetype)valueWithObject:(GFXObjectRef)object takeOwnership:(BOOL)takeOwnership;

///Initialize the receiver with a given object.
///
/// \param  object          The object to store in the value. Not retained.
/// \param  takeOwnership   Whether or not the object should be released with the value wrapper.
///
/// \result A fully initialized value object that may be passed around.
///
///This is the designated initializer.
- (instancetype)initWithObject:(GFXObjectRef)object takeOwnership:(BOOL)takeOwnership;

///Not available.
///
/// \seealso(-[GFXValue initWithObject:takeOwnership:])
- (id)init UNAVAILABLE_ATTRIBUTE;

#pragma mark - Creating Values

///Creates and returns a new value object with the contents of a given number.
///
/// \param  number  The number to use as the contents of the value. Required.
///
/// \result A new autoreleased GFXValue ready for use.
+ (instancetype)valueWithNumber:(NSNumber *)number;

///Creates and returns a new value object with the contents of a given string.
///
/// \param  string  The string to create a new value object with. Required.
///
/// \result A new autoreleased GFXValue ready for use.
+ (instancetype)valueWithString:(NSString *)string;

#pragma mark - Properties

///The type of the object contained by the value.
@property (readonly) GFXValueType type;

///The object of the value.
@property (readonly) GFXObjectRef object;

///Whether or not the object will be released with the value wrapper.
@property (readonly) BOOL ownsObject;

#pragma mark - Introspection

///Returns the number representation of the receiver.
///
/// \result An NSNumber object if `self.type` is `GFXValueTypeNumber`; nil otherwise.
- (NSNumber *)numberValue;

///Returns the string representation of the receiver.
///
/// \result An NSString object if `self.type` is `GFXValueTypeString`; nil otherwise.
- (NSString *)stringValue;

@end

#if __cplusplus

namespace gfx {
    ///Extracts the object from a `GFXValue` instance.
    ///
    /// \tparam T       The object type to extract from the value.
    ///                 Should be `gfx::Base`-derived. Should not be a pointer.
    ///
    /// \param  value   The value instance to extract the value from. May be nil.
    ///
    /// \result The object of the value if it matches the type `T`. nullptr if `value` is nil.
    /// \throws NSException if the object of the value is not of type `T`.
    ///
    template<typename T> T *lift_value(GFXValue *value)
    {
        if(!value)
            return nullptr;
        
        auto object = value.object;
        auto result = dynamic_cast<T *>(object);
        if(result == nullptr)
            [NSException raise:NSInternalInconsistencyException format:@"Type mismatch in lift_value."];
        
        return result;
    }
}

#endif /* __cplusplus */
