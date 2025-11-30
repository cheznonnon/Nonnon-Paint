// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File




#ifndef _H_NONNON_MAC_NONNON_PAINT_CANVAS
#define _H_NONNON_MAC_NONNON_PAINT_CANVAS




//#import <Cocoa/Cocoa.h>




//#include "../../nonnon/mac/_mac.c"
//#include "../../nonnon/mac/image.c"




#include "extern.c"




#define N_PAINT_CANVAS_MULTITHREAD




static n_bmp  n_paint_target;
static n_bmp *n_paint_canvas = NULL;




@protocol NonnonPaint_delegate

- (void) NonnonPaintResize;
- (void) NonnonPaintColorSet;
- (void) NonnonPaintStatus;
- (void) NonnonScrollbarZoomSync:(BOOL)redraw;

@end




@interface NonnonPaintCanvas : NSView

@property (nonatomic,assign) id delegate;

@property n_paint *paint;
@property n_bmp    bmp_canvas;
@property CGFloat  pressure;

@property NonnonTxtbox *txtbox;

- (NSPoint) n_paint_point_on_bitmap;
- (int)     n_paint_zoom_get_int:(int) zoom;
- (int)     n_paint_zoom_get_int_ui:(int) zoom;
- (void)    n_paint_convert_bitmap2canvas:(void*) zero x:(n_type_gfx*)x y:(n_type_gfx*)y  sx:(n_type_gfx*)sx sy:(n_type_gfx*)sy;
- (void)    n_paint_convert_bitmap2canvas_grabber_frame:(void*) zero x:(n_type_gfx*)x y:(n_type_gfx*)y  sx:(n_type_gfx*)sx sy:(n_type_gfx*)sy;
- (void)    display_optimized;
- (n_bmp*)  n_paint_nbmp_get;
- (void)    n_paint_draw_tooltip;
- (void)    n_paint_layer_window_onoff:(BOOL) onoff;

- (void)    n_paint_grabber_frame_anim_off;

@end




static NonnonPaintCanvas *n_paint_global;




n_posix_bool
n_paint_layer_is_locked( n_type_int i )
{

	n_paint *p = n_paint_global.paint;


	if ( p->layer_onoff == n_posix_false ) { return n_posix_false; }

	if ( p->layer_data[ i ].visible == n_posix_false ) { return n_posix_true; }


	n_posix_char *str = n_txt_get( &p->layer_txt, i );


	return ( str[ 0 ] == n_posix_literal( '*' ) );
}




void
n_paint_indicator( void )
{

	u32 color;// = n_mac_nscolor2argb( [NSColor controlAccentColor] );
	if ( n_paint_global.paint->grabber_mode )
	{
		color = n_bmp_rgb_mac( 255,0,128 );
	} else
	if ( n_paint_global.paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
	{
		color = N_PAINT_CANVAS_COLOR;
	} else
	if ( n_paint_global.paint->tooltype == N_PAINT_TOOL_TYPE_FILL )
	{
		color = n_bmp_rgb_mac( 255,255,0 );
	} else
	if ( n_paint_global.paint->tooltype == N_PAINT_TOOL_TYPE_GRABBER )
	{
		color = n_bmp_rgb_mac( 0,200,255 );
	} else {
		color = N_PAINT_CANVAS_COLOR;
	}


	n_bmp *bmp = [n_paint_global n_paint_nbmp_get];

	n_bmp_box( bmp, 0,0,N_BMP_SX( bmp ), 2, color );


	return;
}




// [!] : Grabber Frame : Shared

#define N_PAINT_GRABBER_FRAME_ANIM_INTERVAL ( 50 )

static NSTimer      *n_paint_grabber_frame_anim_timer =           nil;
static n_posix_bool  n_paint_grabber_frame_anim_onoff = n_posix_false;




#include "grabber.c"
#include "pen.c"

#include "grabber_frame.c"

#include "layer.c"




@implementation NonnonPaintCanvas {

	NSBitmapImageRep *rep;

}


@synthesize delegate;

@synthesize paint;
@synthesize bmp_canvas;
@synthesize pressure;

@synthesize txtbox;




- (instancetype)initWithCoder:(NSCoder *)coder
{
	self = [super initWithCoder:coder];

	if ( self )
	{
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];

		n_bmp_zero( &bmp_canvas );

		n_paint_global = self;


		n_mac_timer_init( n_paint_global, @selector( n_paint_cursor_apply_timer_method ), 100 );

	}

	return self;
}




- (void) n_paint_cursor_apply_timer_method
{

	if ( paint->pen_quick_eraser_onoff )
	{
		if ( FALSE == n_mac_keystate_get( N_MAC_KEYCODE_UNDO ) )
		{
			paint->pen_quick_eraser_onoff = FALSE;
			paint->pen_start              = n_posix_false;
		}
	}

	if ( paint->pen_quick_blur_onoff )
	{
		if ( FALSE == n_mac_keystate_get( N_MAC_KEYCODE_CUT ) )
		{
			paint->pen_quick_blur_onoff = FALSE;
		}
	}

	if ( paint->pen_quick_eraser_whole_layer_onoff )
	{
		if ( FALSE == n_mac_keystate_get( N_MAC_KEYCODE_COPY ) )
		{
			paint->pen_quick_eraser_whole_layer_onoff = FALSE;
		}
	}

	if ( paint->grabber_frame_hide )
	{
		if ( FALSE == n_mac_keystate_get( N_MAC_KEYCODE_UNDO ) )
		{
			paint->grabber_frame_hide = FALSE;
		}
	}


	[self resetCursorRects];

}




- (BOOL) isFlipped
{
	return YES;
}

- (BOOL) acceptsFirstMouse:(NSEvent *)event
{
//NSLog( @"acceptsFirstMouse" );

	return YES;
}




- (n_bmp*) n_paint_nbmp_get
{
	return &bmp_canvas;
}

- (void) n_paint_picked_color_set:(u32) color
{
	paint->color = color;

	[self.delegate NonnonPaintColorSet];
}

- (void) n_paint_layer_window_onoff:(BOOL) onoff
{

	if ( onoff )
	{
		paint->layer_txt.readonly = FALSE;
		[self.txtbox display];

		[paint->whole_preview_checkbox setEnabled:YES];
		[paint->whole_grab_checkbox    setEnabled:YES];
	} else {
		paint->layer_txt.readonly = TRUE;
		[self.txtbox display];

		[paint->whole_preview_checkbox setEnabled:NO];
		[paint->whole_grab_checkbox    setEnabled:NO];
	}

}




- (void) n_paint_canvas_reset_cache
{

	if ( paint->layer_data == NULL ) { return; }
	if ( n_bmp_error( &paint->layer_data[ 0 ].bmp_data ) ) { return; }


	static n_type_gfx psx = -1;
	static n_type_gfx psy = -1;

	n_type_gfx bmpsx = N_BMP_SX( &paint->layer_data[ 0 ].bmp_data );
	n_type_gfx bmpsy = N_BMP_SY( &paint->layer_data[ 0 ].bmp_data );

	if ( ( psx != bmpsx )||( psy != bmpsy ) )
	{
		n_bmp_new_fast( &paint->layer_cache_bmp_data, bmpsx, bmpsy );

		n_bmp_new_fast( &paint->layer_cache_bmp_pre, bmpsx, bmpsy );
		n_bmp_new_fast( &paint->layer_cache_bmp_pst, bmpsx, bmpsy );
	}

	psx = bmpsx;
	psy = bmpsy;


	n_bmp_flush( &paint->layer_cache_bmp_data, 0 );

	n_bmp_flush( &paint->layer_cache_bmp_pre, 0 );
	n_bmp_flush( &paint->layer_cache_bmp_pst, 0 );

}

- (void) n_paint_canvas_reset_cache_layer
{

	n_bmp_flush( &paint->layer_cache_bmp_pre, 0 );
	n_bmp_flush( &paint->layer_cache_bmp_pst, 0 );

}

- (void) display_optimized
{

	[self n_paint_canvas_reset_cache];

	[self display];

}




