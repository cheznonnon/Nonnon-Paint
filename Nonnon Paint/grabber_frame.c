// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




// [!] : shared

void
n_paint_grabber_frame_reset( n_paint *paint )
{

	n_paint_grabber_frame_anim_onoff = FALSE;
	paint->grabber_frame_lock        = FALSE;


	return;
}

void
n_paint_grabber_frame_init
(
	n_paint *paint,
	n_type_gfx bitmap_sx, n_type_gfx bitmap_sy,
	n_type_gfx  frame_fx, n_type_gfx  frame_fy,
	n_type_gfx  frame_tx, n_type_gfx  frame_ty
)
{
//return;

	if ( paint->grabber_mode == 0 ) { return; }


	// [!] : zero-clear is important

	{
		s64 byte = bitmap_sx * bitmap_sy * sizeof( NSPoint );
//NSLog( @"%lld", byte );

		paint->grabber_frame_path = n_memory_new( byte );

		n_memory_zero( paint->grabber_frame_path, byte );
	}


	// [!] : clockwise

	extern n_type_gfx n_paint_grabber_frame_edge_redraw_area_get( n_paint *paint );

	n_type_gfx z = n_paint_grabber_frame_edge_redraw_area_get( paint );
	n_type_gfx h = z / 2;

	n_type_int i = 0;
	n_type_gfx x = frame_fx;
	n_type_gfx y = frame_fy;

	n_posix_loop
	{
		NSPoint p = { x, y };
		paint->grabber_frame_path[ i ] = p; i++;

		x++;
		if ( x >= frame_tx ) { break; }
	}

	{
		NSRect p = NSMakeRect( x-h, y-h, z, z );
		paint->grabber_frame_rect[ 1 ] = p;
	}

	n_posix_loop
	{
		NSPoint p = { x, y };
		paint->grabber_frame_path[ i ] = p; i++;

		y++;
		if ( y >= frame_ty ) { break; }
	}

	{
		NSRect p = NSMakeRect( x-h, y-h, z, z );
		paint->grabber_frame_rect[ 2 ] = p;
	}

	n_posix_loop
	{
		NSPoint p = { x, y };
		paint->grabber_frame_path[ i ] = p; i++;

		x--;
		if ( x <= frame_fx ) { break; }
	}

	{
		NSRect p = NSMakeRect( x-h, y-h, z, z );
		paint->grabber_frame_rect[ 3 ] = p;
	}

	n_posix_loop
	{
		NSPoint p = { x, y };
		paint->grabber_frame_path[ i ] = p; i++;

		y--;
		if ( y <= frame_fy ) { break; }
	}

	{
		NSRect p = NSMakeRect( x-h, y-h, z, z );
		paint->grabber_frame_rect[ 0 ] = p;
	}

	paint->grabber_frame_path_count = i;

//NSLog( @"%lld %d", paint->grabber_frame_path_count, bitmap_sx * bitmap_sy );


	//paint->grabber_tooltip_rect_drawn = NSMakeRect( 0,0,0,0 );


	return;
}

void
n_paint_grabber_frame_exit( n_paint *paint )
{

	if ( paint->grabber_mode == 0 ) { return; }


	n_memory_free( paint->grabber_frame_path );
	paint->grabber_frame_path = NULL;


	return;
}




// [!] : N_PAINT_GRABBER_FRAME_MODE_EDGE

n_type_gfx
n_paint_grabber_frame_edge_redraw_area_get( n_paint *paint )
{
	return 24;
}

// internal
n_type_gfx
n_paint_grabber_frame_edge_zoom_get( n_paint *paint )
{

	n_type_gfx z = 1;

	if ( n_paint_global.paint->zoom > 0 )
	{
		z = [n_paint_global n_paint_zoom_get_int:paint->zoom];
	}


	return z;
}

// internal
n_posix_bool
n_paint_grabber_frame_edge_is_hovered( n_paint *paint, int index )
{

	n_posix_bool ret = n_posix_false;


	NSPoint pt = n_mac_cursor_position_get( n_paint_global );

	n_type_gfx px = pt.x;
	n_type_gfx py = pt.y;

//NSLog( @"%d %d", px, py );

	{
		n_type_gfx  x = paint->grabber_frame_rect[ index ].origin.x;
		n_type_gfx  y = paint->grabber_frame_rect[ index ].origin.y;
		n_type_gfx sx = paint->grabber_frame_rect[ index ].size.width;
		n_type_gfx sy = paint->grabber_frame_rect[ index ].size.height;

		n_type_gfx fx = x;
		n_type_gfx fy = y;
		n_type_gfx tx = x + sx;
		n_type_gfx ty = y + sy;

		if (
			( px >= fx )&&( px <= tx )
			&&
			( py >= fy )&&( py <= ty )
		)
		{
			ret = n_posix_true;
		}

	}


	return ret;
}

