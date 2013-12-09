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
    //We apply the default attributes to the entire text storage
    //before we do anything more specific. This allows the document
    //to have a sane appearance, even if there are no definitions
    //available.
    [textStorage addAttributes:_defaultAttributes range:NSMakeRange(0, textStorage.length)];
    
    
    //The actual highlighting is very simple. Each definition
    //has a name, and a precompiled regular expression object.
    //We just apply the regular expression, and then add the
    //attributes for whatever ranges match.
    for (SHDefinition *definition in self.definitions) {
        NSArray *matches = [definition.regex matchesInString:textStorage.string
                                                     options:kNilOptions
                                                       range:NSMakeRange(0, [textStorage length])];
        for (NSTextCheckingResult *match in matches) {
            NSDictionary *attributes = _attributes[definition.name];
            if(attributes)
                [textStorage addAttributes:attributes range:match.range];
        }
    }
}

@end
