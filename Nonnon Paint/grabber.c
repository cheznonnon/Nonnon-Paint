// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




// internal
void
n_paint_grabber_system_get( n_type_gfx *x, n_type_gfx *y, n_type_gfx *sx, n_type_gfx *sy, n_type_gfx *fx, n_type_gfx *fy )
{

	// [!] : Why is this messy?
	//
	//	a : for reserving namespace
	//	b : for avoiding silent reference to global variables

	n_rect r = n_paint->grabber_rect;
	
	if ( x  != NULL ) { *x  = r.x ; }
	if ( y  != NULL ) { *y  = r.y ; }
	if ( sx != NULL ) { *sx = r.sx; }
	if ( sy != NULL ) { *sy = r.sy; }
	if ( fx != NULL ) { *fx = n_paint->grabber_fx; }
	if ( fy != NULL ) { *fy = n_paint->grabber_fy; }


	return;
}

// internal
void
n_paint_grabber_system_set( n_type_gfx *x, n_type_gfx *y, n_type_gfx *sx, n_type_gfx *sy, n_type_gfx *fx, n_type_gfx *fy )
{

	n_paint_struct *p = n_paint;

	if ( x  != NULL ) { p->grabber_rect.x  = *x ; }
	if ( y  != NULL ) { p->grabber_rect.y  = *y ; }
	if ( sx != NULL ) { p->grabber_rect.sx = *sx; }
	if ( sy != NULL ) { p->grabber_rect.sy = *sy; }
	if ( fx != NULL ) { p->grabber_fx      = *fx; }
	if ( fy != NULL ) { p->grabber_fy      = *fy; }


	return;
}

// internal
void
n_paint_grabber_system_zero( void )
{

	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );

	x = y = sx = sy = fx = fy = 0;

	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


	return;
}

// internal
void
n_paint_grabber_init( void )
{

	n_paint_struct *p = n_paint;


	p->grabber_mode = N_PAINT_GRABBER_NEUTRAL;

	n_paint_grabber_system_zero();


	[n_paint_global n_paint_grabber_frame_anim_off];


	[n_paint_global n_paint_layer_window_onoff:TRUE];


	p->grabber_is_key_input      = 0;
	p->grabber_is_key_input_fade = FALSE;


	return;
}

// internal
void
n_paint_grabber_resync( n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy )
{

	n_paint_struct *p = n_paint;


	if ( ( p->layer_onoff )&&( p->grabber_mode != N_PAINT_GRABBER_SELECTING ) )
	{
		n_type_gfx px  = p->grabber_cache_redraw_x;
		n_type_gfx py  = p->grabber_cache_redraw_y;
		n_type_gfx psx = p->grabber_cache_redraw_sx;
		n_type_gfx psy = p->grabber_cache_redraw_sy;

		n_bmp_box( &p->layer_cache_bmp_data, px,py,psx,psy, 0 );

		p->grabber_cache_redraw_x  = x;
		p->grabber_cache_redraw_y  = y;
		p->grabber_cache_redraw_sx = sx;
		p->grabber_cache_redraw_sy = sy;
	}


	n_type_gfx cx  =  x;
	n_type_gfx cy  =  y;
	n_type_gfx csx = sx;
	n_type_gfx csy = sy;


	// [!] : add frame

	n_type_gfx area;

	if ( p->grabber_frame_mode == N_PAINT_GRABBER_FRAME_MODE_EDGE )
	{
		extern n_type_gfx n_paint_grabber_frame_edge_redraw_area_get( void );
		area = n_paint_grabber_frame_edge_redraw_area_get();

	} else {
		extern n_type_gfx n_paint_grabber_frame_pixel_redraw_area_get( void );
		area = n_paint_grabber_frame_pixel_redraw_area_get();
	}

	cx  -= area;
	cy  -= area;
	csx += area * 2;
	csy += area * 2;


	[n_paint_global n_paint_convert_bitmap2canvas:nil x:&cx y:&cy sx:&csx sy:&csy];
//NSLog( @"C : %d %d %d %d", cx,cy,csx,csy );

	if ( p->zoom < 0 )
	{
		cx  -= area;
		cy  -= area;
		csx += area * 2;
		csy += area * 2;
	}


	n_type_gfx rx  = p->grabber_resync_rect.origin.x;
	n_type_gfx ry  = p->grabber_resync_rect.origin.y;
	n_type_gfx rsx = p->grabber_resync_rect.size.width;
	n_type_gfx rsy = p->grabber_resync_rect.size.height;

	if ( rx  == 0 ) { rx  =  cx; }
	if ( ry  == 0 ) { ry  =  cy; }
	if ( rsx == 0 ) { rsx = csx; }
	if ( rsy == 0 ) { rsy = csy; }
//NSLog( @"R : %d %d %d %d", rx,ry,rsx,rsy );


	CGFloat tx  = MIN( cx, rx );
	CGFloat ty  = MIN( cy, ry );
	CGFloat tsx = MAX( cx + csx, rx + rsx ) - tx;
	CGFloat tsy = MAX( cy + csy, ry + rsy ) - ty;
//NSLog( @"T : %0.0f %0.0f %0.0f %0.0f", tx,ty,tsx,tsy );

	p->redraw_type = N_PAINT_REDRAW_TYPE_PEN;
	[n_paint_global displayRect:NSMakeRect( tx,ty,tsx,tsy )];[n_paint_global displayRect:NSMakeRect( tx,ty,tsx,tsy )];

	p->grabber_resync_rect = NSMakeRect( cx,cy,csx,csy );


	return;
}

void
n_paint_grabber_resync_auto( void )
{
//[n_paint_global display_optimized]; return;


	[n_paint_global displayRect:n_paint->grabber_tooltip_rect_drawn];


	n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );

	n_paint_grabber_resync( x,y,sx,sy );


	return;
}





// internal
void
n_paint_grabber_pixel_get( n_type_gfx tx, n_type_gfx ty, u32 picked, u32 *before, u32 *after, n_type_real blend )
{

	n_paint_struct *p = n_paint;


	u32 color_b, color_a;


	// [x] : don't use _fast()

	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{

		//n_bmp_ptr_get( n_paint_bmp_data, tx,ty, &color_b );

		color_b = n_bmp_antialias_pixel( p->pen_bmp_data, tx,ty, blend );
		color_a = picked;

	} else {

		n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );


		n_type_gfx gx = tx - x;
		n_type_gfx gy = ty - y;

		if ( n_bmp_ptr_is_accessible( n_paint->pen_bmp_grab, gx,gy ) )
		{
			color_b = n_bmp_antialias_pixel( p->pen_bmp_grab, gx,gy, blend );
		} else {
			color_b = n_bmp_antialias_pixel( p->pen_bmp_data, tx,ty, blend );
		}


		// [!] : Eraser

		if ( N_BMP_ALPHA_CHANNEL_INVISIBLE == n_bmp_a( picked ) )
		//if ( n_false )
		{
			n_bmp_ptr_get( p->pen_bmp_data, tx,ty, &color_a );
		} else {
			color_a = picked;
		}

	}


	if ( before != NULL ) { (*before) = color_b; }
	if ( after  != NULL ) { (*after ) = color_a; }


	return;
}