int
n_paint_grabber_frame_edge_resize_dot_detect( n_paint *paint )
{

	if ( n_paint_grabber_frame_anim_onoff == n_posix_false ) { return -1; }

	//if ( paint->grabber_mode != N_PAINT_GRABBER_DRAG_OK ) { return -1; }

	if ( paint->grabber_frame_lock ) { return -1; }


	int i = 0;
	n_posix_loop
	{
		if ( n_paint_grabber_frame_edge_is_hovered( paint, i ) )
		{
			return i;
		}

		i++;
		if ( i >= 4 ) { break; }
	}


	return -1;
}

// internal
void
n_paint_grabber_frame_edge_reselect( n_paint *paint )
{

	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );
//NSLog( @"B : %d %d %d %d %d %d", x,y,sx,sy,fx,fy );

	if ( n_paint_grabber_frame_edge_is_hovered( paint, 0 ) )
	{
//NSLog( @"0" );
		paint->grabber_drag_fx = x + sx - 1;
		paint->grabber_drag_fy = y + sy - 1;
	} else 
	if ( n_paint_grabber_frame_edge_is_hovered( paint, 1 ) )
	{
//NSLog( @"1" );
		paint->grabber_drag_fx = x;
		paint->grabber_drag_fy = y + sy - 1;
	} else 
	if ( n_paint_grabber_frame_edge_is_hovered( paint, 2 ) )
	{
//NSLog( @"2" );
		paint->grabber_drag_fx = x;
		paint->grabber_drag_fy = y;
	} else 
	if ( n_paint_grabber_frame_edge_is_hovered( paint, 3 ) )
	{
//NSLog( @"3" );
		paint->grabber_drag_fx = x + sx - 1;
		paint->grabber_drag_fy = y;
	}

//NSLog( @"A : %d %d %d %d %d %d", x,y,sx,sy,fx,fy );
	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );

	paint->grabber_mode = N_PAINT_GRABBER_SELECTING;

	paint->grabber_is_resel = n_posix_true;


	return;
}

void
n_paint_grabber_frame_edge_mouseDown( n_paint *paint )
{

	if ( n_paint_grabber_frame_anim_onoff == n_posix_false ) { return; }

	if ( paint->grabber_mode != N_PAINT_GRABBER_DRAG_OK ) { return; }

	if ( paint->grabber_frame_lock ) { return; }


	int i = 0;
	n_posix_loop
	{
		if ( n_paint_grabber_frame_edge_is_hovered( paint, i ) )
		{
//NSLog( @"Clicked %d", i );
			n_paint_grabber_frame_edge_reselect( paint );

			break;
		}

		i++;
		if ( i >= 4 ) { break; }
	}


	return;
}

void
n_paint_grabber_frame_edge_draw( n_paint *paint, n_bmp *bmp )
{
//return;

	if ( paint->grabber_frame_hide ) { return; }


	if (
		( paint->grabber_mode == N_PAINT_GRABBER_DRAG_OK )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_SELECTING )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_DRAGGING )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
		||
		( paint->grabber_mode == N_PAINT_GRABBER_STRETCH_TRANSFORM )
	)
	{
		//
	} else {
		return;
	}


	n_type_gfx bitmap_sx = N_BMP_SX( bmp );
	n_type_gfx bitmap_sy = N_BMP_SY( bmp );
//NSLog( @"%d %d", bitmap_sx, bitmap_sy );

	n_type_gfx frame_fx, frame_fy, frame_tx, frame_ty;
	n_paint_grabber_system_get( &frame_fx, &frame_fy, &frame_tx, &frame_ty, NULL,NULL );

	[n_paint_global n_paint_convert_bitmap2canvas_grabber_frame:nil x:&frame_fx y:&frame_fy sx:&frame_tx sy:&frame_ty];

	frame_tx += frame_fx;
	frame_ty += frame_fy;

	n_paint_grabber_frame_init( paint, bitmap_sx, bitmap_sy, frame_fx, frame_fy, frame_tx, frame_ty );


	// [!] : Marching Ants Effect

	n_type_int i = 0;

	n_type_gfx px = paint->grabber_frame_path[ i ].x;
	n_type_gfx py = paint->grabber_frame_path[ i ].y;

