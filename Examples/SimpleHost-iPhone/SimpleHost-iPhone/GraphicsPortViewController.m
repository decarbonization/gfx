//
//  GraphicsPortViewController.m
//  SimpleHost-iPhone
//
//  Created by Kevin MacWhinnie on 12/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "GraphicsPortViewController.h"

@interface GraphicsPortViewController () <GFXViewDelegate>

#pragma mark - readwrite

@property (nonatomic, readwrite) GFXView *graphicsPortView;
@property (nonatomic, readwrite, copy) NSString *code;

@end

@implementation GraphicsPortViewController

- (instancetype)initWithCode:(NSString *)code
{
    if((self = [super init])) {
        self.code = code;
    }
    
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.graphicsPortView = [[GFXView alloc] initWithFrame:self.view.bounds];
    self.graphicsPortView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.graphicsPortView.delegate = self;
    self.graphicsPortView.code = self.code;
    
    [self.view addSubview:self.graphicsPortView];
    
    UISwipeGestureRecognizer *backGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(goBack:)];
    backGesture.direction = UISwipeGestureRecognizerDirectionRight;
    [self.view addGestureRecognizer:backGesture];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    [self.navigationController setNavigationBarHidden:YES animated:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    [self.navigationController setNavigationBarHidden:NO animated:animated];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Actions

- (IBAction)goBack:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark - <GFXViewDelegate>

- (void)graphicsViewDidBeginRendering:(GFXView *)sender
{
}

- (void)graphicsViewDidFinishRendering:(GFXView *)sender
{
}

- (void)graphicsView:(GFXView *)sender didEncounterError:(NSError *)error
{
    [self.graphicsPortView removeFromSuperview];
    
    UILabel *errorLabel = [UILabel new];
    errorLabel.font = [UIFont boldSystemFontOfSize:15.0];
    errorLabel.lineBreakMode = NSLineBreakByWordWrapping;
    errorLabel.numberOfLines = 0;
    errorLabel.textAlignment = NSTextAlignmentCenter;
    errorLabel.text = [NSString stringWithFormat:NSLocalizedString(@"An error occurred:\n%@", @""),
                       [error localizedDescription] ?: NSLocalizedString(@"An unknown error occurred during rendering.", @"")];
    
    CGRect bounds = CGRectInset(self.view.bounds, 20.0, 20.0);
    
    CGRect errorLabelFrame = {};
    errorLabelFrame.size = [errorLabel sizeThatFits:bounds.size];
    errorLabelFrame.origin.x = round(CGRectGetMidX(bounds) - CGRectGetWidth(errorLabelFrame) / 2.0);
    errorLabelFrame.origin.y = round(CGRectGetMidY(bounds) - CGRectGetHeight(errorLabelFrame) / 2.0);
    errorLabel.frame = errorLabelFrame;
    errorLabel.autoresizingMask = (UIViewAutoresizingFlexibleTopMargin |
                                   UIViewAutoresizingFlexibleBottomMargin |
                                   UIViewAutoresizingFlexibleLeftMargin | 
                                   UIViewAutoresizingFlexibleRightMargin);
    
    [self.view addSubview:errorLabel];
}

@end
