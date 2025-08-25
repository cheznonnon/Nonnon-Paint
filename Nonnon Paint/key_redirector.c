// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




@interface NonnonPaintKeyRedirector : NSWindow

@end


@implementation NonnonPaintKeyRedirector




- (BOOL)acceptsFirstResponder
{
//NSLog(@"acceptsFirstResponder");
	return YES;
}

- (BOOL)becomeFirstResponder
{
//NSLog(@"becomeFirstResponder");
        return YES;
}

- (void) keyDown:(NSEvent *)theEvent
{
//NSLog(@"keyDown");

	[n_paint_global keyDown:theEvent];

}




@end