//NSLog( @"%d", n_paint_global.paint->zoom );

	i++;


	n_type_gfx step_full = 5;
	n_type_gfx step_half = step_full / 2;

	n_type_int n = paint->grabber_frame_step;

	n_posix_loop
	{//break;

		n_type_gfx x = paint->grabber_frame_path[ i ].x;
		n_type_gfx y = paint->grabber_frame_path[ i ].y;

		u32 c;
		if ( ( ( i + n ) % step_full ) <= step_half ) { c = n_bmp_black; } else { c = n_bmp_white; }

		const n_type_gfx size = 2;
		if ( px == x )
		{
			// [!] : Left / Right
			n_bmp_box( bmp, x,y,size,step_half, c );
		} else {
			// [!] : Top / Bottom
			n_bmp_box( bmp, x,y,step_half,size, c );
		}

		px = x;
		py = y;

		i++;
		if ( i >= paint->grabber_frame_path_count ) { break; }
	}


	n_paint_grabber_frame_exit( paint );


	return;
}

void
n_paint_grabber_frame_edge_resize_dot_draw( n_paint *paint, n_bmp *bmp )
{
//return;

	if ( paint->grabber_mode == 0 ) { return; }


	n_type_gfx border = (CGFloat) paint->grabber_frame_rect[ 0 ].size.width * 0.1;


	const u32 color_border = n_bmp_rgb_mac( 111,111,111 );
	const u32 color_accent = n_bmp_color_mac( n_mac_nscolor2argb( [NSColor controlAccentColor] ) );

	n_type_int i = 0;
	n_posix_loop
	{//break;
		n_type_gfx  x = paint->grabber_frame_rect[ i ].origin.x;
		n_type_gfx  y = paint->grabber_frame_rect[ i ].origin.y;
		n_type_gfx sx = paint->grabber_frame_rect[ i ].size.width;
		n_type_gfx sy = paint->grabber_frame_rect[ i ].size.height;

		u32 color;
		if ( paint->tooltype == N_PAINT_TOOL_TYPE_GRABBER )
		{
			if ( paint->grabber_frame_lock )
			{
				color = color_border;
			} else {
				color = color_accent;
			}
		} else {
			color = color_border;
		}


		n_bmp_circle( bmp, x, y, sx, sy, color );
		n_bmp_hoop  ( bmp, x, y, sx, sy, border, n_bmp_white );


		i++;
		if ( i >= 4 ) { break; }
	}


	return;
}




// [!] : N_PAINT_GRABBER_FRAME_MODE_PIXEL

n_posix_bool
n_paint_grabber_frame_pixel_onoff( n_type_gfx zx, n_type_gfx zy, n_type_gfx gx, n_type_gfx gy, n_type_gfx gsx, n_type_gfx gsy )
{

	n_paint *p = n_paint_global.paint;


	n_posix_bool ret = n_posix_false;


	if ( p->zoom < 0 )
	{
		n_type_gfx zoom = p->zoom * -1;

		gx  /= zoom;
		gy  /= zoom;
		gsx /= zoom;
		gsy /= zoom;
	}


	if ( p->grabber_frame_hide ) { return ret; }


	if ( p->grabber_mode )
	{
/*
if ( ( zx >= gx )&&( zy >= gy )&&( zx <= ( gx + gsx - 1 ) )&&( zy <= ( gy + gsy - 1 ) ) )
{
	ret = n_posix_true;
}
*/
		// Top and Bottom

		if ( ( zx >= ( gx - 1 ) )&&( zx <= ( gx + gsx ) ) )
		{
			if ( ( zy == ( gy - 1 ) )||( zy == ( gy + gsy ) ) )
			{
				ret = n_posix_true;
			}
		}

		// Left and Right

		if ( ( zy >= ( gy - 1 ) )&&( zy <= ( gy + gsy ) ) )
		{
			if ( ( zx == ( gx - 1 ) )||( zx == ( gx + gsx ) ) )
			{
				ret = n_posix_true;
			}
		}

	}


	return ret;
}

BOOL
n_paint_grabber_frame_pixel_detect( n_type_gfx n )
{

	BOOL ret;


	int d = n % 2;

	if ( d )
	{
		ret = TRUE;
	} else {
		ret = FALSE;
	}


	return ret;
}