- (void) n_paint_draw_tooltip
{
//NSLog( @"%d %d %d", paint->grabber_mode, paint->grabber_is_key_input, paint->grabber_is_key_input_fade );

//static int i = 0;
	if (
		( paint->grabber_mode == N_PAINT_GRABBER_SELECTING )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_DRAGGING )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_TRANSFORM )
		||
		( paint->grabber_is_key_input )
		||
		( paint->grabber_is_key_input_fade )
	)
	{
//NSLog( @"%d", i ); i++;

		n_posix_char str[ 100 ];
		{
			n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );
//NSLog( @"%d %d %d %d", gx, gy, gsx, gsy );

			if ( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
			{
				n_type_real ratio = (n_type_real) sx / paint->grabber_stretch_sx;

				n_posix_sprintf_literal( str, "%0.0f%%", ratio * 100 );
			} else
			if ( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_TRANSFORM )
			{
				n_type_real ratio_x = (n_type_real) sx / paint->grabber_stretch_sx;
				n_type_real ratio_y = (n_type_real) sy / paint->grabber_stretch_sy;

				n_posix_sprintf_literal( str, "%0.0f%% %0.0f%%", ratio_x * 100, ratio_y * 100 );
			} else
			if ( paint->grabber_mode == N_PAINT_GRABBER_SELECTING )
			{
				n_posix_sprintf_literal( str, "%d x %d", sx,sy );
			} else
			if ( paint->grabber_mode == N_PAINT_GRABBER_DRAGGING )
			{
				n_posix_sprintf_literal( str, "X %d, Y %d", x,y );
			} else
			if (
				( paint->grabber_is_key_input )
				||
				( paint->grabber_is_key_input_fade )
			)
			{
				n_posix_sprintf_literal( str, "X %d, Y %d", x,y );
			} else {
				n_posix_sprintf_literal( str, "X %d, Y %d", x,y );
			}
		}


		n_gdi gdi; n_gdi_zero( &gdi );

		gdi.sx                 = 0;
		gdi.sy                 = 0;
		gdi.style              = 0;//N_GDI_SMOOTH;

		gdi.base_color_bg      = N_PAINT_CANVAS_COLOR;
		gdi.base_color_fg      = N_PAINT_CANVAS_COLOR;
		gdi.base_style         = N_GDI_BASE_SOLID;

		gdi.frame_style        = N_GDI_FRAME_ROUND;//N_GDI_FRAME_SIMPLE;

		gdi.text               = str;
		gdi.text_font          = n_posix_literal( "Trebuchet MS" );
		gdi.text_size          = 14;
		gdi.text_color_main    = n_bmp_rgb_mac( 255,255,255 );
		gdi.text_style         = N_GDI_TEXT_MAC_NO_CROP;
		gdi.text_fxsize2       = 0;


		n_bmp bmp; n_bmp_zero( &bmp ); n_gdi_bmp( &gdi, &bmp );
//n_bmp_save( &bmp, "/Users/nonnon/Desktop/ret.bmp" );


		n_type_gfx cx;
		n_type_gfx cy;

		// [!] : Centering

		n_type_gfx bmpsx = N_BMP_SX( n_paint_canvas );
		n_type_gfx bmpsy = N_BMP_SY( n_paint_canvas );

		cx = ( ( bmpsx - gdi.sx ) / 2 );
		cy = ( ( bmpsy - gdi.sy ) / 2 );


		n_bmp_transcopy( &bmp, n_paint_canvas, 0,0,gdi.sx,gdi.sy, cx,cy );

		paint->grabber_tooltip_rect_drawn = NSMakeRect( cx, cy, gdi.sx, gdi.sy );


		n_bmp_free_fast( &bmp );

//NSLog( @"%d %d", gdi.sx, gdi.sy );
	}

}




