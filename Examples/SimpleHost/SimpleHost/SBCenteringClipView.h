#import <Cocoa/Cocoa.h>

@interface SBCenteringClipView : NSClipView
{
	NSPoint _lookingAt; // the proportion up and across the view, not coordinates.
}

-(void)centerDocument;

@end