n_type_gfx
n_paint_grabber_frame_pixel_redraw_area_get( n_paint *paint )
{

	n_type_gfx ret;

	if ( paint->zoom > 0 )
	{
		ret = n_posix_max_n_type_gfx( 18, paint->zoom * 4 );
	} else {
		ret = 12;
	}


	return ret;
}

// internal
BOOL
n_paint_grabber_frame_pixel_resize_dot_is_hit_kernel( n_paint *paint, n_type_int i, n_type_gfx x, n_type_gfx y )
{

	n_type_gfx fx =      paint->grabber_frame_rect[ i ].origin.x;
	n_type_gfx fy =      paint->grabber_frame_rect[ i ].origin.y;
	n_type_gfx tx = fx + paint->grabber_frame_rect[ i ].size.width;
	n_type_gfx ty = fy + paint->grabber_frame_rect[ i ].size.height;

	if (
		( ( fx < x )&&( x < tx ) )
		&&
		( ( fy < y )&&( y < ty ) )
	)
	{
		return TRUE;
	}


	return FALSE;
}

// internal
void
n_paint_grabber_frame_pixel_resize_dot_is_hit( n_paint *paint, BOOL *top_left, BOOL *top_right, BOOL *bottom_left, BOOL *bottom_right )
{

	if ( paint->grabber_frame_lock )
	{
		*top_left = *top_right = *bottom_left = *bottom_right = FALSE;
		return;
	}


	NSPoint pt = n_mac_cursor_position_get( n_paint_global );

	n_type_gfx px = pt.x;
	n_type_gfx py = pt.y;


	*top_left     = n_paint_grabber_frame_pixel_resize_dot_is_hit_kernel( paint, 0, px, py );
	*top_right    = n_paint_grabber_frame_pixel_resize_dot_is_hit_kernel( paint, 1, px, py );
	*bottom_left  = n_paint_grabber_frame_pixel_resize_dot_is_hit_kernel( paint, 2, px, py );
	*bottom_right = n_paint_grabber_frame_pixel_resize_dot_is_hit_kernel( paint, 3, px, py );


	return;
}

// internal
void
n_paint_grabber_frame_pixel_reselect( n_paint *paint )
{

	BOOL top_left, top_right, bottom_left, bottom_right;
	n_paint_grabber_frame_pixel_resize_dot_is_hit( paint, &top_left, &top_right, &bottom_left, &bottom_right );


	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );

	if ( top_left )
	{
		paint->grabber_drag_fx = x + sx - 1;
		paint->grabber_drag_fy = y + sy - 1;
	} else 
	if ( bottom_left )
	{
		paint->grabber_drag_fx = x + sx - 1;
		paint->grabber_drag_fy = y;
	} else 
	if ( bottom_right )
	{
		paint->grabber_drag_fx = x;
		paint->grabber_drag_fy = y;
	} else 
	if ( top_right )
	{
		paint->grabber_drag_fx = x;
		paint->grabber_drag_fy = y + sy - 1;
	}

//NSLog( @"A : %d %d %d %d %d %d", x,y,sx,sy,fx,fy );
	n_paint_grabber_system_set( &x,&y, &sx,&sy, &fx,&fy );


	if ( ( top_left )||( top_right )||( bottom_left )||( bottom_right ) )
	{
		paint->grabber_mode     = N_PAINT_GRABBER_SELECTING;
		paint->grabber_is_resel = n_posix_true;
	}


	return;
}

void
n_paint_grabber_frame_pixel_mouseDown( n_paint *paint )
{

	if ( paint->grabber_mode != N_PAINT_GRABBER_DRAG_OK ) { return; }


	if ( paint->grabber_frame_lock ) { return; }


	n_paint_grabber_frame_pixel_reselect( paint );


	return;
}