- (void) n_paint_draw:(void*) zero sx:(n_type_gfx)sx sy:(n_type_gfx)sy
{

	BOOL is_zoom_in = ( paint->zoom > 0 );
//NSLog( @"Is Zoom In %d : %d", is_zoom_in, paint->zoom );

	int zoom    = [self n_paint_zoom_get_int   :paint->zoom];
	int zoom_ui = [self n_paint_zoom_get_int_ui:paint->zoom];
//NSLog( @"Zoom %d : Zoom UI %d", zoom, zoom_ui );

	n_type_gfx bmpsx = N_BMP_SX( paint->pen_bmp_data );
	n_type_gfx bmpsy = N_BMP_SY( paint->pen_bmp_data );

	if ( is_zoom_in )
	{
		bmpsx *= zoom;
		bmpsy *= zoom;
	} else {
		bmpsx /= zoom;
		bmpsy /= zoom;
	}

	n_type_gfx fx  = 0;
	n_type_gfx fy  = 0;
	n_type_gfx fsx = bmpsx;
	n_type_gfx fsy = bmpsy;
	n_type_gfx tx  = 0;
	n_type_gfx ty  = 0;

	if ( fsx > paint->inner_sx )
	{
		fx  = paint->scroll.x;
		fsx = paint->inner_sx;
	}

	if ( fsy > paint->inner_sy )
	{
		fy  = paint->scroll.y;
		fsy = paint->inner_sy;
	}

	if ( paint->margin_onoff )
	{
		paint->canvas_offset_x = tx = ( sx - fsx ) / 2;
		paint->canvas_offset_y = ty = ( sy - fsy ) / 2;

		paint->canvas_offset_sx = (CGFloat) fsx;
		paint->canvas_offset_sy = (CGFloat) fsy;
	}


	n_type_gfx gx,gy,gsx,gsy; n_paint_grabber_system_get( &gx,&gy, &gsx,&gsy, NULL,NULL );
//NSLog( @"%d %d %d %d", gx, gy, gsx, gsy );


	// [!] : Grabber : out-of-bound shadow

	if ( paint->grabber_mode )
	{
		n_type_gfx  x = gx;
		n_type_gfx  y = gy;
		n_type_gfx sx = gsx;
		n_type_gfx sy = gsy;

		if ( is_zoom_in )
		{
			 x *= paint->zoom;
			 y *= paint->zoom;
			sx *= paint->zoom;
			sy *= paint->zoom;
		} else {
			 x /= paint->zoom * -1;
			 y /= paint->zoom * -1;
			sx /= paint->zoom * -1;
			sy /= paint->zoom * -1;
		}

		x += tx; x -= paint->scroll.x;
		y += ty; y -= paint->scroll.y;

//NSLog( @"%d %d %d %d", x, y, sx, sy );

		n_bmp_box( n_paint_canvas, x,y,sx,sy, n_bmp_rgb_mac( 111,111,111 ) );
	}


	// [!] : redraw area

	n_type_gfx rx;
	n_type_gfx ry;
	n_type_gfx rsx;
	n_type_gfx rsy;

	n_mac_rect_expand_size( paint->rect_redraw, &rx, &ry, &rsx, &rsy );

	rx  -= zoom_ui * 1;
	ry  -= zoom_ui * 1;
	rsx += zoom_ui * 2;
	rsy += zoom_ui * 2;

//rx = 0;
//ry = 0;
//rsx = N_BMP_SX( &bmp_canvas );
//rsy = N_BMP_SY( &bmp_canvas );


	// [!] : Grid

	n_type_gfx grid_bmpsx = N_BMP_SX( paint->pen_bmp_data );
	n_type_gfx grid_bmpsy = N_BMP_SY( paint->pen_bmp_data );


	BOOL grid_is_odd_x = ( grid_bmpsx % 2 );
	BOOL grid_is_odd_y = ( grid_bmpsy % 2 );


	n_type_gfx center_x = grid_bmpsx / 2;
	n_type_gfx center_y = grid_bmpsy / 2;


	n_type_gfx grid_size = n_posix_max( grid_bmpsx, grid_bmpsy ) / 8;

	n_type_gfx grid_x = grid_size;
	n_type_gfx grid_y = grid_size;


	// [!] : multithread : array is not available

	n_type_gfx grid_check_x_0 = center_x - ( grid_x * 1 );
	n_type_gfx grid_check_x_1 = center_x - ( grid_x * 2 );
	n_type_gfx grid_check_x_2 = center_x - ( grid_x * 3 );
	n_type_gfx grid_check_x_3 = center_x - ( grid_x * 4 );
	n_type_gfx grid_check_x_4 = center_x + ( grid_x * 1 );
	n_type_gfx grid_check_x_5 = center_x + ( grid_x * 2 );
	n_type_gfx grid_check_x_6 = center_x + ( grid_x * 3 );
	n_type_gfx grid_check_x_7 = center_x + ( grid_x * 4 );

	if ( grid_is_odd_x )
	{
		grid_check_x_0++;
		grid_check_x_1++;
		grid_check_x_2++;
		grid_check_x_3++;
	}

	n_type_gfx grid_check_xx_0 = grid_check_x_0 - 1;
	n_type_gfx grid_check_xx_1 = grid_check_x_1 - 1;
	n_type_gfx grid_check_xx_2 = grid_check_x_2 - 1;
	n_type_gfx grid_check_xx_3 = grid_check_x_3 - 1;
	n_type_gfx grid_check_xx_4 = grid_check_x_4 - 1;
	n_type_gfx grid_check_xx_5 = grid_check_x_5 - 1;
	n_type_gfx grid_check_xx_6 = grid_check_x_6 - 1;
	n_type_gfx grid_check_xx_7 = grid_check_x_7 - 1;

	n_type_gfx grid_check_y_0 = center_y - ( grid_y * 1 );
	n_type_gfx grid_check_y_1 = center_y - ( grid_y * 2 );
	n_type_gfx grid_check_y_2 = center_y - ( grid_y * 3 );
	n_type_gfx grid_check_y_3 = center_y - ( grid_y * 4 );
	n_type_gfx grid_check_y_4 = center_y + ( grid_y * 1 );
	n_type_gfx grid_check_y_5 = center_y + ( grid_y * 2 );
	n_type_gfx grid_check_y_6 = center_y + ( grid_y * 3 );
	n_type_gfx grid_check_y_7 = center_y + ( grid_y * 4 );

	if ( grid_is_odd_y )
	{
		grid_check_y_0++;
		grid_check_y_1++;
		grid_check_y_2++;
		grid_check_y_3++;
	}

	n_type_gfx grid_check_yy_0 = grid_check_y_0 - 1;
	n_type_gfx grid_check_yy_1 = grid_check_y_1 - 1;
	n_type_gfx grid_check_yy_2 = grid_check_y_2 - 1;
	n_type_gfx grid_check_yy_3 = grid_check_y_3 - 1;
	n_type_gfx grid_check_yy_4 = grid_check_y_4 - 1;
	n_type_gfx grid_check_yy_5 = grid_check_y_5 - 1;
	n_type_gfx grid_check_yy_6 = grid_check_y_6 - 1;
	n_type_gfx grid_check_yy_7 = grid_check_y_7 - 1;


	// [!] : optimization

	if ( is_zoom_in )
	{
		fx = ( fx / zoom );
		fy = ( fy / zoom );
	} else {
		fx = ( fx * zoom );
		fy = ( fy * zoom );
	}


#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD


	// [!] : Y axis

	n_type_gfx  y = 0;
	n_type_gfx zy = 0;
	n_posix_loop
	{


#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD


	// [!] : X axis : singleline

	n_type_gfx  x = 0;
	n_type_gfx zx = 0;
	n_posix_loop
	{

		if (
			( ( tx + x ) >= rx )
			&&
			( ( ty + y ) >= ry )
			&&
			( ( tx + x ) <= ( rx + rsx ) )
			&&
			( ( ty + y ) <= ( ry + rsy ) )
		)
		{

			n_type_gfx xx,yy;

			if ( is_zoom_in )
			{
				xx = fx + zx;
				yy = fy + zy;
			} else {
				xx = fx + ( x * zoom );
				yy = fy + ( y * zoom );
			}

			u32 color; //n_bmp_ptr_get_fast( paint->pen_bmp_data, xx,yy, &color );

			BOOL is_accessible = n_bmp_ptr_is_accessible( self->paint->pen_bmp_data, xx,yy );

			if ( is_accessible == n_posix_false )
			{
//NSLog( @"is_accessible : false" );
				// [!] : never come
				color = N_PAINT_CANVAS_COLOR;
			} else
			if ( self->paint->layer_onoff )
			{
				color = n_bmp_layer_ptr_get( self->paint->layer_data, xx,yy, n_posix_false, 0, n_posix_true );
			} else {
				n_paint_grabber_pixel_get( xx,yy, self->paint->color, &color, NULL, 0.00 );
			}
//color = n_bmp_rgb_mac( 0,200,255 );


			// [!] : grabber

			if ( self->paint->grabber_mode != N_PAINT_GRABBER_NEUTRAL )
			{

				// [x] : don't use n_bmp_ptr_get_fast() : crash sometimes

				if ( self->paint->grabber_per_pixel_alpha_onoff )
				{
					int alpha = n_bmp_a( color );

					u32 c; n_bmp_ptr_get( self->paint->pen_bmp_data, xx,yy, &c );
					color = n_bmp_blend_pixel( color, c, n_bmp_blend_alpha2ratio( alpha ) );
				}

				if ( self->paint->layer_onoff == FALSE )
				{
					if ( self->paint->grabber_blend )
					{
						u32 c; n_bmp_ptr_get( self->paint->pen_bmp_data, xx,yy, &c );
						color = n_bmp_blend_pixel( color, c, self->paint->grabber_blend_ratio );
					}
				}

			}


			// [!] : checker background

			int alpha = n_bmp_a( color );

			if ( N_BMP_ALPHA_CHANNEL_VISIBLE != alpha )
			{
				u32 c = n_paint_bmp_checker_pixel( xx, yy, N_BMP_SX( self->paint->pen_bmp_data ), N_BMP_SY( self->paint->pen_bmp_data ), N_PAINT_CANVAS_COLOR );
				color = n_bmp_blend_pixel( color, c, n_bmp_blend_alpha2ratio( alpha ) );
			}


//n_bmp_box( &bmp_canvas, tx + x, ty + y, zoom_ui, zoom_ui, color );

			n_type_gfx bx = 0;
			n_type_gfx by = 0;
			while( is_accessible )
			{//break;

				u32 c = color;


				// [!] : grid

				if ( self->paint->grid_onoff )
				{
					u32 redraw_color = 0;

					if ( is_zoom_in )
					{
						if ( grid_is_odd_x )
						{
							if ( xx == center_x )
							{
								redraw_color = n_bmp_rgb_mac( 255,0,128 );
							}
						} else
						if ( ( xx == center_x )||( xx == ( center_x - 1 ) ) )
						{
							redraw_color = n_bmp_rgb_mac( 255,0,128 );
						}

						if ( redraw_color != 0 )
						{
							//
						} else
						if ( grid_is_odd_y )
						{
							if ( yy == center_y )
							{
								redraw_color = n_bmp_rgb_mac( 255,0,128 );
							}
						} else
						if ( ( yy == center_y )||( yy == ( center_y - 1 ) ) )
						{
							redraw_color = n_bmp_rgb_mac( 255,0,128 );
						}
					} else {
						if ( ( xx >= ( center_x - zoom ) )&&( xx <= ( center_x + zoom ) ) )
						{
							redraw_color = n_bmp_rgb_mac( 255,0,128 );
						} else
						if ( ( yy >= ( center_x - zoom ) )&&( yy <= ( center_x + zoom ) ) )
						{
							redraw_color = n_bmp_rgb_mac( 255,0,128 );
						}
					}

					if ( redraw_color != 0 )
					{
						//
					} else
					if ( is_zoom_in )
					{
						if (
							( xx == grid_check_x_0 )
							||
							( xx == grid_check_x_1 )
							||
							( xx == grid_check_x_2 )
							||
							( xx == grid_check_x_3 )
							||
							( xx == grid_check_x_4 )
							||
							( xx == grid_check_x_5 )
							||
							( xx == grid_check_x_6 )
							||
							( xx == grid_check_x_7 )

							||

							( xx == grid_check_xx_0 )
							||
							( xx == grid_check_xx_1 )
							||
							( xx == grid_check_xx_2 )
							||
							( xx == grid_check_xx_3 )
							||
							( xx == grid_check_xx_4 )
							||
							( xx == grid_check_xx_5 )
							||
							( xx == grid_check_xx_6 )
							||
							( xx == grid_check_xx_7 )

							||

							( yy == grid_check_y_0 )
							||
							( yy == grid_check_y_1 )
							||
							( yy == grid_check_y_2 )
							||
							( yy == grid_check_y_3 )
							||
							( yy == grid_check_y_4 )
							||
							( yy == grid_check_y_5 )
							||
							( yy == grid_check_y_6 )
							||
							( yy == grid_check_y_7 )

							||

							( yy == grid_check_yy_0 )
							||
							( yy == grid_check_yy_1 )
							||
							( yy == grid_check_yy_2 )
							||
							( yy == grid_check_yy_3 )
							||
							( yy == grid_check_yy_4 )
							||
							( yy == grid_check_yy_5 )
							||
							( yy == grid_check_yy_6 )
							||
							( yy == grid_check_yy_7 )
						)
						{
							redraw_color = n_bmp_rgb_mac( 0,200,255 );
						}
					} else {
						if (
							( ( xx > ( grid_check_x_0 - zoom ) )&&( xx < ( grid_check_x_0 + zoom ) ) )
							||
							( ( xx > ( grid_check_x_1 - zoom ) )&&( xx < ( grid_check_x_1 + zoom ) ) )
							||
							( ( xx > ( grid_check_x_2 - zoom ) )&&( xx < ( grid_check_x_2 + zoom ) ) )
							||
							( ( xx > ( grid_check_x_3 - zoom ) )&&( xx < ( grid_check_x_3 + zoom ) ) )
							||
							( ( xx > ( grid_check_x_4 - zoom ) )&&( xx < ( grid_check_x_4 + zoom ) ) )
							||
							( ( xx > ( grid_check_x_5 - zoom ) )&&( xx < ( grid_check_x_5 + zoom ) ) )
							||
							( ( xx > ( grid_check_x_6 - zoom ) )&&( xx < ( grid_check_x_6 + zoom ) ) )
							||
							( ( xx > ( grid_check_x_7 - zoom ) )&&( xx < ( grid_check_x_7 + zoom ) ) )

							||

							( ( yy > ( grid_check_y_0 - zoom ) )&&( yy < ( grid_check_y_0 + zoom ) ) )
							||
							( ( yy > ( grid_check_y_1 - zoom ) )&&( yy < ( grid_check_y_1 + zoom ) ) )
							||
							( ( yy > ( grid_check_y_2 - zoom ) )&&( yy < ( grid_check_y_2 + zoom ) ) )
							||
							( ( yy > ( grid_check_y_3 - zoom ) )&&( yy < ( grid_check_y_3 + zoom ) ) )
							||
							( ( yy > ( grid_check_y_4 - zoom ) )&&( yy < ( grid_check_y_4 + zoom ) ) )
							||
							( ( yy > ( grid_check_y_5 - zoom ) )&&( yy < ( grid_check_y_5 + zoom ) ) )
							||
							( ( yy > ( grid_check_y_6 - zoom ) )&&( yy < ( grid_check_y_6 + zoom ) ) )
							||
							( ( yy > ( grid_check_y_7 - zoom ) )&&( yy < ( grid_check_y_7 + zoom ) ) )
						)
						{
							redraw_color = n_bmp_rgb_mac( 0,200,255 );
						}
					}

					if ( redraw_color != 0 )
					{
						if ( ( bx == 0 )||( by == 0 )||( bx == ( zoom_ui - 1 ) )||( by == ( zoom_ui - 1 ) ) )
						{
							c = n_bmp_blend_pixel( c, redraw_color, 0.25 );
						}
					}
				}


				// [!] : pixel grid

				if ( ( self->paint->pixel_grid_onoff )&&( zoom_ui >= 3 ) )
				{
					if ( ( bx == 0 )||( by == 0 )||( bx == ( zoom_ui - 1 ) )||( by == ( zoom_ui - 1 ) ) )
					{
						c = n_bmp_blend_pixel( c, n_bmp_rgb_mac( 0,200,255 ), 0.25 );
					}
				}


				// [!] : alpha emphasizer

				if ( self->paint->alpha_emphasizer_onoff )
				{
					u32 emphasize_color = n_bmp_rgb_mac( 255,255,0 );

					if ( N_BMP_ALPHA_CHANNEL_VISIBLE != n_bmp_a( c ) )
					{
						if ( zoom_ui < 3 )
						{
							c = n_bmp_blend_pixel( c, emphasize_color, 0.5 );
						} else {
							if ( ( bx == 0 )||( by == 0 )||( bx == ( zoom_ui - 1 ) )||( by == ( zoom_ui - 1 ) ) )
							{
								c = n_bmp_blend_pixel( c, emphasize_color, 0.5 );
							}
						}
					}

				}

				c = n_bmp_alpha_visible_pixel( c );
				n_bmp_ptr_set( n_paint_canvas, tx + x + bx, ty + y + by, c );

				bx++;
				if ( bx >= zoom_ui )
				{
					bx = 0;

					by++;
					if ( by >= zoom_ui ) { break; }
				}
			}
		}

		x += zoom_ui; zx++;
		if ( x >= fsx ) { break; }
	}


#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	}];
	[paint->queue addOperation:o];

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD


	// [!] : Y axis

	y += zoom_ui; zy += 1;
	if ( y >= fsy ) { break; }

	}

#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	[paint->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD


	return;
}

