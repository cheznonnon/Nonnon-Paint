// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [x] : gray-out at lost focus : un-implementable : system should do




#ifndef _H_NONNON_MAC_N_LABEL
#define _H_NONNON_MAC_N_LABEL




#import <Cocoa/Cocoa.h>




@interface NonnonLabel : NSView

@property NSString *string;

@end




@implementation NonnonLabel {

	BOOL gray_onoff;

}

@synthesize string;

- (instancetype)initWithCoder:(NSCoder *)coder
{
	self = [super initWithCoder:coder];

	if ( self )
	{
		string = @"";

		gray_onoff = FALSE;

		[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector( applicationWillBecomeActive: )
			       name:NSApplicationDidBecomeActiveNotification
			     object:nil
		];

		[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector( applicationWillResignActive: )
			       name:NSApplicationDidResignActiveNotification
			     object:nil
		];
	}

	return self;
}

- (void) applicationWillBecomeActive:(NSNotification *)notification
{
//NSLog( @"applicationWillBecomeActive" );

	gray_onoff = n_posix_false;

	[self display];

}

- (void) applicationWillResignActive:(NSNotification *)notification
{
//NSLog( @"applicationWillResignActive" );

	gray_onoff = n_posix_true;

	[self display];

}
-(void) drawRect:(NSRect) rect
{

	NSMutableDictionary *attr = [NSMutableDictionary dictionary];

	NSColor *color;
	if ( gray_onoff )
	{
		color = [NSColor systemGrayColor];
	} else {
		color = [NSColor textColor];
	}
	[attr setObject:color forKey:NSForegroundColorAttributeName];

	[string drawInRect:[super bounds] withAttributes:attr];

}

@end




#endif // _H_NONNON_MAC_N_LABEL


