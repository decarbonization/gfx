//
//  SHDefinition.h
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

FOUNDATION_EXPORT NSString *const SHDefinitionName;
FOUNDATION_EXPORT NSString *const SHDefinitionRegex;

FOUNDATION_EXPORT NSString *const SHDefinitionNameStrings;
FOUNDATION_EXPORT NSString *const SHDefinitionNameNumbers;
FOUNDATION_EXPORT NSString *const SHDefinitionNameColors;
FOUNDATION_EXPORT NSString *const SHDefinitionNameKeywords;
FOUNDATION_EXPORT NSString *const SHDefinitionNameCoreFunctions;

@interface SHDefinition : NSObject

+ (NSArray *)definitionsNamed:(NSString *)bundleName;

+ (NSArray *)definitionsWithArrayOfDictionaries:(NSArray *)definitions;

- (instancetype)initWithDefinitionDictionary:(NSDictionary *)dictionary;

#pragma mark - Properties

@property (copy) NSString *name;
@property NSRegularExpression *regex;

@end