-(void) drawRect:(NSRect) rect
{
//NSLog( @"drawRect" );


	// [x] : Sonoma : problem happens

	rect = [super bounds];


	// [Needed] : always needed

	[self n_paint_scroll_clamp];


	if ( paint->layer_load_onoff )
	{
		paint->layer_load_onoff = FALSE;

		NSString *text = [NSString stringWithFormat:@"%0.0f%%", paint->layer_load_percent];

		NSMutableDictionary *attr = [NSMutableDictionary dictionary];

		NSFont  *font     = [NSFont fontWithName:@"Trebuchet MS" size:33];
		NSColor *color_fg = [NSColor whiteColor];
		NSColor *color_bg = n_mac_argb2nscolor( N_PAINT_CANVAS_COLOR );

		[attr setObject:font     forKey:NSFontAttributeName           ];
		[attr setObject:color_fg forKey:NSForegroundColorAttributeName];

		CGFloat isx = rect.size.width;
		CGFloat isy = rect.size.height;

		CGSize size = n_mac_image_text_pixelsize( text, font );

		NSRect rect_centered = NSMakeRect
		(
			( ( isx - size.width  ) / 2 ),
			( ( isy - size.height ) / 2 ),
			size.width,
			size.height
		);

		n_mac_draw_box( color_bg, rect );
		[text drawInRect:rect_centered withAttributes:attr];

		return;
	}


	paint->rect_redraw = rect;


	BOOL is_flushed = FALSE;

	static n_type_gfx prv_sx = -1;
	static n_type_gfx prv_sy = -1;

	n_type_gfx sx = NSWidth ( self.frame ) + 1;
	n_type_gfx sy = NSHeight( self.frame ) + 1;

	if ( NULL == N_BMP_PTR( &bmp_canvas ) )
	{
		n_bmp_new_fast( &bmp_canvas, sx,sy );
		rep = n_mac_image_NSBitmapImageRep( &bmp_canvas );

		n_mac_image_imagerep_alias( rep, &n_paint_target );

		is_flushed = TRUE;
		n_bmp_flush( &bmp_canvas, N_PAINT_CANVAS_COLOR );

		//n_paint_canvas = &bmp_canvas;

//NSLog( @"alloc : %x", [rep bitmapData] );
	}


	// [Needed] : make everytime

	n_paint_canvas = &n_paint_target;


	if ( ( prv_sx != sx )||( prv_sy != sy ) )
	{
		if ( is_flushed == FALSE )
		{
			n_bmp_flush( n_paint_canvas, N_PAINT_CANVAS_COLOR );
		}
	}


	if ( paint->init == FALSE )
	{
		NSRect rect_canvas = NSMakeRect( 0,0,sx,sy );
		//n_mac_image_nbmp_direct_draw_fast( n_paint_canvas, &rect_canvas, NO );

		//n_mac_image_imagerep_sync( rep, n_paint_canvas );
		n_mac_image_nbmp_direct_draw_faster( rep, &rect_canvas, NO );

		return;
	}


	BOOL prv_scroller_x_onoff = paint->scroller_x_onoff;
	BOOL prv_scroller_y_onoff = paint->scroller_y_onoff;

	{

		// [!] : Fake Scroller : Calc Only : Horizontal

		CGFloat zoom = [self n_paint_zoom_get_ratio:paint->zoom];

		CGFloat csx              = sx;
		CGFloat items_per_canvas = paint->inner_sx;
		CGFloat max_count        = (CGFloat) N_BMP_SX( paint->pen_bmp_data ) * zoom;

		if ( max_count > items_per_canvas )
		{
//NSLog( @"%f %f", items_per_canvas, max_count );

			paint->scroller_x_onoff = TRUE;

			CGFloat shaft = csx - paint->scroller_size;
			CGFloat page  = max_count / items_per_canvas;

			CGFloat scrsx = n_posix_max_n_type_real( paint->scroller_size, ( items_per_canvas / page ) + paint->margin - paint->scroller_size );
			CGFloat scrsy = paint->scroller_size;
			CGFloat scr_x = ( paint->scroll.x / max_count ) * items_per_canvas;
			CGFloat scr_y = sy - scrsy;
//NSLog( @"%f %f %f %f", scr_x, scr_y, scrsx, scrsy );


			// [!] : for hit test

			paint->scroller_x_rect_shaft = NSMakeRect(     0, scr_y, shaft, scrsy );
			paint->scroller_x_rect_thumb = NSMakeRect( scr_x, scr_y, scrsx, scrsy );

		} else {
//NSLog( @"2" );
			paint->scroller_x_onoff = FALSE;
			paint->scroll.x         = 0;

		}

	}

	{

		// [!] : Fake Scroller : Calc Only : Vertical

		CGFloat zoom = [self n_paint_zoom_get_ratio:paint->zoom];

		CGFloat csy              = sy;
		CGFloat items_per_canvas = paint->inner_sy;
		CGFloat max_count        = (CGFloat) N_BMP_SY( paint->pen_bmp_data ) * zoom;

		if ( max_count > items_per_canvas )
		{

			paint->scroller_y_onoff = TRUE;

			CGFloat shaft = csy - paint->scroller_size;
			CGFloat page  = max_count / items_per_canvas;

			CGFloat scrsx = paint->scroller_size;
			CGFloat scrsy = n_posix_max_n_type_real( paint->scroller_size, ( items_per_canvas / page ) + paint->margin - paint->scroller_size );
			CGFloat scr_x = sx - scrsx;
			CGFloat scr_y = ( paint->scroll.y / max_count ) * items_per_canvas;


			// [!] : for hit test

			paint->scroller_y_rect_shaft = NSMakeRect( scr_x,     0, scrsx, shaft );
			paint->scroller_y_rect_thumb = NSMakeRect( scr_x, scr_y, scrsx, scrsy );

		} else {

			paint->scroller_y_onoff = FALSE;
			paint->scroll.y         = 0;

		}

	}

	if (
		( prv_scroller_x_onoff != paint->scroller_x_onoff )
		||
		( prv_scroller_y_onoff != paint->scroller_y_onoff )
	)
	{
		if ( is_flushed == FALSE )
		{
			n_bmp_flush( n_paint_canvas, N_PAINT_CANVAS_COLOR );
		}
	}


	// [!] : Combined Scale Copy

	[self n_paint_draw:nil sx:sx sy:sy];

	paint->redraw_type = N_PAINT_REDRAW_TYPE_ALL;


	if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_EDGE )
	{
		n_paint_grabber_frame_edge_draw( paint, n_paint_canvas );

		n_paint_grabber_frame_edge_resize_dot_draw( paint, n_paint_canvas );

		[self n_paint_draw_tooltip];
	} else
	if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_PIXEL )
	{
		int zoom_unit = [self n_paint_zoom_get_int_ui:paint->zoom];
		n_paint_grabber_frame_pixel_draw( paint, n_paint_canvas, zoom_unit );

		n_type_gfx size = n_paint_grabber_frame_pixel_redraw_area_get( paint );
		n_paint_grabber_frame_pixel_resize_dot_draw( paint, n_paint_canvas, size );

		[self n_paint_draw_tooltip];
	}

