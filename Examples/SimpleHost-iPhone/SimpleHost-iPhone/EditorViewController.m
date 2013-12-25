//
//  EditorViewController.m
//  SimpleHost-iPhone
//
//  Created by Kevin MacWhinnie on 12/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "EditorViewController.h"
#import "GraphicsPortViewController.h"

@interface EditorViewController () <UITextViewDelegate>

///The text view where the user inputs source code.
@property (nonatomic) UITextView *textView;

@end

@implementation EditorViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.title = NSLocalizedString(@"Editor", @"");
    self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:NSLocalizedString(@"Render", @"")
                                                                              style:UIBarButtonItemStyleBordered
                                                                             target:self
                                                                             action:@selector(showRenderedImage:)];
    
    self.textView = [[UITextView alloc] initWithFrame:self.view.bounds];
    self.textView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.textView.delegate = self;
    self.textView.backgroundColor = [UIColor colorWithRed:0.15 green:0.16 blue:0.18 alpha:1.00];
    self.textView.textColor = [UIColor colorWithRed:0.92 green:0.92 blue:0.92 alpha:1.00];
    self.textView.font = [UIFont fontWithName:@"Courier" size:13.0];
    self.textView.tintColor = [UIColor whiteColor];
    self.textView.autocapitalizationType = UITextAutocapitalizationTypeNone;
    self.textView.autocorrectionType = UITextAutocorrectionTypeNo;
    self.textView.keyboardAppearance = UIKeyboardAppearanceDark;
    
    NSURL *defaultSourceLocation = [[NSBundle mainBundle] URLForResource:@"default" withExtension:@"gfx"];
    NSError *error = nil;
    NSString *defaultSource = [NSString stringWithContentsOfURL:defaultSourceLocation encoding:NSUTF8StringEncoding error:&error];
    NSAssert(defaultSource != nil, @"Could not load default source");
    self.textView.text = defaultSource;
    
    [self.view addSubview:self.textView];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [self.textView becomeFirstResponder];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Actions

- (IBAction)showRenderedImage:(id)sender
{
    GraphicsPortViewController *graphicsPortController = [[GraphicsPortViewController alloc] initWithCode:self.textView.text];
    [self.navigationController pushViewController:graphicsPortController animated:YES];
}

@end