// internal
void
n_paint_grabber_pixel_set( n_type_gfx tx, n_type_gfx ty, u32 before, u32 after )
{

	n_paint_struct *p = n_paint;


	if ( before == after ) { return; }


	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{

		n_bmp_ptr_set( p->pen_bmp_data, tx,ty, after );

	} else {

		n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );

/*
		int a = n_bmp_a( before );

		if ( a == N_BMP_ALPHA_CHANNEL_INVISIBLE )
		{
			n_bmp_ptr_get( n_paint_bmp_data, tx,ty, &after );
		}
*/

		n_type_gfx grab_x = tx - x;
		n_type_gfx grab_y = ty - y;

		n_bmp_ptr_set( p->pen_bmp_grab, grab_x,grab_y, after );

	}


	return;
}

// internal
void
n_paint_grabber_fill( n_type_gfx tx, n_type_gfx ty, u32 color )
{

	n_paint_struct *p = n_paint;


	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{

		n_bmp_fill( p->pen_bmp_data, tx, ty, color );

	} else {

		n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );

		n_type_gfx grab_x = tx - x;
		n_type_gfx grab_y = ty - y;

		n_bmp_fill( p->pen_bmp_grab, grab_x,grab_y, color );

	}

	if ( p->layer_onoff )
	{
		n_bmp_flush( &p->layer_data[ p->layer_index ].bmp_blur, 0 );
	}


	return;
}

// internal
void
n_paint_grabber_fill_special( n_type_gfx tx, n_type_gfx ty, u32 color )
{

	n_paint_struct *p = n_paint;


	n_type_gfx gx, gy;
	{
		n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );

		gx = tx - x;
		gy = ty - y;
	}

/*
	u32 color_orig;
	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		n_bmp_ptr_get( p->pen_bmp_data, tx, ty, &color_orig );
	} else {
		n_bmp_ptr_get( p->pen_bmp_grab, gx, gy, &color_orig );
	}
*/

	n_bmp bmp; n_bmp_zero( &bmp );

	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		n_bmp_carboncopy( p->pen_bmp_data, &bmp );
	} else {
		n_bmp_carboncopy( p->pen_bmp_grab, &bmp );
	}

	{
		n_type_index c = N_BMP_SX( &bmp ) * N_BMP_SY( &bmp );
		n_type_index i = 0;
		n_posix_loop
		{

			u32 clr = N_BMP_PTR( &bmp )[ i ];

			if ( n_bmp_a( clr ) )
			{
				N_BMP_PTR( &bmp )[ i ] = n_bmp_black;
			}

			i++;
			if ( i >= c ) { break; }
		}
	}

	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		n_bmp_fill( &bmp, tx, ty, color );
	} else {
		n_bmp_fill( &bmp, gx, gy, color );
	}


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 c1; n_bmp_ptr_get_fast( &bmp, x, y, &c1 );

		if ( c1 != n_bmp_black )
		{
			u32 c2;
			if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
			{
				n_bmp_ptr_get_fast( p->pen_bmp_data, x, y, &c2 );
			} else {
				n_bmp_ptr_get_fast( p->pen_bmp_grab, x, y, &c2 );
			}

			int a = n_posix_min( n_bmp_a( c1 ), n_bmp_a( c2 ) );
			int r = n_bmp_r( c1 );
			int g = n_bmp_g( c1 );
			int b = n_bmp_b( c1 );

			u32 c3 = n_bmp_argb( a,r,g,b );

			if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
			{
				n_bmp_ptr_set_fast( p->pen_bmp_data, x, y, c3 );
			} else {
				n_bmp_ptr_set_fast( p->pen_bmp_grab, x, y, c3 );
			}
		}

		x++;
		if ( x >= N_BMP_SX( &bmp ) )
		{
			x = 0;
			y++;
			if ( y >= N_BMP_SY( &bmp ) ) { break; }
		}
	}


	return;
}




// internal
void
n_paint_grabber_composition( n_bmp *grab, n_bmp *target, BOOL finalize, BOOL frame )
{

	n_paint_struct *p = n_paint;


	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL ) { return; }


	n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );

//return;
	if ( p->grabber_per_pixel_alpha_onoff )
	{
		n_bmp_blendcopy_all( grab, target, 0,0,sx,sy, x,y, FALSE,FALSE, finalize, n_paint->grabber_blend_ratio );
	} else
	if ( p->grabber_blend )
	{
		n_bmp_blendcopy_all( grab, target, 0,0,sx,sy, x,y, TRUE ,FALSE, finalize, n_paint->grabber_blend_ratio );
	} else {
		n_bmp_fastcopy( grab, target, 0,0,sx,sy, x,y );
	}


	return;
}

// internal
void
n_paint_grabber_reset( void )
{

	n_paint_struct *p = n_paint;


	if ( p->layer_onoff )
	{
//NSLog( @"n_paint_grabber_reset()" );

		n_type_int i = 0;
		n_posix_loop
		{

			n_posix_char *str = n_txt_get( &p->layer_decoration_txt, i );
			if ( str[ 0 ] == 'u' )
			{
				str[ 0 ] = ' ';
			} else
			if ( str[ 0 ] == 'U' )
			{
				str[ 0 ] = 'B';
			}

			n_bmp_free( &p->layer_data[ i ].bmp_grab );

			i++;
			if ( i >= p->layer_count ) { break; }
		}

		[n_paint_global.txtbox display];

	} else {

		n_bmp_free( p->pen_bmp_grab );

	}

	n_paint_grabber_init();

	[n_paint_global display_optimized];


	// [x] : cannot connect to AppDelegate

	//p->grabber_per_pixel_alpha_onoff = FALSE;
	//p->grabber_blend                 = 0;
	//p->grabber_blend_ratio           = 0;


	return;
}

// internal
void
n_paint_grabber_finish( void )
{

	n_paint_struct *p = n_paint;


	if ( p->layer_whole_grab_onoff )
	{

		n_type_int i = 0;
		n_posix_loop
		{

			n_posix_char *str = n_txt_get( &p->layer_decoration_txt, i );
			if ( str[ 0 ] == 'u' )
			{
				str[ 0 ] = ' ';
			} else
			if ( str[ 0 ] == 'U' )
			{
				str[ 0 ] = 'B';
			}

			if ( p->layer_data[ i ].visible )
			{
				n_paint_grabber_composition
				(
					&p->layer_data[ i ].bmp_grab,
					&p->layer_data[ i ].bmp_data,
					TRUE,//n_paint_grabber_is_rotated,
					FALSE
				);
			}

			i++;
			if ( i >= p->layer_count ) { break; }
		}

		[n_paint_global.txtbox display];

	} else
	if ( p->layer_onoff )
	{

		n_type_int i = p->layer_index;

		if ( p->layer_data[ i ].visible )
		{
			n_paint_grabber_composition
			(
				&p->layer_data[ i ].bmp_grab,
				&p->layer_data[ i ].bmp_data,
				TRUE,//n_paint_grabber_is_rotated,
				FALSE
			);
		}

	} else {
	
		n_paint_grabber_composition( p->pen_bmp_grab, p->pen_bmp_data, TRUE, FALSE );

	}


	if ( p->layer_onoff )
	{

		n_type_int i = 0;
		n_posix_loop
		{
			n_bmp_free( &p->layer_data[ i ].bmp_grab );

			i++;
			if ( i >= p->layer_count ) { break; }
		}

	} else {

		n_bmp_free( p->pen_bmp_grab );

	}

	n_paint_grabber_init();

	[n_paint_global.delegate NonnonPaintStatus];


	[n_paint_global display_optimized];


	return;
}

