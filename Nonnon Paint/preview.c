// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




static n_bmp      n_paint_preview_bmp;
static n_type_gfx n_paint_preview_default_sx = -1;
static n_type_gfx n_paint_preview_default_sy = -1;




void
n_n_paint_preview_grid( n_paint *paint )
{

	if ( paint->grid_onoff == n_posix_false ) { return; }


	n_type_gfx sx = N_BMP_SX( &n_paint_preview_bmp );
	n_type_gfx sy = N_BMP_SY( &n_paint_preview_bmp );

	u32 blue = n_bmp_rgb_mac( 0,200,255 );
	u32 pink = n_bmp_rgb_mac( 255,0,128 );

	n_type_real blend = 0.25;

	n_type_gfx grid_size = n_posix_max( sx, sy ) / 8;

	n_type_gfx grid_sx = grid_size;
	n_type_gfx grid_sy = grid_size;

	n_type_gfx gfx = sx / 2;
	n_type_gfx gtx = sx / 2;
	n_posix_loop
	{
		gfx -= grid_sx;
		gtx += grid_sx;

		n_paint_bmp_line( &n_paint_preview_bmp, gfx  , 0, gfx  , sy, blue, blend );
		n_paint_bmp_line( &n_paint_preview_bmp, gtx  , 0, gtx  , sy, blue, blend );

		n_paint_bmp_line( &n_paint_preview_bmp, gfx-1, 0, gfx-1, sy, blue, blend );
		n_paint_bmp_line( &n_paint_preview_bmp, gtx-1, 0, gtx-1, sy, blue, blend );

		if ( gfx < 0 ) { break; }
	}

	n_type_gfx gfy = sy / 2;
	n_type_gfx gty = sy / 2;
	n_posix_loop
	{
		gfy -= grid_sy;
		gty += grid_sy;

		n_paint_bmp_line( &n_paint_preview_bmp, 0, gfy  , sx, gfy  , blue, blend );
		n_paint_bmp_line( &n_paint_preview_bmp, 0, gty  , sx, gty  , blue, blend );

		n_paint_bmp_line( &n_paint_preview_bmp, 0, gfy-1, sx, gfy-1, blue, blend );
		n_paint_bmp_line( &n_paint_preview_bmp, 0, gty-1, sx, gty-1, blue, blend );

		if ( gfy < 0 ) { break; }
	}

	n_paint_bmp_line( &n_paint_preview_bmp, sx / 2, 0, sx / 2, sy, pink, 0.25 );
	n_paint_bmp_line( &n_paint_preview_bmp, 0, sy / 2, sx, sy / 2, pink, 0.25 );


	return;
}

// internal
void
n_paint_preview_cache_exit( n_paint *paint )
{

	n_bmp_free( &n_paint_preview_bmp );


	return;
}

// internal
void
n_paint_preview_cache_init( n_paint *paint, NSWindow *window, NSView *view )
{

	n_bmp_zero( &n_paint_preview_bmp );


	if ( paint->layer_onoff )
	{
		n_paint_layer_save_as_one( paint, &n_paint_preview_bmp );
	} else {
		n_bmp_carboncopy( paint->pen_bmp_data, &n_paint_preview_bmp );
		n_paint_grabber_composition( paint->pen_bmp_grab, &n_paint_preview_bmp, n_posix_true, n_posix_true );
	}

	n_bmp_flush_mirror( &n_paint_preview_bmp, N_BMP_MIRROR_UPSIDE_DOWN );


	CGFloat desktop_sx, desktop_sy;
	n_mac_desktop_size( &desktop_sx, &desktop_sy );

	n_type_gfx csx = (n_type_gfx) ( (n_type_real) desktop_sx * 0.9 );
	n_type_gfx csy = (n_type_gfx) ( (n_type_real) desktop_sy * 0.9 );


	n_type_gfx bmpsx = N_BMP_SX( paint->pen_bmp_data );
	n_type_gfx bmpsy = N_BMP_SY( paint->pen_bmp_data );


	n_type_gfx cs  = n_posix_min( csx, csy );

	if ( ( cs > bmpsx )&&( cs > bmpsy ) )
	{
//NSLog( @"Black Margin" );

		n_n_paint_preview_grid( paint );
		n_bmp_resizer( &n_paint_preview_bmp, cs, cs, n_bmp_black, N_BMP_RESIZER_CENTER );

	} else
	if ( bmpsx == bmpsy )
	{

		if ( ( cs < bmpsx )||( cs < bmpsy ) )
		{
			n_type_real ratio_x = (n_type_real) cs / bmpsx;
			n_type_real ratio_y = (n_type_real) cs / bmpsy;

			n_bmp_resampler( &n_paint_preview_bmp, ratio_x, ratio_y );
			n_n_paint_preview_grid( paint );
		}

	} else
	if ( bmpsx < bmpsy )
	{
//NSLog( @"Vert" );

		if ( ( cs < bmpsx )||( cs < bmpsy ) )
		{
			n_type_real ratio = (n_type_real) csy / bmpsy;

			n_bmp_resampler( &n_paint_preview_bmp, ratio, ratio );
			n_n_paint_preview_grid( paint );
		}

	} else
	if ( bmpsx > bmpsy )
	{

		if ( ( cs < bmpsx )||( cs < bmpsy ) )
		{
			n_type_real ratio = (n_type_real) csx / bmpsx;

			n_bmp_resampler( &n_paint_preview_bmp, ratio, ratio );
			n_n_paint_preview_grid( paint );
		}

	}


	{
		n_type_gfx sx = N_BMP_SX( &n_paint_preview_bmp );
		n_type_gfx sy = N_BMP_SY( &n_paint_preview_bmp );

		[window setContentSize:NSMakeSize( sx,sy )];
		[view setFrame:NSMakeRect( 0,0,sx,sy )];

		n_paint_preview_default_sx = sx;
		n_paint_preview_default_sy = sy;
	}


	[view display];


	return;
}




@interface NonnonPaintPreview : NSView

@property (nonatomic,assign) id delegate;

@end


@implementation NonnonPaintPreview


@synthesize delegate;




- (instancetype)initWithCoder:(NSCoder *)coder
{
	self = [super initWithCoder:coder];

	if ( self )
	{
		n_bmp_zero( &n_paint_preview_bmp );
	}

	return self;
}

-(void) drawRect:(NSRect) rect
{

/*
	n_win_hwndprintf_literal
	(
		hwnd,
		"Preview : X %dpx (%d%%) : Y %dpx (%d%%)",
		tsx - n_paint_preview_default_sx,
		(int) trunc( (n_type_real) tsx / n_paint_preview_default_sx * 100 ),
		tsy - n_paint_preview_default_sy,
		(int) trunc( (n_type_real) tsy / n_paint_preview_default_sy * 100 )
	);
*/


	// [x] : Sonoma : problem happens

	rect = [super bounds];


	n_mac_image_nbmp_direct_draw( &n_paint_preview_bmp, &rect, NO );

}




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

- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Key Code = %d", event.keyCode );

	switch( event.keyCode ) {

	case N_MAC_KEYCODE_NUMBER_1: 

		[self.delegate keyDown:event];

	break;

	} // switch

}


- (void) mouseUp:(NSEvent *)theEvent
{
//NSLog(@"mouseUp");

	if ( [theEvent clickCount] == 2 )
	{

		[self.delegate mouseUp:theEvent];

	}

}


@end
