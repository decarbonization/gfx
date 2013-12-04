//
//  SyntaxHighligher.m
//  SimpleHost
//
//  Created by Kevin MacWhinnie on 12/3/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "SyntaxHighligher.h"
#import "SHDefinition.h"

@implementation SyntaxHighligher

- (instancetype)initWithDefinitions:(NSArray *)definitions
{
    NSParameterAssert(definitions);
    
    if((self = [super init])) {
        self.definitions = definitions;
        self.defaultAttributes = @{NSForegroundColorAttributeName: [NSColor blackColor]};
    }
    
    return self;
}

- (id)init
{
    [self doesNotRecognizeSelector:_cmd];
    return nil;
}

#pragma mark - Highlighting

- (void)highlightTextStorage:(NSTextStorage *)textStorage
{
    [textStorage addAttributes:_defaultAttributes range:NSMakeRange(0, textStorage.length)];
    
    for (SHDefinition *definition in self.definitions) {
        NSArray *matches = [definition.regex matchesInString:textStorage.string
                                                     options:kNilOptions
                                                       range:NSMakeRange(0, [textStorage length])];
        for (NSTextCheckingResult *match in matches) {
            NSDictionary *attributes = _attributes[definition.name] ?: _defaultAttributes;
            [textStorage addAttributes:attributes range:match.range];
        }
    }
}

@end