void
n_paint_grabber_frame_pixel_draw( n_paint *paint, n_bmp *bmp, int zoom_unit )
{

	if ( paint->grabber_frame_hide ) { return; }

	if ( paint->grabber_mode == 0 ) { return; }


	n_type_gfx bitmap_sx = N_BMP_SX( bmp );
	n_type_gfx bitmap_sy = N_BMP_SY( bmp );

	n_type_gfx frame_fx, frame_fy, frame_tx, frame_ty;
	n_paint_grabber_system_get( &frame_fx, &frame_fy, &frame_tx, &frame_ty, NULL,NULL );

	frame_fx--; frame_tx++;
	frame_fy--; frame_ty++;

	[n_paint_global n_paint_convert_bitmap2canvas_grabber_frame:nil x:&frame_fx y:&frame_fy sx:&frame_tx sy:&frame_ty];

	frame_tx += frame_fx;
	frame_ty += frame_fy;

	n_paint_grabber_frame_init( paint, bitmap_sx, bitmap_sy, frame_fx, frame_fy, frame_tx, frame_ty );


	// [!] : Marching Ants Effect

	if ( zoom_unit == 1 ) { zoom_unit = 2; }

	n_type_gfx step_full = zoom_unit * 2;
	n_type_gfx step_half = step_full / 3;

	n_type_int n = paint->grabber_frame_step;

	n_type_int i = 0;
	n_posix_loop
	{//break;

		n_type_gfx x = paint->grabber_frame_path[ i ].x;
		n_type_gfx y = paint->grabber_frame_path[ i ].y;

		u32 c;
		if ( ( ( i + n ) % step_full ) <= step_half ) { c = n_bmp_white; } else { c = n_bmp_black; }

		n_bmp_box( bmp, x,y,zoom_unit,zoom_unit, c );

		i++;
		if ( i >= paint->grabber_frame_path_count ) { break; }
	}


	n_paint_grabber_frame_exit( paint );


	return;
}

void
n_paint_grabber_frame_pixel_resize_dot_draw( n_paint *paint, n_bmp *bmp, n_type_gfx size )
{

	if ( paint->grabber_frame_hide ) { return; }


	if ( paint->grabber_mode == 0 ) { return; }


	n_type_gfx z = size;
	n_type_gfx h = z / 2;

	n_type_gfx x,y,sx,sy,fx,fy; n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );
//NSLog( @"%d %d %d %d", x, y, sx, sy );

	[n_paint_global n_paint_convert_bitmap2canvas_grabber_frame:nil x:&x y:&y sx:&sx sy:&sy];
//NSLog( @"%d %d %d %d", x, y, sx, sy );


	const u32 color_border = n_bmp_rgb_mac( 111,111,111 );
	const u32 color_accent = n_bmp_color_mac( n_mac_nscolor2argb( [NSColor controlAccentColor] ) );

	u32 color;
	if ( paint->tooltype == N_PAINT_TOOL_TYPE_GRABBER )
	{
		if ( paint->grabber_frame_lock )
		{
			color = color_border;
		} else {
			color = color_accent;
		}
	} else {
		color = color_border;
	}


	if ( paint->grabber_frame_lock )
	{
		n_bmp_circle( bmp, x-h   , y-h   , z, z, color );
		n_bmp_circle( bmp, x-h+sx, y-h   , z, z, color );
		n_bmp_circle( bmp, x-h   , y-h+sy, z, z, color );
		n_bmp_circle( bmp, x-h+sx, y-h+sy, z, z, color );
	} else {
/*
		n_bmp_box( bmp, x-h   , y-h   , z, z, color );
		n_bmp_box( bmp, x-h+sx, y-h   , z, z, color );
		n_bmp_box( bmp, x-h   , y-h+sy, z, z, color );
		n_bmp_box( bmp, x-h+sx, y-h+sy, z, z, color );
*/
		n_bmp_circle( bmp, x-h   , y-h   , z, z, color );
		n_bmp_circle( bmp, x-h+sx, y-h   , z, z, color );
		n_bmp_circle( bmp, x-h   , y-h+sy, z, z, color );
		n_bmp_circle( bmp, x-h+sx, y-h+sy, z, z, color );

		n_type_gfx border = n_posix_max( 2, z / 8 );

		n_bmp_hoop( bmp, x-h   , y-h   , z, z, border, n_bmp_white );
		n_bmp_hoop( bmp, x-h+sx, y-h   , z, z, border, n_bmp_white );
		n_bmp_hoop( bmp, x-h   , y-h+sy, z, z, border, n_bmp_white );
		n_bmp_hoop( bmp, x-h+sx, y-h+sy, z, z, border, n_bmp_white );

		paint->grabber_frame_rect[ 0 ] = NSMakeRect( x-h   , y-h   , z, z );
		paint->grabber_frame_rect[ 1 ] = NSMakeRect( x-h+sx, y-h   , z, z );
		paint->grabber_frame_rect[ 2 ] = NSMakeRect( x-h   , y-h+sy, z, z );
		paint->grabber_frame_rect[ 3 ] = NSMakeRect( x-h+sx, y-h+sy, z, z );
	}


	return;
}