// internal
void
n_paint_grabber_select( n_bmp *b, BOOL redraw )
{

	n_paint_struct *p = n_paint;


	n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );


	if ( p->layer_whole_grab_onoff )
	{

		n_type_int i = 0;
		n_posix_loop
		{

			if ( n_paint_layer_is_locked( i ) )
			{
				//
			} else {
				n_posix_char *str = n_txt_get( &p->layer_decoration_txt, i );
				if ( str[ 0 ] == ' ' )
				{
					str[ 0 ] = 'u';
				} else
				if ( str[ 0 ] == 'B' )
				{
					str[ 0 ] = 'U';
				}

				if ( b == NULL )
				{

					n_bmp_new_fast( &p->layer_data[ i ].bmp_grab, sx,sy );
					n_bmp_fastcopy( &p->layer_data[ i ].bmp_data, &p->layer_data[ i ].bmp_grab, x,y,sx,sy, 0,0 );

				} else {

					p->pen_bmp_data = &p->layer_data[ i ].bmp_data;
					p->pen_bmp_grab = &p->layer_data[ i ].bmp_grab;

					n_bmp_free( &p->layer_data[ i ].bmp_grab );
					n_bmp_carboncopy( b, &p->layer_data[ i ].bmp_grab );

					sx = N_BMP_SX( b );
					sy = N_BMP_SY( b );

				}
			}

			i++;
			if ( i >= p->layer_count ) { break; }
		}

		[n_paint_global.txtbox display];

	} else
	if ( p->layer_onoff )
	{

		if ( b == NULL )
		{
//NSLog( @" 2-0 : %d ", p->layer_index );

			n_bmp_new_fast( p->pen_bmp_grab, sx,sy );
			n_bmp_fastcopy( p->pen_bmp_data, p->pen_bmp_grab, x,y,sx,sy, 0,0 );

		} else {
//NSLog( @" 2-1 " );

			n_type_int y = p->layer_index;

			p->pen_bmp_data = &p->layer_data[ y ].bmp_data;
			p->pen_bmp_grab = &p->layer_data[ y ].bmp_grab;

			n_bmp_free( p->pen_bmp_grab );
			n_bmp_carboncopy( b, p->pen_bmp_grab );

			sx = N_BMP_SX( b );
			sy = N_BMP_SY( b );

		}

	} else {

		if ( b == NULL )
		{
//NSLog( @" 3-1 " );
//NSLog( @"%d %d", sx,sy );

			n_bmp_new_fast( p->pen_bmp_grab, sx,sy );
			n_bmp_fastcopy( p->pen_bmp_data, p->pen_bmp_grab, x,y,sx,sy, 0,0 );
//n_bmp_flush( p->pen_bmp_grab, n_bmp_rgb( 0,200,255 ) );
//n_bmp_box( p->pen_bmp_data, x,y,sx,sy, n_bmp_rgb( 0,200,255 ) );

		} else {
//NSLog( @" 3-2 " );

			n_bmp_free( p->pen_bmp_grab );
			n_bmp_carboncopy( b, p->pen_bmp_grab );

			sx = N_BMP_SX( b );
			sy = N_BMP_SY( b );

		}

	}

	n_paint_grabber_system_set( &x,&y, &sx,&sy, NULL,NULL );


	if ( redraw )
	{
		n_paint_grabber_resync_auto();
		//[n_paint_global display_optimized];
	}


	return;
}

// internal
void
n_paint_grabber_select_drop( n_bmp *b, BOOL is_zero, BOOL redraw )
{

	n_paint_struct *p = n_paint;


	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );
//NSLog( @"%d %d %d %d", x, y, sx, sy );

	n_bmp_free( p->pen_bmp_grab );
	n_bmp_carboncopy( b, p->pen_bmp_grab );

	sx = N_BMP_SX( b );
	sy = N_BMP_SY( b );


	if ( is_zero )
	{
		x = y = 0;
	} else {
		NSPoint pos = [n_paint_global n_paint_point_on_bitmap];
//NSLog( @"%f %f", pos.x, pos.y );

		x = pos.x - ( sx / 2 );
		y = pos.y - ( sy / 2 );	
	}

	fx = x;
	fy = y;

	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


	if ( redraw )
	{
		n_paint_grabber_resync_auto();
		//[n_paint_global display_optimized];
	}


	return;
}

void
n_paint_grabber_position_reset( void )
{

	if ( n_paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL ) { return; }


	n_type_gfx x,y, fx,fy; n_paint_grabber_system_get( &x,&y, NULL,NULL, &fx,&fy );

	x = fx;
	y = fy;

	n_paint_grabber_system_set( &x,&y, NULL,NULL, &fx,&fy );

	[n_paint_global display_optimized];


	return;
}

// internal
void
n_paint_grabber_filter_go( n_bmp *b, int mode )
{

	const int mirror = N_BMP_MIRROR_LEFTSIDE_RIGHT;


	if ( mode == N_PAINT_FILTER_SCALE_LIL ) { n_bmp_scaler_lil(    b, 2                  ); } else
	if ( mode == N_PAINT_FILTER_SCALE_BIG ) { n_bmp_scaler_big(    b, 2                  ); } else
	if ( mode == N_PAINT_FILTER_MIRROR    ) { n_bmp_flush_mirror(  b, mirror             ); } else
	if ( mode == N_PAINT_FILTER_ROTATE_L  ) { n_bmp_rotate(        b, N_BMP_ROTATE_LEFT  ); } else
	if ( mode == N_PAINT_FILTER_ROTATE_R  ) { n_bmp_rotate(        b, N_BMP_ROTATE_RIGHT ); } else
	if ( mode == N_PAINT_FILTER_ALPHA_CLR ) { n_bmp_alpha_visible( b                     ); } else
	if ( mode == N_PAINT_FILTER_ALPHA_REV ) { n_bmp_alpha_reverse( b                     ); }// else


	return;
}

void
n_paint_grabber_filter_go_thread( int filter_type, BOOL data_onoff, BOOL grab_onoff )
{

	// [!] : performance
	//
	//	non-threaded : 935 msec.
	//	    threaded : 178 msec.

//u32 tick = n_posix_tickcount();


	BOOL prv = n_bmp_is_multithread;
	n_bmp_is_multithread = TRUE;


	n_type_int i = 0;
	n_posix_loop
	{//break;

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		if ( data_onoff )
		{
			n_paint_grabber_filter_go( &n_paint->layer_data[ i ].bmp_data, filter_type );
		}

		if ( grab_onoff )
		{
			n_paint_grabber_filter_go( &n_paint->layer_data[ i ].bmp_grab, filter_type );
		}

		}];
		[n_paint->queue addOperation:o];

		i++;
		if ( i >= n_paint->layer_count ) { break; }
	}


	[n_paint->queue waitUntilAllOperationsAreFinished];
	n_bmp_is_multithread = prv;


//NSLog( @"%d", n_posix_tickcount() - tick );


	return;
}

// internal
void
n_paint_grabber_filter_sync_calc( n_bmp *bmp_base, int filter_type )
{
//return;

	n_type_gfx psx = N_BMP_SX( bmp_base );
	n_type_gfx psy = N_BMP_SY( bmp_base );


	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );
