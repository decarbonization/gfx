//
//  AppDelegate.m
//  SimpleHost-iPhone
//
//  Created by Kevin MacWhinnie on 12/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import "AppDelegate.h"
#import "EditorViewController.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.backgroundColor = [UIColor whiteColor];
    
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:[EditorViewController new]];
    navigationController.navigationBar.barStyle = UIBarStyleBlack;
    navigationController.view.tintColor = [UIColor colorWithRed:0.90 green:0.63 blue:0.09 alpha:1.00];
    self.window.rootViewController = navigationController;
    
    [self.window makeKeyAndVisible];
    return YES;
}

@end
