// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




@interface NonnonPaintStub : NSView

@property (nonatomic,assign) id delegate;
@property (nonatomic,assign) id delegate_canvas;

@end


@implementation NonnonPaintStub


@synthesize delegate;
@synthesize delegate_canvas;


- init
{
	self = [super init];
	if ( self )
	{
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
	}

	return self;
}




- (void) updateTrackingAreas
{
//return;

	int options = (
		NSTrackingMouseEnteredAndExited |
		NSTrackingMouseMoved            |
		NSTrackingActiveAlways          |
		NSTrackingActiveInActiveApp
	);

	NSTrackingArea *trackingArea = [
		[NSTrackingArea alloc]
			initWithRect:[self bounds]
			     options:options
			       owner:self
			    userInfo:nil
	];

	[self addTrackingArea:trackingArea];

}

- (void) mouseEntered:(NSEvent *)theEvent
{
//NSLog(@"mouseEntered");

	// [Needed] : NSTrackingMouseEnteredAndExited

	n_paint *p = n_paint_global.paint;
	if ( p->xmouse_stop ) { return; }

	if ( FALSE == n_mac_window_is_keywindow( self.window ) )
	{
		[self.window makeKeyWindow];
	}

	[self.delegate_canvas resetCursorRects];

}

- (void) mouseExited:(NSEvent *)theEvent
{
//NSLog(@"mouseExited");

	// [Needed] : NSTrackingMouseEnteredAndExited

	[self.delegate_canvas resetCursorRects];

}



/*
	// [x] : not working

- (BOOL) acceptsFirstMouse:(NSEvent *)event
{
	return YES;
}

- (void) mouseDown:(NSEvent *)theEvent
{
//NSLog(@"XMouse : mouseDown");

	[self.delegate mouseDown:theEvent];

	[super mouseDown:theEvent];

}
*/




- (void) mouseUp:(NSEvent *)theEvent
{
//NSLog(@"XMouse : mouseUp");

	// [!] : scrollers use this for preventing press N run
	[self.delegate mouseUp:theEvent];

	[super mouseUp:theEvent];

}

- (void) mouseDragged:(NSEvent *)theEvent
{
//NSLog(@"! mouseDragged");

	// [!] : without this, mouse capture doesn't work

	[self.delegate mouseDragged:theEvent];


	// [Needed] : Tool Window Scroller : you cannot drag a thumb

	[super mouseDragged:theEvent];

}




- (void) otherMouseUp:(NSEvent *)theEvent
{
//NSLog(@"XMouse : mouseUp");

	n_paint *paint = n_paint_global.paint;

	paint->cursor_grab_n_drag_onoff = FALSE;
	[self resetCursorRects];

	//[self.delegate otherMouseUp:theEvent];

}
/*
- (void) otherMouseDragged:(NSEvent *)theEvent
{
//NSLog(@"XMouse : otherMouseDragged");

	[self.delegate otherMouseDragged:theEvent];

}
*/



-(BOOL) NonnonPaintDnDIsDisabled:( id <NSDraggingInfo> ) sender
{

	BOOL ret = FALSE;


	n_paint *p = n_paint_global.paint;


	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSString     *nsstr      = n_mac_alias_resolve( [[NSURL URLFromPasteboard:pasteboard] path] );
//NSLog( @"%@", nsstr );


	BOOL is_dir = NO;
	BOOL check  = [[NSFileManager defaultManager] fileExistsAtPath:nsstr isDirectory:&is_dir];

	check = ( check && is_dir );
//NSLog( @"%d", check );

//NSLog( @"%d %d", p->tooltype, p->grabber_mode );

	if ( p->tooltype == N_PAINT_TOOL_TYPE_GRABBER )
	{
		if ( check ) { ret = TRUE; }
	} else {
		if ( p->grabber_mode ) { return TRUE; }
		if ( check ) { return FALSE; }
	}


	if ( p->readonly                                ) { ret = TRUE; }
	if ( p->grabber_mode != N_PAINT_GRABBER_NEUTRAL ) { ret = TRUE; }
	if (
		( p->layer_onoff )
		&&
		(
			( p->layer_data[ p->layer_index ].visible == FALSE )
			||
			( n_paint_layer_is_locked( p->layer_index ) )
		)
	)
	{
		 ret = TRUE;
	}


	return ret;
}

-(NSDragOperation) draggingEntered:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingEntered" );


	// [!] : call when hovered

	if ( [self NonnonPaintDnDIsDisabled:sender] ) { return NSDragOperationNone; }


	return NSDragOperationCopy;
}

-(void) draggingExited:( id <NSDraggingInfo> )sender
{
//NSLog( @"draggingExited" );

}

-(BOOL) prepareForDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"prepareForDragOperation" );

	if ( [self NonnonPaintDnDIsDisabled:sender] ) { return NO; }

	return YES;
}

-(BOOL) performDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"performDragOperation" );

	if ( [self NonnonPaintDnDIsDisabled:sender] ) { return NO; }

	return YES;
}

-(void) concludeDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"concludeDragOperation" );

	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSString     *nsstr      = [[NSURL URLFromPasteboard:pasteboard] path];

	[self.delegate NonnonDragAndDrop_dropped:nsstr];

}

-(NSDragOperation) draggingUpdated:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingUpdated" );

	if ( [self NonnonPaintDnDIsDisabled:sender] ) { return NSDragOperationNone; }

	return NSDragOperationCopy;
}




@end