//NSLog( @"Before : %d %d %d %d ", x, y, sx, sy );

	n_type_gfx px = x;
	n_type_gfx py = y;

	if ( filter_type == N_PAINT_FILTER_SCALE_LIL )
	{
		 x =  x / 2;
		 y =  y / 2;
		sx = sx / 2;
		sy = sy / 2;

		n_paint->scroll.x /= 2;
		n_paint->scroll.y /= 2;
	} else
	if ( filter_type == N_PAINT_FILTER_SCALE_BIG )
	{
//NSLog( @"N_PAINT_FILTER_SCALE_BIG" );
		 x =  x * 2;
		 y =  y * 2;
		sx = sx * 2;
		sy = sy * 2;

		n_paint->scroll.x *= 2;
		n_paint->scroll.y *= 2;
	} else
	if ( filter_type == N_PAINT_FILTER_MIRROR    )
	{
		x = psx - ( x + sx );
	} else
	if ( filter_type == N_PAINT_FILTER_ROTATE_L  )
	{
		x = py;
		y = psx - ( px + sx );

		CGFloat tmp = n_paint->scroll.x;
		n_paint->scroll.x = n_paint->scroll.y;
		n_paint->scroll.y = tmp;
	} else
	if ( filter_type == N_PAINT_FILTER_ROTATE_R  )
	{
		x = psy - ( py + sy );
		y = px;

		CGFloat tmp = n_paint->scroll.x;
		n_paint->scroll.x = n_paint->scroll.y;
		n_paint->scroll.y = tmp;
	}

	fx = x;
	fy = y;

//NSLog( @"After : %d %d %d %d ", x, y, sx, sy );

	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


	return;
}

// internal
void
n_paint_grabber_filter_sync_calc_for_grabbed_area( int filter_type, BOOL whole_canvas )
{
//return;

	n_paint_struct *p = n_paint;

	n_type_gfx psx = N_BMP_SX( &p->layer_data[ 0 ].bmp_data );
	n_type_gfx psy = N_BMP_SY( &p->layer_data[ 0 ].bmp_data );


	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );
//NSLog( @"Before : %d %d %d %d ", x, y, sx, sy );

	n_type_gfx px = x;
	n_type_gfx py = y;

	if ( filter_type == N_PAINT_FILTER_SCALE_LIL )
	{
		if ( whole_canvas )
		{
			x = x / 2;
			y = y / 2;
		}

		sx = sx / 2;
		sy = sy / 2;
	} else
	if ( filter_type == N_PAINT_FILTER_SCALE_BIG )
	{
		if ( whole_canvas )
		{
			x = x * 2;
			y = y * 2;
		}

		sx = sx * 2;
		sy = sy * 2;
	} else
	if ( filter_type == N_PAINT_FILTER_MIRROR    )
	{
		if ( whole_canvas )
		{
			x = psx - ( x + sx );
		}
	} else
	if ( filter_type == N_PAINT_FILTER_ROTATE_L  )
	{
		if ( whole_canvas )
		{
			x = py;
			y = psx - ( px + sx );
		}

		sx = N_BMP_SY( p->pen_bmp_grab );
		sy = N_BMP_SX( p->pen_bmp_grab );
	} else
	if ( filter_type == N_PAINT_FILTER_ROTATE_R  )
	{
		if ( whole_canvas )
		{
			x = psy - ( py + sy );
			y = px;
		}

		sx = N_BMP_SY( p->pen_bmp_grab );
		sy = N_BMP_SX( p->pen_bmp_grab );
	}

	fx = x;
	fy = y;

//NSLog( @"After : %d %d %d %d ", x, y, sx, sy );

	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


	return;
}

// internal
BOOL
n_paint_grabber_filter_sync( int filter_type, BOOL calc, BOOL refresh )
{

	n_paint_struct *p = n_paint;


	if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL ) { return FALSE; }


	if ( p->tooltype != N_PAINT_TOOL_TYPE_GRABBER )
	{
		n_paint_grabber_filter_go( p->pen_bmp_data, filter_type );
	}


	if ( calc )
	{
		n_paint_grabber_filter_sync_calc( p->pen_bmp_data, filter_type );
	}


	n_bmp b; n_bmp_carboncopy( p->pen_bmp_grab, &b );

	n_paint_grabber_filter_go( &b, filter_type );

//n_paint_grabber_select( &b, n_false );

	{
		n_bmp_free( p->pen_bmp_grab );
		n_bmp_alias( &b, p->pen_bmp_grab );

		if ( calc )
		{
			n_type_gfx sx = N_BMP_SX( &b );
			n_type_gfx sy = N_BMP_SY( &b );

			n_paint_grabber_system_set( NULL,NULL, &sx,&sy, NULL,NULL );
		}

		if ( refresh )
		{
			n_paint_grabber_resync_auto();
			//[n_paint_global display_optimized];
		}
	}


	[n_paint_global.delegate NonnonPaintStatus];


	return TRUE;
}

#define N_PAINT_GRABBER_LOAD_FILE   ( 1 )
#define N_PAINT_GRABBER_LOAD_FILTER ( 2 )

#define n_paint_grabber_file(   f ) n_paint_grabber_load(    f, N_PAINT_GRABBER_LOAD_FILTER, 0 )
#define n_paint_grabber_filter( f ) n_paint_grabber_load( NULL, N_PAINT_GRABBER_LOAD_FILTER, f )

// internal
BOOL
n_paint_grabber_load( n_posix_char *name, int mode, int filter_type )
{

	n_paint_struct *p = n_paint;


	if ( p->grabber_mode != N_PAINT_GRABBER_NEUTRAL )
	{
		p->grabber_frame_lock = TRUE;
	}


	n_bmp b; n_bmp_zero( &b );


	if ( mode == N_PAINT_GRABBER_LOAD_FILE )
	{

		if ( p->tooltype != N_PAINT_TOOL_TYPE_GRABBER )
		{
			if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
			{
				return FALSE;
			} else {
				return TRUE;
			}
		}


		if ( p->layer_onoff )
		{
			if ( n_paint_layer_is_locked( p->layer_index ) ) { return TRUE; }
		}


		n_curico c; n_curico_zero( &c );

		if ( n_paint_load( name, &b, &c ) )
		{
			n_mac_window_dialog_ok( "Please Check" );

			return TRUE;
		}


		if ( p->layer_onoff )
		{
			n_type_int y = p->layer_index;

			p->pen_bmp_data = &p->layer_data[ y ].bmp_data;
			p->pen_bmp_grab = &p->layer_data[ y ].bmp_grab;
		}

	} else
	if ( mode == N_PAINT_GRABBER_LOAD_FILTER )
	{
//NSLog( @"N_PAINT_GRABBER_LOAD_FILTER" );

		if ( p->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
//NSLog( @"N_PAINT_GRABBER_LOAD_FILTER : N_PAINT_GRABBER_NEUTRAL" );

			if ( p->layer_onoff )
			{
				n_paint_grabber_filter_sync_calc( &p->layer_data[ 0 ].bmp_data, filter_type );
				n_paint_grabber_filter_go_thread( filter_type, YES, NO );
			} else {
				n_paint_grabber_filter_sync_calc( p->pen_bmp_data, filter_type );
				n_paint_grabber_filter_go( p->pen_bmp_data, filter_type );
			}

			return FALSE;
		}

		if ( p->layer_onoff )
		{
//NSLog( @"N_PAINT_GRABBER_LOAD_FILTER : layer_onoff" );

			if ( p->tooltype == N_PAINT_TOOL_TYPE_GRABBER )
			{
//NSLog( @"Grabber Only" );
				n_paint_grabber_filter_sync_calc_for_grabbed_area( filter_type, NO );

				n_paint_grabber_filter_go_thread( filter_type, NO, YES );

				n_paint_grabber_resync_auto();
			} else {
//NSLog( @"Whole Canvas" );
				n_paint_grabber_filter_sync_calc_for_grabbed_area( filter_type, YES );

				n_paint_grabber_filter_go_thread( filter_type, YES, YES );

				n_paint_grabber_resync_auto();
			}


			return FALSE;

		} else {
//NSLog( @"N_PAINT_GRABBER_LOAD_FILTER : %d", p->tooltype );

			if ( p->tooltype == N_PAINT_TOOL_TYPE_GRABBER )
			{
				n_bmp_carboncopy( p->pen_bmp_grab, &b );
				n_paint_grabber_filter_go( &b, filter_type );
			} else {
				return n_paint_grabber_filter_sync( filter_type, TRUE, TRUE );
			}

		}

	} else { return FALSE; }


	p->grabber_mode = N_PAINT_GRABBER_DRAG_OK;

	if ( p->layer_onoff )
	{
		[p->whole_preview_checkbox setEnabled:NO];
		[p->whole_grab_checkbox    setEnabled:NO];
	}


	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );


	if ( mode == N_PAINT_GRABBER_LOAD_FILE )
	{

		x = y = 0;

		fx = x;
		fy = y;

	} else
	if ( mode == N_PAINT_GRABBER_LOAD_FILTER )
	{

		//

	}

	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


	n_paint_grabber_select( &b, TRUE );


	n_bmp_free( &b );


	[n_paint_global.delegate NonnonPaintStatus];


	return TRUE;
}

