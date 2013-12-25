//
//  GraphicsPortViewController.h
//  SimpleHost-iPhone
//
//  Created by Kevin MacWhinnie on 12/24/13.
//  Copyright (c) 2013 Roundabout Software, LLC. All rights reserved.
//

#import <UIKit/UIKit.h>

///The GraphicsPortViewController class contains the actual render
///portion of this example. It hosts a simple GFXView and displays
///errors in a UILabel.
@interface GraphicsPortViewController : UIViewController

///Initialize the receiver with some Gfx code to render.
///
/// \param  code    The code to render.
///
/// \result A fully initialized graphics port controller.
///
///The controller will not render the code until its view is loaded.
- (instancetype)initWithCode:(NSString *)code;

#pragma mark - Properties

///The graphics port view.
@property (nonatomic, readonly) GFXView *graphicsPortView;

///The code being rendered.
@property (nonatomic, readonly, copy) NSString *code;

@end
