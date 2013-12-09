//
//  SHDefinition.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

///The name of the definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionName;

///The regex of the definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionRegex;


///The name of a strings definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionNameStrings;

///The name of a numbers definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionNameNumbers;

///The name of a colors definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionNameColors;

///The name of a keywords definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionNameKeywords;

///The nmae of a core functions definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionNameCoreFunctions;

///The name of a comments definition. Value is the same as the constant name.
FOUNDATION_EXPORT NSString *const SHDefinitionNameComments;


///The SHDefinition class encapsulates a regular expression describing
///a syntactic component of code, and a name for that component.
@interface SHDefinition : NSObject

///Looks for a definitions plist file in the main bundle with a given name.
///
/// \param  bundleName  The name of the plist file in the main bundle. The file must have the extension shd.
///
/// \result An array of SHDefinition objects or nil.
///
+ (NSArray *)definitionsNamed:(NSString *)bundleName;

///Creates an array of SHDefinition objects from an array of dictionaries
///whose keys correspond to `SHDefinitionName` and `SHDefinitionRegex`.
///
/// \param  definitions The definition dictionaries. Required.
///
/// \result An array of SHDefinition objects.
///
+ (NSArray *)definitionsWithArrayOfDictionaries:(NSArray *)definitions;

///Initialize the receiver with a definition dictionary, whose
///keys correspond to `SHDefinitionName` and `SHDefinitionRegex`.
///
/// \param  dictionary  The definition dictionary. Required.
///
/// \result A fully initialized definition.
- (instancetype)initWithDefinitionDictionary:(NSDictionary *)dictionary;

#pragma mark - Properties

///The name of the definition. Should be one of the
///predefined `SHDefinitionName*` constants.
@property (copy) NSString *name;

///The precompiled regular expression of the definition.
@property NSRegularExpression *regex;

@end
