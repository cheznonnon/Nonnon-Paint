// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File


// [!] : Usage
//
//	1 : replace your NSTextView to NonnonTextView
//	2 : IB : right pane : "Custom Class", "Class", set "NonnonTextView"
//	3 : modify behavior




#ifndef _H_NONNON_MAC_NONNON_TXTVIEW
#define _H_NONNON_MAC_NONNON_TXTVIEW




#import <Cocoa/Cocoa.h>




@interface NonnonTextView : NSTextView
@end


@interface NonnonTextView () <NSTextViewDelegate>
@end


@implementation NonnonTextView


- init
{
//NSLog(@"init");

	self = [super init];

	if ( self )
	{
		//[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
		[self setDelegate: self];

		[[NSNotificationCenter defaultCenter]
		      addObserver: self
			 selector: @selector( n_ViewDidChangeSelection: )
			     name: NSTextViewDidChangeSelectionNotification
			   object: nil
		];
	}

	return self;
}

- (id) initWithCoder : (NSCoder*) decoder
{

	self = [super initWithCoder:decoder];

	if ( self )
	{
		//[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
		[self setDelegate: self];

		[[NSNotificationCenter defaultCenter]
		      addObserver: self
			 selector: @selector( n_ViewDidChangeSelection: )
			     name: NSTextViewDidChangeSelectionNotification
			   object: nil
		];
	}

	return self;
}


- (void) controlTextDidChange : (NSNotification*) obj
{
//NSLog( @"%@", self.stringValue );

	//[self setStringValue:@""];

}


- (void)n_ViewDidChangeSelection:(NSNotification *)aNotification {

//NSLog( @"n_ViewDidChangeSelection" );

}


/*
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

	//[self setStringValue:@""];

	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSString     *nsstr      = [[NSURL URLFromPasteboard:pasteboard] path];

	[self setStringValue:nsstr];

}

-(NSDragOperation) draggingUpdated:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingUpdated" );

	return NSDragOperationCopy;
}
*/
@end




#endif // _H_NONNON_MAC_NONNON_TXTVIEW