/*
	if ( paint->grid_onoff )
	{
		n_type_gfx grid_size = n_posix_max( sx, sy ) / 16;

		n_type_gfx cx = sx / 2;
		n_type_gfx gx = 0;
		n_posix_loop
		{
			gx += grid_size;
			if ( gx >= sx ) { break; }

			n_bmp_line( n_paint_canvas, cx + gx, 0, cx + gx, sy, n_bmp_rgb_mac( 0,200,255 ) );
			n_bmp_line( n_paint_canvas, cx - gx, 0, cx - gx, sy, n_bmp_rgb_mac( 0,200,255 ) );
		}

		n_type_gfx cy = sy / 2;
		n_type_gfx gy = 0;
		n_posix_loop
		{
			gy += grid_size;
			if ( gy >= sy ) { break; }

			n_bmp_line( n_paint_canvas, 0, cy + gy, sx, cy + gy, n_bmp_rgb_mac( 0,200,255 ) );
			n_bmp_line( n_paint_canvas, 0, cy - gy, sx, cy - gy, n_bmp_rgb_mac( 0,200,255 ) );
		}

		n_bmp_line( n_paint_canvas, sx / 2, 0, sx / 2, sy, n_bmp_rgb_mac( 255,0,128 ) );
		n_bmp_line( n_paint_canvas, 0, sy / 2, sx, sy / 2, n_bmp_rgb_mac( 255,0,128 ) );
	}
*/

	if ( paint->scroller_x_onoff )
	{
//NSLog( @" 1 " );

		// [!] : Fake Scroller : Draw : Horizontal


		// [!] : shaft

		{
			n_type_gfx  x = paint->scroller_x_rect_shaft.origin.x;
			n_type_gfx  y = paint->scroller_x_rect_shaft.origin.y;
			n_type_gfx sx = paint->scroller_x_rect_shaft.size.width;
			n_type_gfx sy = paint->scroller_x_rect_shaft.size.height;
//NSLog( @"Shaft : %d %d %d %d", x,y,sx,sy );

			u32 color_shaft = n_bmp_rgb_mac( 244,244,244 );
			n_bmp_roundrect( n_paint_canvas, x,y,sx,sy, color_shaft, 50 );
		}


		// [!] : thumb / knob

		int alpha_fade_bg;
		int alpha_fade_fg;
		if ( paint->scroller_fade.color_fg == n_bmp_white )
		{
			alpha_fade_bg = 64;
			alpha_fade_fg = 96;
		} else {
			alpha_fade_bg = 96;
			alpha_fade_fg = 64;
		}
		int alpha = 255;//n_bmp_blend_channel( alpha_fade_bg, alpha_fade_fg, (double) fade_scroll.percent * 0.01 );
//NSLog( @"%d", fade.percent );

		{
			n_type_gfx  x = paint->scroller_x_rect_thumb.origin.x;
			n_type_gfx  y = paint->scroller_x_rect_thumb.origin.y;
			n_type_gfx sx = paint->scroller_x_rect_thumb.size.width;
			n_type_gfx sy = paint->scroller_x_rect_thumb.size.height;
//NSLog( @"Thumb : %d %d %d %d", x,y,sx,sy );

			u32 color       = n_bmp_rgb_mac( 200,200,200 );
			u32 color_thumb = n_bmp_alpha_replace_pixel( color, alpha );
			n_bmp_roundrect( n_paint_canvas, x,y,sx,sy, color_thumb, 50 );
		}

	}

	if ( paint->scroller_y_onoff )
	{

		// [!] : Fake Scroller : Draw : Vertical


		// [!] : shaft

		{
			n_type_gfx  x = paint->scroller_y_rect_shaft.origin.x;
			n_type_gfx  y = paint->scroller_y_rect_shaft.origin.y;
			n_type_gfx sx = paint->scroller_y_rect_shaft.size.width;
			n_type_gfx sy = paint->scroller_y_rect_shaft.size.height;

			u32 color_shaft = n_bmp_rgb_mac( 244,244,244 );
			n_bmp_roundrect( n_paint_canvas, x,y,sx,sy, color_shaft, 50 );
		}


		// [!] : thumb / knob

		int alpha_fade_bg;
		int alpha_fade_fg;
		if ( paint->scroller_fade.color_fg == n_bmp_white )
		{
			alpha_fade_bg = 64;
			alpha_fade_fg = 96;
		} else {
			alpha_fade_bg = 96;
			alpha_fade_fg = 64;
		}
		int alpha = 255;//n_bmp_blend_channel( alpha_fade_bg, alpha_fade_fg, (double) fade_scroll.percent * 0.01 );
//NSLog( @"%d", fade.percent );

		{
			n_type_gfx  x = paint->scroller_y_rect_thumb.origin.x;
			n_type_gfx  y = paint->scroller_y_rect_thumb.origin.y;
			n_type_gfx sx = paint->scroller_y_rect_thumb.size.width;
			n_type_gfx sy = paint->scroller_y_rect_thumb.size.height;

			u32 color       = n_bmp_rgb_mac( 200,200,200 );
			u32 color_thumb = n_bmp_alpha_replace_pixel( color, alpha );
			n_bmp_roundrect( n_paint_canvas, x,y,sx,sy, color_thumb, 50 );
		}

	}


	n_paint_indicator();


	{
		NSRect rect_canvas = NSMakeRect( 0,0,sx,sy );
		//n_mac_image_nbmp_direct_draw_fast( n_paint_canvas, &rect_canvas, NO );

//NSLog( @"sync : %x", [rep bitmapData] );
		//n_mac_image_imagerep_sync( rep, n_paint_canvas );

		n_mac_image_imagerep_alias_fast( rep, &n_paint_target );

		n_mac_image_nbmp_direct_draw_faster( rep, &rect_canvas, NO );
	}

}




- (CGFloat) n_paint_zoom_get_ratio:(CGFloat) zoom
{
	if ( zoom < 0 ) { zoom = 1.0 / ( zoom * -1 ); }

//NSLog( @"%0.2f", zoom );
	return zoom;
}

- (int) n_paint_zoom_get_int:(int) zoom
{
	if ( zoom < 0 ) { zoom *= -1; }

//NSLog( @"%d", zoom );
	return zoom;
}

- (int) n_paint_zoom_get_int_ui:(int) zoom
{
	if ( zoom < 0 ) { zoom = 1; }

	return zoom;
}

- (NSPoint) n_paint_point_on_bitmap
{

	// Phase 1

	n_type_real ratio = [self n_paint_zoom_get_ratio:paint->zoom];

	NSPoint pt = n_mac_cursor_position_get( self );


	// Phase 2

	if ( paint->margin_onoff )
	{
		n_type_real bmpsx = N_BMP_SX( paint->pen_bmp_data ) * ratio;
		n_type_real bmpsy = N_BMP_SY( paint->pen_bmp_data ) * ratio;

		if ( paint->inner_sx > bmpsx )
		{
			pt.x -= ( paint->inner_sx - bmpsx ) / 2;
		}

		if ( paint->inner_sy > bmpsy )
		{
			pt.y -= ( paint->inner_sy - bmpsy ) / 2;
		}

		pt.x -= paint->margin / 2;
		pt.y -= paint->margin / 2;
	}


	// Phase 3

	{
		pt.x /= ratio;
		pt.y /= ratio;
	}


	// Phase 4

	{
		pt.x += trunc( trunc( paint->scroll.x ) / ratio );
		pt.y += trunc( trunc( paint->scroll.y ) / ratio );
	}


//NSLog( @"%f %f", pt.x, pt.y );
	return NSMakePoint( pt.x, pt.y );
}




// internal
- (void) n_paint_scroll_clamp
{
//return;

	if ( paint->scroll.x < 0 ) { paint->scroll.x = 0; }
	if ( paint->scroll.y < 0 ) { paint->scroll.y = 0; }

	CGFloat zoom = [self n_paint_zoom_get_ratio:paint->zoom];

	CGFloat max_sx = ( N_BMP_SX( paint->pen_bmp_data ) * zoom ) - paint->inner_sx + zoom;
	CGFloat max_sy = ( N_BMP_SY( paint->pen_bmp_data ) * zoom ) - paint->inner_sy + zoom;;

	if ( paint->scroll.x >= max_sx ) { paint->scroll.x = trunc( max_sx ); }
	if ( paint->scroll.y >= max_sy ) { paint->scroll.y = trunc( max_sy ); }

}
/*
// internal
- (void) n_paint_scroll_clamp_arrange
{
//return;

	n_type_gfx zoom_ui = [self n_paint_zoom_get_int_ui:paint->zoom];

	n_type_gfx scr_x = paint->scroll.x;
	n_type_gfx scr_y = paint->scroll.y;

	scr_x = scr_x / zoom_ui * zoom_ui;
	scr_y = scr_y / zoom_ui * zoom_ui;

	paint->scroll.x = scr_x;
	paint->scroll.y = scr_y;

}
*/
- (void) n_paint_convert_bitmap2canvas:(void*) zero x:(n_type_gfx*)x y:(n_type_gfx*)y sx:(n_type_gfx*)sx sy:(n_type_gfx*)sy
{

	CGFloat ratio = [self n_paint_zoom_get_ratio:paint->zoom];

	n_type_gfx osetx = paint->canvas_offset_x;
	n_type_gfx osety = paint->canvas_offset_y;
	n_type_gfx scr_x = paint->scroll.x;
	n_type_gfx scr_y = paint->scroll.y;

	if (  x != NULL ) { (* x) = osetx + ( (*x) * ratio ) - scr_x; }
	if (  y != NULL ) { (* y) = osety + ( (*y) * ratio ) - scr_y; }
	if ( sx != NULL ) { (*sx) = (*sx) * ratio; }
	if ( sy != NULL ) { (*sy) = (*sy) * ratio; }

}

