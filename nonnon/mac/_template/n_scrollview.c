// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File




#ifndef _H_NONNON_MAC_NONNON_SCROLLVIEW
#define _H_NONNON_MAC_NONNON_SCROLLVIEW




#import <Cocoa/Cocoa.h>


#include "_mac.c"




@interface NonnonScrollView : NSScrollView

@property (nonatomic,assign) id delegate_draganddrop;
@property (nonatomic,assign) id delegate_doubleclick;

@end


@implementation NonnonScrollView {

	CGPoint pt;

}


@synthesize delegate_draganddrop;
@synthesize delegate_doubleclick;


- init {
//NSLog(@"init : scroll");

	self = [super init];

	if ( self )
	{
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
	}

	return self;
}


-(NSDragOperation) draggingEntered:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingEntered" );

	// [!] : call when hovered

	return NSDragOperationCopy;
}

-(void) draggingExited:( id <NSDraggingInfo> )sender
{
//NSLog( @"draggingExited" );
}

-(BOOL) prepareForDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"prepareForDragOperation" );

	return YES;
}

-(BOOL) performDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"performDragOperation" );

	return YES;
}

-(void) concludeDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"concludeDragOperation" );

	// [!] : call when dropped

	//NSArray *array = [[sender draggingPasteboard] propertyListForType:NSPasteboardTypeFileURL];

	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSString     *nsstr      = [[NSURL URLFromPasteboard:pasteboard] path];

//NSLog( @"%@", nsstr );

	[self.delegate_draganddrop NonnonDragAndDrop_dropped:nsstr];

}

-(NSDragOperation) draggingUpdated:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingUpdated" );

	return NSDragOperationCopy;
}


- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog(@"mouseUp");

	[delegate_doubleclick mouseUp:theEvent];

}


- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");

	pt = [theEvent locationInWindow];

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog(@"mouseDragged");

	CGPoint pt_cur = [theEvent locationInWindow];

	CGFloat dx = pt.x - pt_cur.x;
	CGFloat dy = pt.y - pt_cur.y;

	pt = pt_cur;
//NSLog(@"%f %f", dx, dy);

	NSPoint pt_scr = [self.contentView bounds].origin;

	pt_scr.x += dx;
	pt_scr.y += dy;

	pt_scr.x = MAX( 0, pt_scr.x );
	pt_scr.y = MAX( 0, pt_scr.y );

	CGFloat max_sx = self.documentView.bounds.size.width - self.contentView.bounds.size.width;
	if ( pt_scr.x > max_sx ) { pt_scr.x = max_sx; }

	CGFloat max_sy = self.documentView.bounds.size.height - self.contentView.bounds.size.height;
	if ( pt_scr.y > max_sy ) { pt_scr.y = max_sy; }

	[self.contentView scrollToPoint:pt_scr];
	[self reflectScrolledClipView:[self contentView]];

}

@end




NonnonScrollView*
n_mac_nonnon_scrollview( id self, NSWindow *window, NSImageView *imageview )
{

	NonnonScrollView *scroll;

	scroll = [[NonnonScrollView alloc] init];

	[scroll setFrame:[[window contentView] frame]];
	[scroll setBorderType:NSNoBorder];

	[scroll setHasVerticalScroller:NO];
	[scroll setHasHorizontalScroller:NO];

	[scroll setDocumentView:imageview];


	[window setContentView:scroll];


	scroll.delegate_draganddrop = self;
	scroll.delegate_doubleclick = self;


	return scroll;
}




#endif // _H_NONNON_MAC_NONNON_SCROLL


