#import "SBCenteringClipView.h"

@implementation SBCenteringClipView

// ----------------------------------------

-(void)centerDocument
{
	NSRect docRect = [[self documentView] frame];
	NSRect clipRect = [self bounds];
    
	// The origin point should have integral values or drawing anomalies will occur.
	// We'll leave it to the constrainScrollPoint: method to do it for us.
	if( docRect.size.width < clipRect.size.width )
		clipRect.origin.x = ( docRect.size.width - clipRect.size.width ) / 2.0;
	else
		clipRect.origin.x = _lookingAt.x * docRect.size.width - ( clipRect.size.width / 2.0 );
    
	if( docRect.size.height < clipRect.size.height )
		clipRect.origin.y = ( docRect.size.height - clipRect.size.height ) / 2.0;
	else
		clipRect.origin.y = _lookingAt.y * docRect.size.height - ( clipRect.size.height / 2.0 );
    
	// Probably the best way to move the bounds origin.
	// Make sure that the scrollToPoint contains integer values
	// or the NSView will smear the drawing under certain circumstances.
    
	[self scrollToPoint:[self constrainScrollPoint:clipRect.origin]];
	[[self superview] reflectScrolledClipView:self];
    
	// We could use this instead since it allows a scroll view
	// to coordinate scrolling between multiple clip views.
	// [[self superview] scrollClipView:self toPoint:[self constrainScrollPoint:clipRect.origin]];
}

// ----------------------------------------
// We need to override this so that the superclass doesn't override our new origin point.

-(NSPoint)constrainScrollPoint:(NSPoint)proposedNewOrigin
{
	NSRect docRect = [[self documentView] frame];
	NSRect clipRect = [self bounds];
	float maxX = docRect.size.width - clipRect.size.width;
	float maxY = docRect.size.height - clipRect.size.height;
    
	clipRect.origin = proposedNewOrigin; // shift origin to proposed location
    
	// If the clip view is wider than the doc, we can't scroll horizontally
	if( docRect.size.width < clipRect.size.width )
		clipRect.origin.x = round( maxX / 2.0 );
	else
		clipRect.origin.x = round( MAX(0,MIN(clipRect.origin.x,maxX)) );
    
	// If the clip view is taller than the doc, we can't scroll vertically
	if( docRect.size.height < clipRect.size.height )
		clipRect.origin.y = round( maxY / 2.0 );
	else
		clipRect.origin.y = round( MAX(0,MIN(clipRect.origin.y,maxY)) );
    
	// Save center of view as proportions so we can later tell where the user was focused.
	_lookingAt.x = NSMidX(clipRect) / docRect.size.width;
	_lookingAt.y = NSMidY(clipRect) / docRect.size.height;
    
	return clipRect.origin;
}

// ----------------------------------------
// These two methods get called whenever the NSClipView's subview changes.
// We save the old center of interest, call the superclass to let it do its work,
// then move the scroll point to try and put the old center of interest
// back in the center of the view if possible.

-(void)viewBoundsChanged:(NSNotification *)notification
{
	NSPoint savedPoint = _lookingAt;
	[super viewBoundsChanged:notification];
	_lookingAt = savedPoint;
	[self centerDocument];
}

-(void)viewFrameChanged:(NSNotification *)notification
{
	NSPoint savedPoint = _lookingAt;
	[super viewFrameChanged:notification];
	_lookingAt = savedPoint;
	[self centerDocument];
}

// ----------------------------------------
// These NSClipView superclass methods change the bounds rect
// directly without sending any notifications,
// so we're not sure what other work they silently do for us.
// As a result, we let them do their
// work and then swoop in behind to change the bounds origin ourselves.
// This appears to work just fine without us having to
// reinvent the methods from scratch.
// ---
// Even though an NSView posts an NSViewFrameDidChangeNotification to the default notification center
// if it's configured to do so, NSClipViews appear to be configured not to. The methods
// setPostsFrameChangedNotifications: and setPostsBoundsChangedNotifications: appear
// to be configured not to send notifications.
// ---
// We have some redundancy in the fact that setFrame: appears to call/send setFrameOrigin:
// and setFrameSize: to do its work, but we need to override these individual methods in case
// either one gets called independently. Because none of them explicitly cause a screen update,
// it's ok to do a little extra work behind the scenes because it wastes very little time.
// It's probably the result of a single UI action anyway so it's not like it's slowing
// down a huge iteration by being called thousands of times.

-(void)setFrame:(NSRect)frameRect
{
	[super setFrame:frameRect];
	[self centerDocument];
}

-(void)setFrameOrigin:(NSPoint)newOrigin
{
	[super setFrameOrigin:newOrigin];
	[self centerDocument];
}

-(void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	[self centerDocument];
}

-(void)setFrameRotation:(CGFloat)angle
{
	[super setFrameRotation:angle];
	[self centerDocument];
}

@end