- (void) n_paint_convert_bitmap2canvas_grabber_frame:(void*) zero x:(n_type_gfx*)x y:(n_type_gfx*)y sx:(n_type_gfx*)sx sy:(n_type_gfx*)sy
{

	n_type_gfx zoom_ui = [self n_paint_zoom_get_int_ui:paint->zoom];


	CGFloat ratio = [self n_paint_zoom_get_ratio:paint->zoom];

	n_type_gfx osetx = paint->canvas_offset_x;
	n_type_gfx osety = paint->canvas_offset_y;
	n_type_gfx scr_x = (n_type_gfx) paint->scroll.x / zoom_ui * zoom_ui;
	n_type_gfx scr_y = (n_type_gfx) paint->scroll.y / zoom_ui * zoom_ui;

	if (  x != NULL ) { (* x) = osetx + ( (*x) * ratio ) - scr_x; }
	if (  y != NULL ) { (* y) = osety + ( (*y) * ratio ) - scr_y; }
	if ( sx != NULL ) { (*sx) = (*sx) * ratio; }
	if ( sy != NULL ) { (*sy) = (*sy) * ratio; }

}




- (void) n_paint_grabber_frame_anim_timer_method
{
//NSLog( @"n_paint_grabber_frame_anim_timer_method" );

	if ( paint->resizer_onoff ) { return; }

	if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { return; }


	if ( paint->grabber_is_key_input_fade )
	{
		if ( n_game_timer( &paint->grabber_is_key_input_tick, 500 ) )
		{
			paint->grabber_is_key_input      = FALSE;
			paint->grabber_is_key_input_fade = FALSE;

			n_paint_grabber_resync_auto();

			return;
		}
	}


//NSLog( @"%d", paint->grabber_mode );
	if (
		( paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_DRAGGING )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_TRANSFORM )
	)
	{
		//static u32 timer = 0;
		//if ( n_game_timer( &timer, 200 ) )
		{

			if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_EDGE )
			{
				paint->grabber_frame_step++;
			} else
			if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_PIXEL )
			{
				paint->grabber_frame_step++;
/*
				if ( n_paint_grabber_frame_upddown )
				{
					n_paint_grabber_frame_upddown_param -= 0.1;
					if ( n_paint_grabber_frame_upddown_param <= 0.0 )
					{
						n_paint_grabber_frame_upddown = FALSE;
					}
				} else {
					n_paint_grabber_frame_upddown_param += 0.1;
					if ( n_paint_grabber_frame_upddown_param >= 1.0 )
					{
						n_paint_grabber_frame_upddown = TRUE;
					}
				}
*/
			}

			if (
				( paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
				||
				( paint->grabber_mode == N_PAINT_GRABBER_DRAGGING )
			)
			{
				n_paint_grabber_resync_auto();
			}
		}
	}

}

- (void) n_paint_grabber_frame_anim_off
{
//NSLog( @"n_paint_grabber_frame_anim_off" );

	n_mac_timer_exit( n_paint_grabber_frame_anim_timer );
	n_paint_grabber_frame_anim_timer = nil;

	n_paint_grabber_frame_reset( paint );

}

- (void) n_paint_grabber_frame_anim_on
{
//NSLog( @"n_paint_frame_anim_on" );

//NSLog( @"n_paint_frame_anim_on : %d %d", paint->tooltype, paint->grabber_mode ); return;

	if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { return; }

	if ( n_paint_grabber_frame_anim_timer == nil )
	{
		n_paint_grabber_frame_anim_timer = n_mac_timer_init( n_paint_global, @selector( n_paint_grabber_frame_anim_timer_method ), N_PAINT_GRABBER_FRAME_ANIM_INTERVAL );
	}

	paint->grabber_frame_step        = 0;
	n_paint_grabber_frame_anim_onoff = n_posix_true;

}




