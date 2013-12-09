//From <http://www.bergdesign.com/developer/index_files/88a764e343ce7190c4372d1425b3b6a3-0.html>

#import <Cocoa/Cocoa.h>

///The SBCenteringClipView class modifies NSClipView to always center
///its document view. It also adds a drop shadow to the document view.
@interface SBCenteringClipView : NSClipView

- (void)centerDocument;

@end