void
n_paint_grabber_select_all( void )
{

	n_paint_struct *p = n_paint;

	if ( p->layer_onoff )
	{

		n_type_int i = 0;
		n_posix_loop
		{
			n_bmp_free( &p->layer_data[ i ].bmp_grab );

			i++;
			if ( i >= p->layer_count ) { break; }
		}

	} else {

		n_bmp_free( p->pen_bmp_grab );

	}


	n_paint_grabber_init();

	n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );

	 x = 0;
	 y = 0;
	sx = N_BMP_SX( p->pen_bmp_data );
	sy = N_BMP_SY( p->pen_bmp_data );

	n_paint_grabber_system_set( &x,&y, &sx,&sy, NULL,NULL );


	n_paint_grabber_select( NULL, FALSE );


	n_paint->grabber_mode = N_PAINT_GRABBER_DRAG_OK;


	[n_paint_global n_paint_layer_window_onoff:FALSE];


	[n_paint_global.delegate NonnonPaintStatus];


	extern void n_paint_grabber_frame_reset( void );
	n_paint_grabber_frame_reset();


	[n_paint_global display];


	return;
}

void
n_paint_grabber_copy( void )
{

	n_type_gfx x,y,fx,fy; n_paint_grabber_system_get( &x,&y, NULL,NULL, &fx,&fy );

	n_bmp_flush( n_paint->pen_bmp_grab, n_bmp_white_invisible );
	n_paint_grabber_select( NULL, FALSE );

	fx = x;
	fy = y;

	n_paint_grabber_system_set( &x,&y, NULL,NULL, &fx,&fy );
	[n_paint_global.delegate NonnonPaintStatus];

	n_paint_grabber_resync_auto();


	return;
}

void
n_paint_grabber_paste( void )
{

	// [Needed] : n_bmp_flush() for out-of-bound

	n_paint_grabber_composition( n_paint->pen_bmp_grab, n_paint->pen_bmp_data, TRUE, FALSE );
	n_bmp_flush( n_paint->pen_bmp_grab, n_bmp_white_invisible );

	n_paint_grabber_select( NULL, FALSE );

	n_paint_grabber_resync_auto();


	return;
}

#define N_PAINT_GRABBER_WHOLEGRAB_STRETCH_BACKUP   ( 1 )
#define N_PAINT_GRABBER_WHOLEGRAB_STRETCH_RESAMPLE ( 2 )
#define N_PAINT_GRABBER_WHOLEGRAB_STRETCH_RESET    ( 3 )
#define N_PAINT_GRABBER_WHOLEGRAB_STRETCH_FREE     ( 4 )

void
n_paint_grabber_wholegrb_stretch( int mode, n_type_real ratio_x, n_type_real ratio_y )
{

	n_paint_struct *p = n_paint;

	if ( mode == N_PAINT_GRABBER_WHOLEGRAB_STRETCH_BACKUP )
	{

		BOOL prv = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;

		n_type_int i = 0;
		n_posix_loop
		{

			NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_bmp_carboncopy( &p->layer_data[ i ].bmp_grab, &p->grabber_stretch_bmp[ i ] );

			}];
			[p->queue addOperation:o];

			i++;
			if ( i >= p->layer_count ) { break; }
		}

		[p->queue waitUntilAllOperationsAreFinished];
		n_bmp_is_multithread = prv;

	} else
	if ( mode == N_PAINT_GRABBER_WHOLEGRAB_STRETCH_RESAMPLE )
	{

		BOOL prv = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;

		n_type_int i = 0;
		n_posix_loop
		{

			NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			// [!] : see grabber_select()
			//if ( n_paint_layer_data[ i ].visible )
			{
				n_bmp b; n_bmp_carboncopy( &p->grabber_stretch_bmp[ i ], &b );

				n_bmp_resampler( &b, ratio_x,ratio_y );

//n_paint_grabber_select( &b, n_true );

				n_bmp_free_fast( &p->layer_data[ i ].bmp_grab );
				n_bmp_alias( &b, &p->layer_data[ i ].bmp_grab );
			}

			}];
			[p->queue addOperation:o];

			i++;
			if ( i >= p->layer_count ) { break; }
		}

		[p->queue waitUntilAllOperationsAreFinished];
		n_bmp_is_multithread = prv;

	} else
	if ( mode == N_PAINT_GRABBER_WHOLEGRAB_STRETCH_RESET )
	{

		BOOL prv = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;

		n_type_int i = 0;
		n_posix_loop
		{

			NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			// [!] : see grabber_select()
			//if ( n_paint_layer_data[ i ].visible )
			{
				n_bmp_free( &p->layer_data[ i ].bmp_grab );
				n_bmp_carboncopy( &p->grabber_stretch_bmp[ i ], &p->layer_data[ i ].bmp_grab );

				//sx = N_BMP_SX( &n_paint_layer_data[ i ].bmp_grab );
				//sy = N_BMP_SY( &n_paint_layer_data[ i ].bmp_grab );
			}

			}];
			[p->queue addOperation:o];

			i++;
			if ( i >= p->layer_count ) { break; }
		}

		[p->queue waitUntilAllOperationsAreFinished];
		n_bmp_is_multithread = prv;

	} else
	if ( mode == N_PAINT_GRABBER_WHOLEGRAB_STRETCH_FREE )
	{

		BOOL prv = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;

		n_type_int i = 0;
		n_posix_loop
		{

			NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_bmp_free( &p->grabber_stretch_bmp[ i ] );

			}];
			[p->queue addOperation:o];

			i++;
			if ( i >= p->layer_count ) { break; }
		}

		[p->queue waitUntilAllOperationsAreFinished];
		n_bmp_is_multithread = prv;

	}


	return;
}