- (void) updateTrackingAreas
{
//return;

	int options = (
		//NSTrackingMouseEnteredAndExited |
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
/*
- (void) resetCursorRects
{
//NSLog(@"resetCursorRects");

	// [x] : buggy : maybe impossible to implement accurate behavior
	//
	//	[cursor set]     : applied globally
	//	mouseMoved       : delay will occur
	//	at launch time   : always arrow cursor
	//	push/pop         : more complicated behavior happens
	//	resetCursorRects : not refreshed on inactive window

	//[NSCursor hide];

	//return;


	// [Patch]

	NSRect rect = NSMakeRect(
		self.bounds.origin.x    +   paint->scroller_size,
		self.bounds.origin.y    +   paint->scroller_size,
		self.bounds.size.width  - ( paint->scroller_size * 2 ),
		self.bounds.size.height - ( paint->scroller_size * 2 )
	);

	//[self discardCursorRects];

	[self removeCursorRect:rect cursor:paint->cursor_arrow  ];
	[self removeCursorRect:rect cursor:paint->cursor_no     ];
	[self removeCursorRect:rect cursor:paint->cursor_hand   ];
	[self removeCursorRect:rect cursor:paint->cursor_pen_on ];
	[self removeCursorRect:rect cursor:paint->cursor_pen_off];
	[self removeCursorRect:rect cursor:paint->cursor_eraser ];

	//[self.window invalidateCursorRectsForView:self];
	//[self.window invalidateCursorRectsForView:self];

	if ( paint->readonly )
	{
		//[[NSCursor arrowCursor] set];
		[self addCursorRect:rect cursor:paint->cursor_arrow];
	} else
	if (
		( paint->layer_onoff )
		&&
		(
			( paint->layer_data[ paint->layer_index ].visible == FALSE )
			||
			( n_paint_layer_is_locked( paint->layer_index ) )
		)
	)
	{
		[self addCursorRect:rect cursor:paint->cursor_no];
	} else
	if ( paint->cursor_grab_n_drag_onoff )
	{
		[self addCursorRect:rect cursor:paint->cursor_hand];
	} else
	if ( paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
	{
		if ( paint->pen_quick_eraser_onoff )
		{
			//[paint->cursor_eraser set];
			[self addCursorRect:rect cursor:paint->cursor_eraser ];
		} else
		if ( paint->pen_start )
		{
			//[paint->cursor_pen_on set];
			[self addCursorRect:rect cursor:paint->cursor_pen_on ];
		} else {
			//[paint->cursor_pen_off set];
			[self addCursorRect:rect cursor:paint->cursor_pen_off];
		}
	} else {
		//[[NSCursor arrowCursor] set];
		[self addCursorRect:rect cursor:paint->cursor_arrow];
	}

}
*/
- (BOOL) n_paint_is_not_canvas
{

	// [!] : skip canvas window
	n_type_int i = 1;
	n_posix_loop
	{
		if ( paint->nswindow[ i ] == nil ) { break; }

		if ( n_mac_window_is_keywindow( paint->nswindow[ i ] ) )
		{
			return TRUE;
		}

		i++;
	}


	return FALSE;
}

- (void) resetCursorRects
{
//NSLog(@"resetCursorRects2");

//NSLog( @"%d", n_paint_grabber_frame_anim_onoff );

	NSPoint pt = n_mac_cursor_position_get( self );

	if ( FALSE == n_mac_window_is_keywindow( paint->nswindow[ 0 ] ) )
	{
		//
	} else
	if ( [self n_paint_is_not_canvas] )
	{
//NSLog( @"key window: %d", paint->pen_start );
		//[paint->cursor_arrow set]; // [x] : resizer X/Y fields
	} else
	if ( paint->cursor_grab_n_drag_onoff )
	{
		[paint->cursor_hand set];
	} else
	if (
		( pt.x < 0 )
		||
		( pt.y < 0 )
		||
		( pt.x > ( [self frame].size.width  - paint->scroller_size ) )
		||
		( pt.y > ( [self frame].size.height - paint->scroller_size ) )
	)
	{
//NSLog( @"out of canvas : %d", paint->pen_start );

		if ( paint->cursor_grab_n_drag_onoff )
		{
			[paint->cursor_hand set];
		} else
		if ( paint->pen_start )
		{
			[paint->cursor_pen_on set];
		} else {
			[paint->cursor_arrow set];
		}
	} else
	if (
		( paint->layer_onoff )
		&&
		(
			( paint->layer_data[ paint->layer_index ].visible == FALSE )
			||
			( n_paint_layer_is_locked( paint->layer_index ) )
		)
	)
	{
		[paint->cursor_no set];
	} else
	if ( n_paint_grabber_frame_anim_onoff )
	{
//NSLog( @"type %d : start %d", paint->tooltype, paint->pen_start );
		if ( paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
		{
			if ( paint->pen_quick_eraser_whole_layer_onoff )
			{
				[paint->cursor_eraser set];
			} else
			if ( paint->pen_quick_eraser_onoff )
			{
				[paint->cursor_eraser set];
			} else
			if ( paint->pen_quick_blur_onoff )
			{
				[paint->cursor_blur set];
			} else
			if ( paint->pen_start )
			{
				[paint->cursor_pen_on set];
			} else {
				[paint->cursor_pen_off set];
			}
		} else
		if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_EDGE )
		{
			int i = n_paint_grabber_frame_edge_resize_dot_detect( paint );
//NSLog( @"%d", i );
			if ( i == -1 )
			{
				[paint->cursor_arrow set];
				paint->cursor_resize_dragging = paint->cursor_arrow;
			} else
			if ( ( i == 0 )||( i == 2 ) )
			{
				[paint->cursor_resize_ne set];
				paint->cursor_resize_dragging = paint->cursor_resize_ne;
			} else {
				[paint->cursor_resize_nw set];
				paint->cursor_resize_dragging = paint->cursor_resize_nw;
			}
		} else
		if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_PIXEL )
		{
			BOOL top_left, top_right, bottom_left, bottom_right;
			n_paint_grabber_frame_pixel_resize_dot_is_hit( paint, &top_left, &top_right, &bottom_left, &bottom_right );

			if ( ( top_left )||( bottom_right ) )
			{
				[paint->cursor_resize_ne set];
				paint->cursor_resize_dragging = paint->cursor_resize_ne;
			} else
			if ( ( top_right )||( bottom_left ) )
			{
				[paint->cursor_resize_nw set];
				paint->cursor_resize_dragging = paint->cursor_resize_nw;
			} else {
				[paint->cursor_arrow set];
				paint->cursor_resize_dragging = paint->cursor_arrow;
			}
		}
	} else
	if ( paint->readonly )
	{
//NSLog( @"read-only" );
		[paint->cursor_arrow set];
	} else
	if ( paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
	{
//NSLog( @"N_PAINT_TOOL_TYPE_PEN" );
		if ( paint->pen_quick_eraser_whole_layer_onoff )
		{
			[paint->cursor_eraser set];
		} else
		if ( paint->pen_quick_eraser_onoff )
		{
			[paint->cursor_eraser set];
		} else
		if ( paint->pen_quick_blur_onoff )
		{
			[paint->cursor_blur set];
		} else
		if ( paint->pen_start )
		{
			[paint->cursor_pen_on set];
		} else {
			[paint->cursor_pen_off set];
		}
	} else {
//NSLog( @"N/A %d", paint->pen_start );

		[paint->cursor_arrow set];
	}

}

- (void) mouseEntered:(NSEvent *)theEvent
{
//NSLog(@"mouseEntered");

	// [Needed] : NSTrackingMouseEnteredAndExited

}

- (void) mouseExited:(NSEvent *)theEvent
{
//NSLog(@"mouseExited");

	// [Needed] : NSTrackingMouseEnteredAndExited

}

- (void) mouseMoved:(NSEvent *)theEvent
{
//NSLog( @"mouseMoved" );

	// [Needed] : NSTrackingMouseMoved
	//[self.delegate mouseMoved:theEvent];

	[n_paint_global.delegate NonnonPaintStatus];

	[self resetCursorRects];

}




- (void) n_paint_pressure_set:(NSEvent*) theEvent
{

	// [x] : WACOM Driver : latest version : zero while side button is pressed

	if ( paint->pressure_onoff )
	{
		pressure = [theEvent pressure];
	} else {
		pressure = 1.0;
	}

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog( @"mouseUp" );


	paint->scroller_thumb_is_captured = FALSE;

	paint->scroller_x_offset = -1;
	paint->scroller_y_offset = -1;


	if ( paint->readonly ) { return; }


	NonnonPaintPen_mouseUp( paint );
	NonnonPaintGrabber_mouseUp( paint );


	[self resetCursorRects];

}

- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");

//NSPoint pt = [self n_paint_point_on_bitmap];
//NSLog( @"%0.2f %0.2f", pt.x, pt.y );
//n_bmp_circle( paint->pen_bmp_data, pt.x-16,pt.y-16,32,32, n_bmp_rgb_mac( 0,200,255 ) );
//[self display];

	paint->scroller_x_thumb_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, paint->scroller_x_rect_thumb );
	paint->scroller_x_shaft_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, paint->scroller_x_rect_shaft );
	paint->scroller_y_thumb_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, paint->scroller_y_rect_thumb );
	paint->scroller_y_shaft_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, paint->scroller_y_rect_shaft );


	NSPoint pt_cur = n_mac_cursor_position_get( self );

	if ( paint->scroller_x_thumb_is_hovered )
	{
		paint->scroller_x_offset = pt_cur.x - paint->scroller_x_rect_thumb.origin.x;
	} else {
		paint->scroller_x_offset = -1;
	}

	if ( paint->scroller_y_thumb_is_hovered )
	{
		paint->scroller_y_offset = pt_cur.y - paint->scroller_y_rect_thumb.origin.y;
	} else {
		paint->scroller_y_offset = -1;
	}


	if ( ( paint->scroller_x_thumb_is_hovered )||( paint->scroller_y_thumb_is_hovered ) )
	{
		paint->scroller_thumb_is_captured = TRUE;
	} else
	if ( paint->scroller_x_shaft_is_hovered )
	{
		CGFloat zoom = [self n_paint_zoom_get_ratio:paint->zoom];

		CGFloat items_per_canvas = paint->inner_sx;
		CGFloat max_count        = (CGFloat) N_BMP_SX( paint->pen_bmp_data ) * zoom;

		paint->scroll.x = pt_cur.x - ( NSWidth( paint->scroller_x_rect_thumb )/ 2 );
		paint->scroll.x = ( paint->scroll.x / items_per_canvas ) * max_count;

		[self display];
	} else
	if ( paint->scroller_y_shaft_is_hovered )
	{
		CGFloat zoom = [self n_paint_zoom_get_ratio:paint->zoom];

		CGFloat items_per_canvas = paint->inner_sy;
		CGFloat max_count        = (CGFloat) N_BMP_SY( paint->pen_bmp_data ) * zoom;

		paint->scroll.y = pt_cur.y - ( NSHeight( paint->scroller_y_rect_thumb )/ 2 );
		paint->scroll.y = ( paint->scroll.y / items_per_canvas ) * max_count;

		[self display];
	} else {

		// [!] : for traditional Mac mice like Magic Mouse
		NSUInteger flags = [[NSApp currentEvent] modifierFlags];
		if ( flags & NSEventModifierFlagShift )
		{
			[self rightMouseDown:theEvent];
			return;
		}


		if ( paint->readonly ) { return; }


		[self n_paint_grabber_frame_anim_on];


		if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_EDGE )
		{
			n_paint_grabber_frame_edge_mouseDown( paint );
		} else
		if ( paint->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_PIXEL )
		{
			n_paint_grabber_frame_pixel_mouseDown( paint );
		}


		[self n_paint_pressure_set:theEvent];

		NonnonPaintPen_mouseDown( paint, theEvent );
		NonnonPaintGrabber_mouseDown( paint, theEvent );

		[self resetCursorRects];

	}

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog( @"mouseDragged" );

	if ( paint->scroller_thumb_is_captured )
	{
		NSPoint pt_cur = n_mac_cursor_position_get( self );

		CGFloat zoom = [self n_paint_zoom_get_ratio:paint->zoom];

		if ( paint->scroller_x_thumb_is_hovered )
		{
			CGFloat items_per_canvas = paint->inner_sx;
			CGFloat max_count        = (CGFloat) N_BMP_SX( paint->pen_bmp_data ) * zoom;

			paint->scroll.x = pt_cur.x - paint->scroller_x_offset;
			paint->scroll.x = ( paint->scroll.x / items_per_canvas ) * max_count;
		}

		if ( paint->scroller_y_thumb_is_hovered )
		{
			CGFloat items_per_canvas = paint->inner_sy;
			CGFloat max_count        = (CGFloat) N_BMP_SY( paint->pen_bmp_data ) * zoom;

			paint->scroll.y = pt_cur.y - paint->scroller_y_offset;
			paint->scroll.y = ( paint->scroll.y / items_per_canvas ) * max_count;
		}

		[self display];
	} else {
		if ( paint->readonly ) { return; }

		[self n_paint_pressure_set:theEvent];

		NonnonPaintPen_mouseDragged( paint );
		NonnonPaintGrabber_mouseDragged( paint );
	}

}

- (void)rightMouseDown:(NSEvent *)theEvent
{

	if ( paint->readonly ) { return; }

	NonnonPaintPen_rightMouseDown( paint, theEvent );
	NonnonPaintGrabber_rightMouseDown( paint );

	[self.delegate NonnonPaintColorSet];

}


