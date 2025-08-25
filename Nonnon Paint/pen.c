// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




// internal
int
n_paint_average( u32 color )
{

	int a = n_bmp_a( color );
	int r = n_bmp_r( color );
	int g = n_bmp_g( color );
	int b = n_bmp_b( color );


	return ( a + r + g + b ) / 4;
}




n_type_real
n_paint_pen_air( n_paint *paint, n_type_gfx x, n_type_gfx y, n_type_gfx radius, n_type_real coeff, n_type_real blend )
{

	coeff *= blend;


	if ( paint->air )
	{
		if ( 0 == n_random_range( paint->air ) )
		{
			coeff = coeff * ( 1.0 / 7.5 );
		} else {
			coeff = 0;
		}
	}


	return coeff;
}




void
n_paint_pen_display( n_paint *paint, n_type_gfx px, n_type_gfx py, n_type_gfx radius )
{
//[n_paint_global display]; return;

	radius++;

	if ( paint->zoom < 0 ) { radius *= paint->zoom * -1; }

	n_type_gfx  x = px - radius;
	n_type_gfx  y = py - radius;
	n_type_gfx sx = ( radius * 2 ) + 1;
	n_type_gfx sy = ( radius * 2 ) + 1;

	if ( paint->layer_onoff )
	{
		if ( paint->layer_data[ paint->layer_index ].blur )
		{
			n_type_gfx blur = paint->layer_data[ paint->layer_index ].blur * 2;

			 x -= blur;
			 y -= blur;
			sx += blur * 2;
			sy += blur * 2;
		}
	}

	n_bmp_box( &paint->layer_cache_bmp_data, x,y,sx,sy, 0 );

	if ( paint->layer_onoff )
	{
		if ( paint->layer_data[ paint->layer_index ].blur )
		{
			n_bmp_box( &paint->layer_data[ paint->layer_index ].bmp_blur, x,y,sx,sy, 0 );
		}
	}

	[n_paint_global n_paint_convert_bitmap2canvas:nil x:&x y:&y sx:&sx sy:&sy];

	NSRect rect = NSMakeRect( x,y,sx,sy );

	paint->redraw_type = N_PAINT_REDRAW_TYPE_PEN;

	[n_paint_global displayRect:rect];


	return;
}