void
NonnonPaintGrabber_mouseDown( NSEvent *theEvent )
{

	if ( n_paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { return; }


	if ( n_paint->layer_onoff )
	{
		if ( n_paint_layer_is_locked( n_paint->layer_index ) )
		{
			return;
		}
	}


	if ( [theEvent clickCount] == 2 )
	{

		if ( n_paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
//n_win_text_set_literal( hwnd, "Grabber : NEUTRAL : WM_LBUTTONDBLCLK" );

			n_paint_grabber_select_all();

		} else

		if ( n_paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
		{
//n_win_text_set_literal( hwnd, "Grabber : DRAG_OK : WM_LBUTTONDBLCLK" );

			n_paint_grabber_position_reset();

		}

	} else
	if ( [theEvent clickCount] == 1 )
	{

		if ( n_paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{

			// [x] : for bad mice like wireless mice

			n_paint->grabber_delay_onoff = TRUE;
			n_paint->grabber_delay_timer = n_posix_tickcount();


			n_type_gfx x,y; n_paint_grabber_system_get( &x,&y, NULL,NULL, NULL,NULL );

			{

				const n_type_gfx maxsx = N_BMP_SX( n_paint->pen_bmp_data ) - 1;
				const n_type_gfx maxsy = N_BMP_SY( n_paint->pen_bmp_data ) - 1;

				NSPoint pt = [n_paint_global n_paint_point_on_bitmap];
				x = pt.x;
				y = pt.y;

				if ( x < 0 ) { x = 0; } else if ( x > maxsx ) { x = maxsx; }
				if ( y < 0 ) { y = 0; } else if ( y > maxsy ) { y = maxsy; }

			}


			n_paint->grabber_delayed_x = x;
			n_paint->grabber_delayed_y = y;

		} else

		if ( n_paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
		{

			// Phase 1 : starting position and size

			n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );


			// Phase 2 : switching to DnD mode

			n_type_gfx canvas_x,canvas_y;

			NSPoint pt = [n_paint_global n_paint_point_on_bitmap];
			canvas_x = pt.x;
			canvas_y = pt.y;

			if (
				( canvas_x < x )||( canvas_x >= ( x + sx ) )
				||
				( canvas_y < y )||( canvas_y >= ( y + sy ) )
			)
			{
				return;
			}

//n_win_text_set_literal( hwnd, "Selected" );

//NSLog( @"n_paint_frame_anim_lock : N_PAINT_GRABBER_DRAG_OK" );
			n_paint->grabber_frame_lock = TRUE;


			if ( FALSE == ( theEvent.modifierFlags & NSEventModifierFlagCommand ) )
			{

				n_paint->grabber_mode = N_PAINT_GRABBER_DRAGGING;

				n_paint->grabber_drag_ox = canvas_x - x;
				n_paint->grabber_drag_oy = canvas_y - y;

			} else {

				if ( theEvent.modifierFlags & NSEventModifierFlagOption )
				{
					n_paint->grabber_mode = N_PAINT_GRABBER_STRETCH_TRANSFORM;
				} else {
					n_paint->grabber_mode = N_PAINT_GRABBER_STRETCH_PROPORTIONAL;
					n_paint->grabber_stretch_ratio = (n_type_real) sx / sy;
				}


				// [!] : Backup

				if ( n_paint->layer_whole_grab_onoff )
				{
					n_paint->grabber_stretch_bmp = n_memory_new( sizeof( n_bmp ) * n_paint->layer_count );
					n_paint_grabber_wholegrb_stretch( N_PAINT_GRABBER_WHOLEGRAB_STRETCH_BACKUP, 0,0 );
				} else
				if ( n_paint->layer_onoff )
				{
					n_paint->grabber_stretch_bmp = n_memory_new( sizeof( n_bmp ) );
					n_bmp_carboncopy( n_paint->pen_bmp_grab, n_paint->grabber_stretch_bmp );
				} else {
					n_paint->grabber_stretch_bmp = n_memory_new( sizeof( n_bmp ) );
					n_bmp_carboncopy( n_paint->pen_bmp_grab, n_paint->grabber_stretch_bmp );
				}

				n_paint->grabber_stretch_px = x;
				n_paint->grabber_stretch_py = y;


				// [!] : Init

				x += ( sx / 2 );
				y += ( sy / 2 );

				n_paint->grabber_stretch_sx = sx;
				n_paint->grabber_stretch_sy = sy;
				n_paint->grabber_stretch_cx =  x;
				n_paint->grabber_stretch_cy =  y;

//NSLog( @"%d", ( x + sx ) - canvas_x );
				n_paint->grabber_stretch_ox_l = n_paint->grabber_stretch_px - canvas_x;
				n_paint->grabber_stretch_oy_u = n_paint->grabber_stretch_py - canvas_y;
				n_paint->grabber_stretch_ox_r = ( n_paint->grabber_stretch_px + sx ) - canvas_x;
				n_paint->grabber_stretch_oy_d = ( n_paint->grabber_stretch_py + sy ) - canvas_y;

				n_paint->grabber_stretch_start_x = canvas_x;
				n_paint->grabber_stretch_start_y = canvas_y;

//NSLog( @"%d", paint->grabber_mode );
				extern void NonnonPaintGrabber_mouseDragged( void );
				NonnonPaintGrabber_mouseDragged();

			}

		}

	}


	return;
}

void
NonnonPaintGrabber_mouseDragged( void )
{

	if ( n_paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { return; }


	if ( n_paint->grabber_delay_onoff )
	{

		if ( FALSE == n_bmp_ui_timer( &n_paint->grabber_delay_timer, 100 ) ) { return; }


		n_paint->grabber_delay_onoff = FALSE;


		n_type_gfx x = n_paint->grabber_delayed_x;
		n_type_gfx y = n_paint->grabber_delayed_y;
		n_type_gfx s = 0;

		n_paint->grabber_drag_fx = x;
		n_paint->grabber_drag_fy = y;

		n_paint_grabber_system_set( &x,&y, &s,&s, &x,&y );


		n_paint->grabber_resync_rect = NSMakeRect( 0,0,0,0 );


		n_paint->grabber_mode = N_PAINT_GRABBER_SELECTING;


		[n_paint_global n_paint_layer_window_onoff:FALSE];

	}


	if ( n_paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
	{

	} else

	if ( n_paint->grabber_mode == N_PAINT_GRABBER_SELECTING )
	{

		// Phase 1 : init

		n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );

		{

			const n_type_gfx maxsx = N_BMP_SX( n_paint->pen_bmp_data ) - 1;
			const n_type_gfx maxsy = N_BMP_SY( n_paint->pen_bmp_data ) - 1;

			NSPoint pt = [n_paint_global n_paint_point_on_bitmap];
			x = pt.x;
			y = pt.y;

			if ( x < 0 ) { x = 0; } else if ( x > maxsx ) { x = maxsx; }
			if ( y < 0 ) { y = 0; } else if ( y > maxsy ) { y = maxsy; }

		}


		// Phase 2 : size : +1 for including outer

		sx = abs( n_paint->grabber_drag_fx - x ) + 1;
		sy = abs( n_paint->grabber_drag_fy - y ) + 1;


		// Phase 3 : position : top-left

		fx = x = n_posix_min_n_type_gfx( n_paint->grabber_drag_fx, x );
		fy = y = n_posix_min_n_type_gfx( n_paint->grabber_drag_fy, y );


		n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


		// [!] : selection is delayed : see WM_LBUTTONUP below
		//
		//	n_paint_grabber_resync_auto() adds a frame automatically

		if ( n_paint->grabber_is_resel )
		{
			n_paint_grabber_select( NULL, TRUE );
		} else {
			n_paint_grabber_resync_auto();
		}

	} else

	if ( n_paint->grabber_mode == N_PAINT_GRABBER_DRAGGING )
	{
//n_win_text_set_literal( hwnd, "Grabber : DRAGGING : WM_MOUSEMOVE" );


		n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );
		//n_paint_grabber_resync_auto();

		NSPoint pt = [n_paint_global n_paint_point_on_bitmap];
		x = pt.x;
		y = pt.y;

		x -= n_paint->grabber_drag_ox;
		y -= n_paint->grabber_drag_oy;

		n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );
		n_paint_grabber_resync_auto();

//[n_paint_global display_optimized];

	} else

	if (
		( n_paint->grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
		||
		( n_paint->grabber_mode == N_PAINT_GRABBER_STRETCH_TRANSFORM )
	)
	{

		// Phase 1 : init

		n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, NULL,NULL, NULL,NULL );
		//n_paint_grabber_resync_auto();


		// [!] : don't clip for usability

		NSPoint pt = [n_paint_global n_paint_point_on_bitmap];

		x = pt.x;
		y = pt.y;


		if ( ( n_paint->grabber_stretch_start_x > n_paint->grabber_stretch_cx )&&( n_paint->grabber_stretch_start_y > n_paint->grabber_stretch_cy ) )
		{
			// [!] : Bottom Right
			x += n_paint->grabber_stretch_ox_r;
			y += n_paint->grabber_stretch_oy_d;
		} else
		if ( ( n_paint->grabber_stretch_start_x < n_paint->grabber_stretch_cx )&&( n_paint->grabber_stretch_start_y < n_paint->grabber_stretch_cy ) )
		{
			// [!] : Top Left
			x += n_paint->grabber_stretch_ox_l;
			y += n_paint->grabber_stretch_oy_u;
		} else
		if ( ( n_paint->grabber_stretch_start_x < n_paint->grabber_stretch_cx )&&( n_paint->grabber_stretch_start_y > n_paint->grabber_stretch_cy ) )
		{
			// [!] : Bottom Left
			x += n_paint->grabber_stretch_ox_l;
			y += n_paint->grabber_stretch_oy_d;
		}// else
		if ( ( n_paint->grabber_stretch_start_x > n_paint->grabber_stretch_cx )&&( n_paint->grabber_stretch_start_y < n_paint->grabber_stretch_cy ) )
		{
			// [!] : Top Right
			x += n_paint->grabber_stretch_ox_r;
			y += n_paint->grabber_stretch_oy_u;
		}


		// Phase 2 : size

		if ( n_paint->grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
		{
			sy = abs( n_paint->grabber_stretch_cy - y ) * 2;
			sx = (n_type_gfx) ( (n_type_real) sy * n_paint->grabber_stretch_ratio );
		} else {
			sx = abs( n_paint->grabber_stretch_cx - x ) * 2;
			sy = abs( n_paint->grabber_stretch_cy - y ) * 2;
		}

		if ( sx == 0 ) { sx = 1; }
		if ( sy == 0 ) { sy = 1; }

//n_win_hwndprintf_literal( hwnd, "%d : %d", (int) sx, (int) sy );


		// Phase 3 : position

		x = n_paint->grabber_stretch_cx - ( sx / 2 );
		y = n_paint->grabber_stretch_cy - ( sy / 2 );


		{ // n_paint_grabber_transform()

		n_type_real ratio_x = (n_type_real) sx / n_paint->grabber_stretch_sx;
		n_type_real ratio_y = (n_type_real) sy / n_paint->grabber_stretch_sy;

		if ( ( ratio_x * n_paint->grabber_stretch_sx ) == 0 ) { return; }
		if ( ( ratio_y * n_paint->grabber_stretch_sy ) == 0 ) { return; }

		if ( n_paint->layer_whole_grab_onoff )
		{

			n_paint_grabber_wholegrb_stretch( N_PAINT_GRABBER_WHOLEGRAB_STRETCH_RESAMPLE, ratio_x, ratio_y );

			sx = 0;
			sy = 0;

			int i = 0;
			n_posix_loop
			{

				sx = n_posix_max_n_type_gfx( sx, N_BMP_SX( &n_paint->layer_data[ i ].bmp_grab ) );
				sy = n_posix_max_n_type_gfx( sy, N_BMP_SY( &n_paint->layer_data[ i ].bmp_grab ) );

				i++;
				if ( i >= n_paint->layer_count ) { break; }
			}

			n_paint_grabber_system_set( &x,&y, &sx,&sy, NULL,NULL );

		} else
		if ( n_paint->layer_onoff )
		{

			n_paint_grabber_system_set( &x,&y, &sx,&sy, NULL,NULL );

			n_bmp b; n_bmp_carboncopy( n_paint->grabber_stretch_bmp, &b );

			n_bmp_resampler( &b, ratio_x,ratio_y );
			n_paint_grabber_select( &b, FALSE );

			n_bmp_free_fast( &b );

		} else {

			n_paint_grabber_system_set( &x,&y, &sx,&sy, NULL,NULL );

			n_bmp b; n_bmp_carboncopy( n_paint->grabber_stretch_bmp, &b );

			n_bmp_resampler( &b, ratio_x,ratio_y );
			n_paint_grabber_select( &b, FALSE );

			n_bmp_free_fast( &b );

		}

		} // n_paint_grabber_transform()

		n_paint_grabber_resync_auto();

	}// else


	[n_paint_global.delegate NonnonPaintStatus];

}

void
NonnonPaintGrabber_mouseUp( void )
{

	if ( n_paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { return; }


	n_paint->grabber_delay_onoff = FALSE;


	if ( n_paint->grabber_mode == N_PAINT_GRABBER_SELECTING )
	{
//n_win_text_set_literal( hwnd, "Grabber : SELECTING : WM_LBUTTONUP" );


		n_paint->grabber_is_resel = FALSE;


		n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );

//n_win_hwndprintf_literal( hwnd, "%d : %d", (int) sx, (int) sy );


		if ( ( sx <= 0 )&&( sy <= 0 ) )
		{

			n_paint->grabber_mode = N_PAINT_GRABBER_NEUTRAL;

			return;
		}


		// position : use the last position
		// size     : use the last size


		// [!] : delayed
		n_paint_grabber_select( NULL, TRUE );


		n_paint->grabber_mode = N_PAINT_GRABBER_DRAG_OK;

		if ( n_paint->layer_onoff )
		{
			[n_paint->whole_preview_checkbox setEnabled:NO];
			[n_paint->whole_grab_checkbox    setEnabled:NO];
		}

	} else

	if ( n_paint->grabber_mode == N_PAINT_GRABBER_DRAGGING )
	{
//n_win_text_set_literal( hwnd, "Grabber : DRAGGING : WM_LBUTTONUP" );

		n_paint->grabber_mode = N_PAINT_GRABBER_DRAG_OK;

		[n_paint_global.delegate NonnonPaintStatus];

//NSLog( @"%d", n_mac_window_is_hovered( n_paint_global ) );
		if ( FALSE == n_mac_window_is_hovered( n_paint_global ) )
		{
			//NSString *desktop = [NSHomeDirectory()stringByAppendingPathComponent:@"Desktop"];

			NSArray      *paths   = NSSearchPathForDirectoriesInDomains( NSDesktopDirectory, NSUserDomainMask, YES );
			NSString     *desktop = [paths objectAtIndex:0];
			n_posix_char  tmpname[ 100 ]; n_string_path_tmpname( tmpname );
			NSString     *name    = [NSString stringWithFormat:@"%@/%s.png", desktop, tmpname];
//NSLog( @"%@", name );

			BOOL n_bmp_carboncopy_onoff = TRUE;

			if ( n_paint->layer_whole_save_onoff )
			{
				n_bmp_carboncopy_onoff = FALSE;
			}

			n_posix_char *str = n_mac_nsstring2str( name );

			extern BOOL n_paint_save_png( n_bmp *bmp, n_posix_char *path, BOOL n_bmp_carboncopy_onoff );
			BOOL ret = n_paint_save_png( n_paint->pen_bmp_grab, str, n_bmp_carboncopy_onoff );
			if ( ret )
			{
//NSLog( @"1" );
				NSString *name2 = [name lastPathComponent];

				NSSavePanel *panel = [NSSavePanel savePanel];
				[panel setNameFieldStringValue:name2];
				//panel.canCreateDirectories = YES;
				NSModalResponse result = [panel runModal];

				// [x] : return immediately
				// [x] : UI confuses
				//[panel beginSheetModalForWindow:paint->nswindow[ 0 ] completionHandler:^(NSInteger result)
				{
					if ( result == NSModalResponseOK )
					{
						NSString     *nsstring = [[panel URL] path];
						n_posix_char *str2     = n_mac_nsstring2str( nsstring );

						n_paint_save_png( n_paint->pen_bmp_grab, str2, n_bmp_carboncopy_onoff );

						n_string_free( str2 );
					}
				}
				//];
//NSLog( @"2" );
			}

			n_memory_free( str );

			n_paint_grabber_reset();

		}

	} else

	if (
		( n_paint->grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
		||
		( n_paint->grabber_mode == N_PAINT_GRABBER_STRETCH_TRANSFORM )
	)
	{
//n_win_text_set_literal( hwnd, "Grabber : STRETCHING : WM_LBUTTONUP" );

		n_paint->xmouse_stop = TRUE;
		BOOL ret = n_mac_window_dialog_yesno( "Really OK?" );
		n_paint->xmouse_stop = FALSE;
		
		if ( ret )
		{

			n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );

			fx = x;
			fy = y;

			n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );

		} else {

			// [!] : reselect an area before stretching

			n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );

			x = n_paint->grabber_stretch_px;
			y = n_paint->grabber_stretch_py;

			n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


			if ( n_paint->layer_whole_grab_onoff )
			{

				n_paint_grabber_wholegrb_stretch( N_PAINT_GRABBER_WHOLEGRAB_STRETCH_RESET, 0,0 );

				sx = 0;
				sy = 0;

				int i = 0;
				n_posix_loop
				{

					sx = n_posix_max_n_type_gfx( sx, N_BMP_SX( &n_paint->layer_data[ i ].bmp_grab ) );
					sy = n_posix_max_n_type_gfx( sy, N_BMP_SY( &n_paint->layer_data[ i ].bmp_grab ) );

					i++;
					if ( i >= n_paint->layer_count ) { break; }
				}

				n_paint_grabber_system_set( &x,&y, &sx,&sy, NULL,NULL );

			} else
			if ( n_paint->layer_onoff )
			{
				n_paint_grabber_select( n_paint->grabber_stretch_bmp, TRUE );
			} else {
				n_paint_grabber_select( n_paint->grabber_stretch_bmp, TRUE );
			}

		}


		if ( n_paint->layer_whole_grab_onoff )
		{
			n_paint_grabber_wholegrb_stretch( N_PAINT_GRABBER_WHOLEGRAB_STRETCH_FREE, 0,0 );
		} else
		if ( n_paint->layer_onoff )
		{
			n_bmp_free( n_paint->grabber_stretch_bmp );
		} else {
			n_bmp_free( n_paint->grabber_stretch_bmp );
		}

		n_memory_free( n_paint->grabber_stretch_bmp );


		n_paint->grabber_mode = N_PAINT_GRABBER_DRAG_OK;

		[n_paint_global.delegate NonnonPaintStatus];

	}


	n_paint_indicator();
	[n_paint_global display];


	return;
}




void
NonnonPaintGrabber_rightMouseDown( void )
{

	if ( n_paint->tooltype != N_PAINT_TOOL_TYPE_GRABBER ) { return; }


	if ( n_paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
	{
		n_paint_grabber_finish();
	}


	return;
}




void
NonnonPaintGrabber_keyDown_arrow( int keyCode )
{
//return;

	if ( n_paint->grabber_mode != N_PAINT_GRABBER_DRAG_OK ) { return; }


	n_type_gfx x,y,sx,sy,fx,fy;
	n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );

	switch( keyCode ) {

	case N_MAC_KEYCODE_ARROW_UP :

		n_paint->grabber_is_key_input |= N_PAINT_GRABBER_INPUT_U;

		y--;

	break;

	case N_MAC_KEYCODE_ARROW_DOWN :

		n_paint->grabber_is_key_input |= N_PAINT_GRABBER_INPUT_D;

		y++;

	break;


	case N_MAC_KEYCODE_ARROW_LEFT :

		n_paint->grabber_is_key_input |= N_PAINT_GRABBER_INPUT_L;

		x--;

	break;

	case N_MAC_KEYCODE_ARROW_RIGHT:

		n_paint->grabber_is_key_input |= N_PAINT_GRABBER_INPUT_R;

		x++;

	break;

	} // switch

	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


//NSLog( @"n_paint_frame_lock : NonnonPaintGrabber_keyDown_arrow" );
	n_paint->grabber_frame_lock = TRUE;


	n_paint_grabber_resync_auto();

	[n_paint_global.delegate NonnonPaintStatus];


	return;
}

void
NonnonPaintGrabber_keyUp_arrow( int keyCode )
{
//return;

	switch( keyCode ) {

	case N_MAC_KEYCODE_ARROW_UP   :

		n_paint->grabber_is_key_input &= ~N_PAINT_GRABBER_INPUT_U;

	break;

	case N_MAC_KEYCODE_ARROW_DOWN :

		n_paint->grabber_is_key_input &= ~N_PAINT_GRABBER_INPUT_D;

	break;

	case N_MAC_KEYCODE_ARROW_LEFT :

		n_paint->grabber_is_key_input &= ~N_PAINT_GRABBER_INPUT_L;

	break;

	case N_MAC_KEYCODE_ARROW_RIGHT:

		n_paint->grabber_is_key_input &= ~N_PAINT_GRABBER_INPUT_R;

	break;


	} // switch


	if ( n_paint->grabber_is_key_input == FALSE )
	{
//NSLog( @"!" );
		n_paint->grabber_is_key_input_fade = TRUE;
		n_paint->grabber_is_key_input_tick = n_posix_tickcount();
	}


	return;
}