- (void)scrollWheel:(NSEvent *)theEvent
{

	CGFloat delta = 0;

//NSLog( @"%f", [theEvent scrollingDeltaY] );
	if ( [theEvent scrollingDeltaY] == 0 )
	{
		//
	} else {
		if ( [theEvent scrollingDeltaY] < 0 )
		{
			delta = -1;
		} else {
			delta =  1;
		}
	}


	n_type_gfx p_zoom = paint->zoom;

	paint->zoom += delta;


	static int prev = 1;

	if ( ( paint->zoom == 0 )||( paint->zoom == -1 ) )
	{
		if ( prev > 0 ) { paint->zoom = -2; } else { paint->zoom = 1; }
	}

	if ( paint->zoom < -100 ) { paint->zoom = -100; } else
	if ( paint->zoom >  100 ) { paint->zoom =  100; }

	prev = paint->zoom;


	[self.delegate NonnonScrollbarZoomSync:TRUE];


	// [!] : Smart Zoom

	NSPoint pt = n_mac_cursor_position_get( self );

	pt.x -= paint->canvas_offset_x;
	pt.y -= paint->canvas_offset_y;


	n_type_gfx p_z = [self n_paint_zoom_get_int:     p_zoom];
	n_type_gfx c_z = [self n_paint_zoom_get_int:paint->zoom];

	paint->scroll.x += pt.x;
	paint->scroll.y += pt.y;

	if ( ( p_zoom >=  1 )&&( paint->zoom >=  1 ) )
	{
		if ( p_z < c_z )
		{
//NSLog( @"1 : zoom in  : %d %d : %d %d", p_zoom, paint->zoom, p_z, c_z );
			paint->scroll.x = trunc( (n_type_real) paint->scroll.x / p_z * c_z );
			paint->scroll.y = trunc( (n_type_real) paint->scroll.y / p_z * c_z );
		} else {
//NSLog( @"2 : zoom out : %d %d : %d %d", p_zoom, paint->zoom, p_z, c_z );
			paint->scroll.x = trunc( (n_type_real) paint->scroll.x / p_z * c_z );
			paint->scroll.y = trunc( (n_type_real) paint->scroll.y / p_z * c_z );
		}
	} else
	if ( ( p_zoom <= -1 )&&( paint->zoom <= -1 ) )
	{
		if ( p_z > c_z )
		{
//NSLog( @"3 : zoom in  : %d %d : %d %d", p_zoom, paint->zoom, p_z, c_z );
			paint->scroll.x = trunc( (n_type_real) paint->scroll.x * p_z / c_z );
			paint->scroll.y = trunc( (n_type_real) paint->scroll.y * p_z / c_z );
		} else {
//NSLog( @"4 : zoom out : %d %d : %d %d", p_zoom, paint->zoom, p_z, c_z );
			paint->scroll.x = trunc( (n_type_real) paint->scroll.x * p_z / c_z );
			paint->scroll.y = trunc( (n_type_real) paint->scroll.y * p_z / c_z );
		}
	} else {
		if ( p_z > c_z )
		{
//NSLog( @"5 : zoom cross : %d %d : %d %d", p_zoom, paint->zoom, p_z, c_z );
			paint->scroll.x = trunc( (n_type_real) paint->scroll.x * p_z / c_z );
			paint->scroll.y = trunc( (n_type_real) paint->scroll.y * p_z / c_z );
		} else {
//NSLog( @"6 : zoom cross : %d %d : %d %d", p_zoom, paint->zoom, p_z, c_z );
			paint->scroll.x = trunc( (n_type_real) paint->scroll.x / p_z * c_z );
			paint->scroll.y = trunc( (n_type_real) paint->scroll.y / p_z * c_z );
		}
	}

	paint->scroll.x -= pt.x;
	paint->scroll.y -= pt.y;

//NSLog( @"%f %f", paint->scroll.x, paint->scroll.y );


	[self.delegate NonnonPaintResize];

}




- (void) otherMouseUp:(NSEvent*) theEvent
{
//NSLog( @"otherMouseUp : %ld", (long) [theEvent buttonNumber] );

	if ( [theEvent buttonNumber] == 2 )
	{
		// [!] : middle button

		paint->cursor_grab_n_drag_onoff = FALSE;
		[self resetCursorRects];

		[self display];
	}

}

- (void) otherMouseDown:(NSEvent*) theEvent
{
//NSLog( @"otherMouseDown : %ld", (long) [theEvent buttonNumber] );

	// [!] : Grab N Drag

	if ( [theEvent buttonNumber] == 2 )
	{
		// [!] : middle button

		paint->pt_prv = n_mac_cursor_position_get( self );

		paint->cursor_grab_n_drag_onoff = TRUE;
		[self resetCursorRects];
	}

}

- (void) otherMouseDragged:(NSEvent*) theEvent
{
//NSLog( @"otherMouseDragged" );

	if ( [theEvent buttonNumber] == 2 )
	{
		// [!] : middle button

		CGPoint pt_cur = n_mac_cursor_position_get( self );


		CGFloat dx = paint->pt_prv.x - pt_cur.x;
		CGFloat dy = paint->pt_prv.y - pt_cur.y;
//NSLog( @"%f %f", dx, dy );


		paint->scroll.x += dx;
		paint->scroll.y += dy;
//NSLog( @"%f %f", paint->scroll.x, paint->scroll.y );


		paint->pt_prv = pt_cur;


		[self display];
	}

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

- (void) NonnonPaint_keyDown_pen : (NSEvent*) event
{

	if ( paint->tooltype != N_PAINT_TOOL_TYPE_PEN ) { return; }

	switch( event.keyCode ) {

	case N_MAC_KEYCODE_UNDO: // [!] : 'Z'

		if ( paint->pen_quick_blur_onoff ) { break; }

		if ( paint->pen_quick_eraser_whole_layer_onoff ) { break; }

		paint->pen_quick_eraser_onoff = TRUE;

		[self resetCursorRects];

	break;

	case N_MAC_KEYCODE_CUT: // [!] : 'X'

		if ( paint->pen_quick_eraser_onoff ) { break; }

		if ( paint->pen_quick_eraser_whole_layer_onoff ) { break; }

		paint->pen_quick_blur_onoff = TRUE;

		[self resetCursorRects];

	break;

	case N_MAC_KEYCODE_COPY: // [!] : 'C'

		if ( paint->pen_quick_blur_onoff ) { break; }

		if ( paint->pen_quick_eraser_onoff ) { break; }

		if ( paint->layer_onoff )
		{
			if ( paint->grabber_mode ) { break; }

			paint->pen_quick_eraser_whole_layer_onoff = TRUE;

			[self resetCursorRects];
		}

	break;

	} // switch

}

- (void) NonnonPaint_keyUp_pen : (NSEvent*) event
{

	if ( paint->tooltype != N_PAINT_TOOL_TYPE_PEN ) { return; }

	switch( event.keyCode ) {

	case N_MAC_KEYCODE_UNDO: // [!] : 'Z'

		paint->pen_quick_eraser_onoff = FALSE;
		paint->pen_start              = n_posix_false;

		[self resetCursorRects];

	break;

	case N_MAC_KEYCODE_CUT: // [!] : 'X'

		paint->pen_quick_blur_onoff = FALSE;

		[self resetCursorRects];

	break;

	case N_MAC_KEYCODE_COPY: // [!] : 'C'

		paint->pen_quick_eraser_whole_layer_onoff = FALSE;

		[self resetCursorRects];

	break;

	} // switch

}

- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Key Code = %d", event.keyCode );

	switch( event.keyCode ) {


	case N_MAC_KEYCODE_ARROW_UP :

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyDown_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_ARROW_DOWN :

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyDown_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_ARROW_LEFT :

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyDown_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_ARROW_RIGHT:

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyDown_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_UNDO: // [!] : 'Z'

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		paint->grabber_frame_hide = TRUE;

	break;

	case N_MAC_KEYCODE_COPY: // [!] : 'C'

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
			{
				n_paint_grabber_copy();
			}
		}

	break;

	case N_MAC_KEYCODE_PASTE: // [!] : 'V'

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
			{
				n_paint_grabber_paste();
			}
		}

	break;

	case N_MAC_KEYCODE_SELECT_ALL: // [!] : 'A'

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
			{
				n_paint_grabber_select_all();
			}
		}
	break;

	case N_MAC_KEYCODE_F2: 

		[self.delegate keyDown:event];

	break;

	case N_MAC_KEYCODE_F3: 

		n_paint_layer_info();

	break;

	case N_MAC_KEYCODE_NUMBER_1: 

		[self.delegate keyDown:event];

	break;

	} // switch

	[self NonnonPaint_keyDown_pen:event];

}

- (void) keyUp : (NSEvent*) event
{

	switch( event.keyCode ) {

	case N_MAC_KEYCODE_ARROW_UP :

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyUp_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_ARROW_DOWN :

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyUp_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_ARROW_LEFT :

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyUp_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_ARROW_RIGHT:

		if ( paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { break; }

		NonnonPaintGrabber_keyUp_arrow( paint, event.keyCode );

	break;

	case N_MAC_KEYCODE_UNDO: // [!] : 'Z'

		paint->grabber_frame_hide = FALSE;

	break;

	} // switch

	[self NonnonPaint_keyUp_pen:event];

}




@end




#endif // _H_NONNON_MAC_NONNON_PAINT_CANVAS

