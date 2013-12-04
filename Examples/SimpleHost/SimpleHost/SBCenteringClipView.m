//From <http://www.bergdesign.com/developer/index_files/88a764e343ce7190c4372d1425b3b6a3-0.html>

#import "SBCenteringClipView.h"

@implementation SBCenteringClipView {
    ///The proportion up and across the view (not coordinates.)
	NSPoint _lookingAt;
}

- (void)centerDocument
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

- (NSPoint)constrainScrollPoint:(NSPoint)proposedNewOrigin
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

- (void)viewBoundsChanged:(NSNotification *)notification
{
	NSPoint savedPoint = _lookingAt;
	[super viewBoundsChanged:notification];
	_lookingAt = savedPoint;
	[self centerDocument];
}

- (void)viewFrameChanged:(NSNotification *)notification
{
	NSPoint savedPoint = _lookingAt;
	[super viewFrameChanged:notification];
	_lookingAt = savedPoint;
	[self centerDocument];
    
    [self setNeedsDisplay:YES];
}

- (void)setFrame:(NSRect)frameRect
{
	[super setFrame:frameRect];
	[self centerDocument];
}

- (void)setFrameOrigin:(NSPoint)newOrigin
{
	[super setFrameOrigin:newOrigin];
	[self centerDocument];
}

- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	[self centerDocument];
}

- (void)setFrameRotation:(CGFloat)angle
{
	[super setFrameRotation:angle];
	[self centerDocument];
}

#pragma mark - Drawing Shadows

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    
    [NSGraphicsContext saveGraphicsState];
    {
        NSRect documentViewFrame = [[self documentView] frame];
        
        NSShadow *shadow = [NSShadow new];
        [shadow setShadowColor:[NSColor colorWithDeviceWhite:0.0 alpha:0.5]];
        [shadow setShadowBlurRadius:10.0];
        [shadow set];
        
        [[NSColor whiteColor] set];
        [NSBezierPath fillRect:documentViewFrame];
    }
    [NSGraphicsContext restoreGraphicsState];
}

@end