// internal
void
n_paint_pen_engine( n_type_gfx fx, n_type_gfx fy, n_type_real blend )
{

	n_paint *paint = n_paint_global.paint;


	if ( blend == 0.0 ) { return; }


	n_type_gfx radius = paint->pen_radius * n_paint_global.pressure;
	u32        color  = paint->pen_color;


//NSLog( @"%d", paint->pen_radius );
	if ( radius == 0 )
	{

		n_type_gfx tx = fx;
		n_type_gfx ty = fy;


		// [!] : don't use -1 == 0xffffffff == n_bmp_argb( 255,255,255,255 )

		u32 color_f = color + 1;
		u32 color_t = color;


		n_paint_grabber_pixel_get( tx,ty, color, &color_f, &color_t, 0.00 );


		u32 color_ret = n_bmp_blend_pixel_force_move( color_f, color_t, blend );


		n_paint_grabber_pixel_set( tx,ty, color_f, color_ret );


		n_paint_pen_display( paint, fx,fy, 1 );


		return;
	}


	radius++;


	// Multi-Layer Eraser

	n_posix_bool multi_layer_eraser = n_posix_false;

	if (
		( paint->layer_whole_grab_onoff )
		&&
		( paint->grabber_mode != N_PAINT_GRABBER_NEUTRAL )
		&&
		( color == n_bmp_white_invisible )
	)
	{
		multi_layer_eraser = n_posix_true;
	}


	n_type_gfx x = -radius;
	n_type_gfx y = -radius;
	n_posix_loop
	{

		n_type_real coeff;

		if ( n_bmp_ellipse_detect_coeff( x,y, radius,radius, &coeff ) )
		{

			n_type_gfx tx = fx + x;
			n_type_gfx ty = fy + y;


			// [!] : don't use -1 == 0xffffffff == n_bmp_argb( 255,255,255,255 )

			u32 color_f = color + 1;
			u32 color_t = color;

			if ( paint->pen_quick_eraser_whole_layer_onoff )
			{

				n_type_gfx orig_x = tx;
				n_type_gfx orig_y = ty;

				n_type_int i = 0;
				n_posix_loop
				{

					if ( n_posix_false == n_paint_layer_is_locked( i ) )
					{
						n_bmp *bmp_data = &paint->layer_data[ i ].bmp_data;

						u32 c1 = n_bmp_white_invisible;
						u32 c2; n_bmp_ptr_get( bmp_data, orig_x,orig_y, &c2 );

						n_type_real d = n_paint_pen_air( paint, x,y, radius, coeff, blend );

						c1 = n_bmp_blend_pixel_force_move( c2, c1, d );
						n_bmp_ptr_set( bmp_data, orig_x,orig_y, c1 );

						n_bmp_ptr_set( &paint->layer_cache_bmp_data, orig_x,orig_y, 0 );
						n_bmp_ptr_set( &paint->layer_cache_bmp_pre , orig_x,orig_y, 0 );
						n_bmp_ptr_set( &paint->layer_cache_bmp_pst , orig_x,orig_y, 0 );
					}

					i++;
					if ( i >= paint->layer_count ) { break; }
				}

			} else
			if ( multi_layer_eraser )
			{
//color_t = n_bmp_rgb( 0,200,255 );

				n_type_gfx gx,gy; n_paint_grabber_system_get( &gx,&gy, NULL,NULL, NULL,NULL );

				n_type_gfx orig_x = tx;
				n_type_gfx orig_y = ty;
				n_type_gfx grab_x = tx - gx;
				n_type_gfx grab_y = ty - gy;

				n_type_int i = 0;
				n_posix_loop
				{

					n_bmp *bmp_data = &paint->layer_data[ i ].bmp_data;
					n_bmp *bmp_grab = &paint->layer_data[ i ].bmp_grab;

					u32 c1;
					n_bmp_ptr_get( bmp_data, orig_x,orig_y, &c1 );

					u32 c2;
					n_bmp_ptr_get( bmp_grab, grab_x,grab_y, &c2 );

					n_type_real d = n_paint_pen_air( paint, x,y, radius, coeff, blend );

					c1 = n_bmp_blend_pixel_force_move( c2, c1, d );
					n_bmp_ptr_set( bmp_grab, grab_x,grab_y,  c1 );

					i++;
					if ( i >= paint->layer_count ) { break; }
				}

			} else
			if ( paint->mix == 100 )
			{

				n_paint_grabber_pixel_get( tx,ty, color, &color_f, &color_t, 0.00 );
				n_paint_grabber_pixel_set( tx,ty, color_f, color_t );

			} else {

				if ( paint->boost != 0 )
				{
					if ( paint->pensize == 0 )
					{
						if ( ( x == 0 )&&( y == 0 ) )
						{
							coeff /= radius;
						} else {
							coeff = 0.0;
						}
					} else
					if ( paint->pensize == 1 )
					{
						if ( ( x ==  0 )&&( y ==  0 ) )
						{
							coeff /= radius * 0.5;
						} else
						if ( ( x == -1 )&&( y ==  0 ) )
						{
							coeff /= radius;
						} else
						if ( ( x ==  1 )&&( y ==  0 ) )
						{
							coeff /= radius ;
						} else
						if ( ( x ==  0 )&&( y == -1 ) )
						{
							coeff /= radius;
						} else
						if ( ( x ==  0 )&&( y ==  1 ) )
						{
							coeff /= radius;
						} else
						if ( ( x == -1 )&&( y == -1 ) )
						{
							coeff /= radius;
							coeff *= 2.0 - sqrt( 2 );
						} else
						if ( ( x ==  1 )&&( y == -1 ) )
						{
							coeff /= radius;
							coeff *= 2.0 - sqrt( 2 );
						} else
						if ( ( x == -1 )&&( y ==  1 ) )
						{
							coeff /= radius;
							coeff *= 2.0 - sqrt( 2 );
						} else
						if ( ( x ==  1 )&&( y ==  1 ) )
						{
							coeff /= radius;
							coeff *= 2.0 - sqrt( 2 );
						} else
						//
						{
							coeff = 0.0;
						}
					} else {
						if ( n_bmp_ellipse_detect_coeff( x,y, paint->pensize,paint->pensize, NULL ) )
						{
							coeff /= radius;
						} else {
							coeff = 0.0;
						}
					}

					coeff *= (n_type_real) ( 100 - paint->boost ) * 0.01;
				}

				if ( coeff != 0.0 )
				{
				
					if ( paint->pen_quick_blur_onoff )
					{
						n_paint_grabber_pixel_get( tx,ty, color, &color_f, &color_t, 1.00 );
						n_paint_grabber_pixel_set( tx,ty, color_f - 1, color_f );
					} else {
						if ( paint->air )
						{
							n_paint_grabber_pixel_get( tx,ty, color, &color_f, &color_t, 0.25 );
						} else {
							n_paint_grabber_pixel_get( tx,ty, color, &color_f, &color_t, 0.00 );
						}

						coeff = n_paint_pen_air( paint, x,y, radius, coeff, blend );

						u32 color_ret = n_bmp_blend_pixel_force_move( color_f, color_t, coeff );
						n_paint_grabber_pixel_set( tx,ty, color_f, color_ret );
					}

				}

			}

		}


		x++;
		if ( x > radius )
		{

			x = -radius;

			y++;
			if ( y > radius ) { break; }
		}

	}


	n_paint_pen_display( paint, fx,fy, paint->pen_radius );


	return;
}




