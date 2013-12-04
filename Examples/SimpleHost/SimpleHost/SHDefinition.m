//
//  SHDefinition.m
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "SHDefinition.h"

NSString *const SHDefinitionName = @"SHDefinitionName";
NSString *const SHDefinitionRegex = @"SHDefinitionRegex";

NSString *const SHDefinitionNameStrings = @"SHDefinitionNameStrings";
NSString *const SHDefinitionNameNumbers = @"SHDefinitionNameNumbers";
NSString *const SHDefinitionNameColors = @"SHDefinitionNameColors";
NSString *const SHDefinitionNameKeywords = @"SHDefinitionNameKeywords";
NSString *const SHDefinitionNameCoreFunctions = @"SHDefinitionNameCoreFunctions";
NSString *const SHDefinitionNameComments = @"SHDefinitionNameComments";

@implementation SHDefinition

+ (NSArray *)definitionsNamed:(NSString *)bundleName
{
    NSParameterAssert(bundleName);
    
    NSURL *definitionLocation = [[NSBundle mainBundle] URLForResource:bundleName withExtension:@"shd"];
    NSArray *definitions = [NSArray arrayWithContentsOfURL:definitionLocation];
    return [self definitionsWithArrayOfDictionaries:definitions];
}

+ (NSArray *)definitionsWithArrayOfDictionaries:(NSArray *)definitions
{
    NSParameterAssert(definitions);
    
    NSMutableArray *definitionObjects = [NSMutableArray array];
    for (NSDictionary *definition in definitions)
        [definitionObjects addObject:[[SHDefinition alloc] initWithDefinitionDictionary:definition]];
    
    return definitionObjects;
}

- (instancetype)initWithDefinitionDictionary:(NSDictionary *)dictionary
{
    if((self = [super init])) {
        self.name = dictionary[SHDefinitionName];
        
        NSError *error = nil;
        self.regex = [NSRegularExpression regularExpressionWithPattern:dictionary[SHDefinitionRegex]
                                                               options:kNilOptions
                                                                 error:&error];
        NSAssert((self.regex != nil), @"Bad regular expression from definition dictionary %@. %@", dictionary, error);
    }
    
    return self;
}

@end