#define N_PAINT_PEN_START 0
#define N_PAINT_PEN_LOOP  1
#define N_PAINT_PEN_STOP  2

#define n_paint_pen_start( p, pt ) n_paint_pen( p, pt, N_PAINT_PEN_START )
#define n_paint_pen_loop(  p, pt ) n_paint_pen( p, pt, N_PAINT_PEN_LOOP  )
#define n_paint_pen_stop(  p, pt ) n_paint_pen( p, pt, N_PAINT_PEN_STOP  )

void
n_paint_pen( n_paint *paint, NSPoint pt, int mode )
{

	static n_type_gfx px;
	static n_type_gfx py;


	n_type_gfx fx,fy;
	n_type_gfx tx,ty;

	tx = pt.x;
	ty = pt.y;

	if ( mode == N_PAINT_PEN_START )
	{

		paint->pen_start_x = px = fx = tx;
		paint->pen_start_y = py = fy = ty;

	} else {

		if ( mode == N_PAINT_PEN_STOP )
		{
			if ( ( paint->pen_start_x == tx )&&( paint->pen_start_y == ty ) ) { return; }
		}

		fx = px;
		fy = py;

	}


	if (
		( mode == N_PAINT_PEN_LOOP )
		&&
		( paint->pen_blend != 1.0 )
	)
	{

		// [!] : nothing to do

		if ( ( fx == tx )&&( fy == ty ) ) { return; }

	}


	if ( paint->pen_blend == 1.0 )
	{
		n_bmp_pen_bresenham( fx,fy, tx,ty, paint->pen_blend, n_paint_pen_engine );
	} else {
		n_bmp_pen_wu       ( fx,fy, tx,ty, paint->pen_blend, n_paint_pen_engine );
	}

	px = tx;
	py = ty;


	[n_paint_global.delegate NonnonPaintStatus];


	return;
}

void
NonnonPaintPen_mouseDown( n_paint *paint, NSEvent *theEvent )
{

	if ( paint->layer_onoff )
	{
		if ( n_paint_layer_is_locked( paint->layer_index ) )
		{
			return;
		}
	}


	if ( paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
	{

		if ( paint->pen_start == n_posix_false )
		{

			// [!] : pre-calculate

			paint->pen_radius =  paint->pensize / 2;
			paint->pen_blend  = (n_type_real) paint->mix * 0.01;


			// Quick Eraser

			if ( n_mac_keystate_get( N_MAC_KEYCODE_UNDO ) )
			{
				paint->pen_color = n_bmp_white_invisible;
			} else {
				paint->pen_color = paint->color;
			}


			// [!] : the first input
//NSLog( @"Pen Start" );
			paint->pen_start = n_posix_true;

			n_paint_pen_start( paint, [n_paint_global n_paint_point_on_bitmap] );


			if ( paint->grabber_mode )
			{
				paint->grabber_frame_lock = TRUE;
			}

		}

	} else
	if ( paint->tooltype == N_PAINT_TOOL_TYPE_FILL )
	{

		NSPoint pt = [n_paint_global n_paint_point_on_bitmap];

		NSUInteger flags = [[NSApp currentEvent] modifierFlags];
		if ( flags & NSEventModifierFlagCommand )
		{
			n_paint_grabber_fill_special( pt.x, pt.y, paint->color );
		} else {
			n_paint_grabber_fill( pt.x, pt.y, paint->color );
		}


		if ( paint->grabber_mode )
		{
			paint->grabber_frame_lock = TRUE;
		}


		[n_paint_global display_optimized];

	}


	//n_paint_colorhistory_add( n_paint_pen_color );

}

void
NonnonPaintPen_mouseDragged( n_paint *paint )
{

	[n_paint_global.delegate NonnonPaintStatus];

	if ( paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
	{
		if ( paint->pen_start )
		{
//NSLog( @"Pen Loop" );
			n_paint_pen_loop( paint, [n_paint_global n_paint_point_on_bitmap] );

		}
	} else
	if ( paint->tooltype == N_PAINT_TOOL_TYPE_FILL )
	{
		//
	}
	
}

void
NonnonPaintPen_mouseUp( n_paint *paint )
{

	if ( paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
	{

		if ( paint->pen_start )
		{
//NSLog( @"Pen Stop" );
			n_paint_pen_stop( paint, [n_paint_global n_paint_point_on_bitmap] );
			paint->pen_start = n_posix_false;

		}
	} else
	if ( paint->tooltype == N_PAINT_TOOL_TYPE_FILL )
	{
		//
	}

}

BOOL
NonnonPaintPen_colorpicker_margin( n_paint *paint )
{

	BOOL ret = FALSE;

	if ( paint->margin_onoff == FALSE ) { return FALSE; }


	NSPoint pt_m = n_mac_cursor_position_get( n_paint_global );
//NSLog( @"%0.0f %0.0f", pt_m.x, pt_m.y );
//NSLog( @"Offset Point : %0.0f %0.0f", paint->canvas_offset_x , paint->canvas_offset_y  );
//NSLog( @"Offset Size  : %0.0f %0.0f", paint->canvas_offset_sx, paint->canvas_offset_sy );

	if (
		( pt_m.x < paint->canvas_offset_x )
		||
		( pt_m.y < paint->canvas_offset_y )
		||
		( pt_m.x > ( paint->canvas_offset_x + paint->canvas_offset_sx ) )
		||
		( pt_m.y > ( paint->canvas_offset_y + paint->canvas_offset_sy ) )
	)
	{
		paint->color = n_bmp_white_invisible;
		ret = TRUE;
	}


	return ret;
}

void
NonnonPaintPen_rightMouseDown( n_paint *paint, NSEvent *event )
{
//NSLog( @"NonnonPaintPen_rightMouseDown()" );

	if (
		( paint->tooltype == N_PAINT_TOOL_TYPE_PEN )
		||
		( paint->tooltype == N_PAINT_TOOL_TYPE_FILL )
	)
	{
		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			NSPoint pt = n_mac_cursor_position_get( n_paint_global );

			n_bmp_ptr_get( [n_paint_global n_paint_nbmp_get], pt.x, pt.y, &paint->color );
		} else {
			if ( NonnonPaintPen_colorpicker_margin( paint ) )
			{
				//
			} else
			if ( paint->grabber_mode )
			{
//NSLog( @"Grabber" );
				NSPoint pt = [n_paint_global n_paint_point_on_bitmap];

				n_paint_grabber_pixel_get( pt.x, pt.y, n_bmp_white_invisible, &paint->color, NULL, 0.00 );
			} else {
				NSPoint pt = [n_paint_global n_paint_point_on_bitmap];

				if ( n_bmp_ptr_get( paint->pen_bmp_data, pt.x, pt.y, &paint->color ) )
				{
					paint->color = n_bmp_white_invisible;
				}
			}
		}

		extern void n_paint_colorhistory_add( u32 color );
		n_paint_colorhistory_add( paint->color );
	}

}

