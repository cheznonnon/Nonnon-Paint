// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




void
n_paint_bmp_carboncopy( n_bmp *f, n_bmp *t )
{
//return;

	n_bmp_free( t );
	n_bmp_carboncopy( f, t );


	return;
}




u32
n_paint_bmp_checker_pixel( n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 color )
{

	// [!] : ( n & 31 ) == ( n % 32 )


	n_type_gfx mod_sx = ( sx & 31 ) / 2;
	n_type_gfx mod_sy = ( sy & 31 ) / 2;

	x -= mod_sx;
	y -= mod_sy;


	n_type_gfx mod_x = ( x & 31 );
	n_type_gfx mod_y = ( y & 31 );

	if (
		( ( 16 >  mod_x )&&( 16 >  mod_y ) )
		||
		( ( 16 <= mod_x )&&( 16 <= mod_y ) )
	)
	{
		n_type_real ratio = 0.75;//n_bmp_blend_alpha2ratio( color );
		color = n_bmp_blend_pixel( n_bmp_white, color, ratio );
	}


	return color;
}




void
n_paint_bmp_flush_reverse( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		int a =       n_bmp_a( color );
		int r = 255 - n_bmp_r( color );
		int g = 255 - n_bmp_g( color );
		int b = 255 - n_bmp_b( color );

		n_bmp_ptr_set_fast( bmp, x,y, n_bmp_argb( a,r,g,b ) );

		x++;
		if ( x >= sx )
		{
			x = 0;
			y++;
			if ( y >= sy ) { break; }
		}
	}


	return;
}




void
n_paint_bmp_flush_alpha_base_color( n_bmp *bmp, u32 base_color )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		int a = n_bmp_a(      color );
		int r = n_bmp_r( base_color );
		int g = n_bmp_g( base_color );
		int b = n_bmp_b( base_color );

		n_bmp_ptr_set_fast( bmp, x,y, n_bmp_argb( a,r,g,b ) );

		x++;
		if ( x >= sx )
		{
			x = 0;
			y++;
			if ( y >= sy ) { break; }
		}
	}


	return;
}




void
n_paint_bmp_thicken( n_bmp *bmp, u32 color_target )
{

	n_bmp_flush_antialias( bmp, 1.0 );


	const n_type_real ratio = 1.5;

	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		int a = n_bmp_a( color );
		if ( a != 0 )
		{
			u32 c = n_bmp_blend_pixel( n_bmp_white_invisible, color_target, (n_type_real) a * ratio * n_bmp_coeff_channel );
			n_bmp_ptr_set_fast( bmp, x,y, c );
		}


		x++;
		if ( x >= sx )
		{
			x = 0;
			y++;
			if ( y >= sy ) { break; }
		}
	}


	return;
}

void
n_paint_bmp_thin( n_bmp *bmp, u32 color_target )
{

	n_bmp_flush_antialias( bmp, 1.0 );
	n_bmp_flush_antialias( bmp, 1.0 );


	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		if ( color != n_bmp_white_invisible )
		{
			n_type_real a = n_bmp_a( color ) * n_bmp_coeff_channel;

			//n_type_real d = pow( a, 2 );
			n_type_real d = sin( 2 * M_PI * pow( a, 2 ) / 4 );

			color = n_bmp_blend_pixel( n_bmp_white_invisible, color_target, d );
			n_bmp_ptr_set_fast( bmp, x,y, color );
		}


		x++;
		if ( x >= sx )
		{
			x = 0;
			y++;
			if ( y >= sy ) { break; }
		}
	}


	return;
}




void
n_paint_bmp_icon_squircle( n_bmp *bmp_ret )
{

	u32 color_bg = n_bmp_argb( 0,128,128,128 );


	n_type_gfx bmpsx = N_BMP_SX( bmp_ret );
	n_type_gfx bmpsy = N_BMP_SY( bmp_ret );


	n_type_gfx sx = bmpsx - ( bmpsx / 5 );
	n_type_gfx sy = bmpsy - ( bmpsy / 5 );

	n_bmp bmp; n_bmp_zero( &bmp ); n_bmp_new_fast( &bmp, sx,sy );
	n_bmp_flush( &bmp, color_bg );

	n_bmp_squircle( &bmp, 0,0,sx,sy, n_bmp_white, 5.25 );

	n_bmp_resizer( &bmp, bmpsx,bmpsy, color_bg, N_BMP_RESIZER_CENTER );

	n_bmp_flush_antialias( &bmp, 1.0 );
	n_bmp_flush_antialias( &bmp, 1.0 );


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{//break;

		u32 c; n_bmp_ptr_get( &bmp, x, y, &c );
		if ( N_BMP_ALPHA_CHANNEL_INVISIBLE != n_bmp_a( c ) )
		{
			u32 t; n_bmp_ptr_get(  bmp_ret, x, y, &t );

			c = n_bmp_blend_pixel( c, t, (n_type_real) n_bmp_a( c ) / 256 );

			       n_bmp_ptr_set( &bmp    , x, y,  c );
		}

		x++;
		if ( x >= bmpsx )
		{
			x = 0;
			y++;
			if ( y >= bmpsy ) { break; }
		}
	}
//n_bmp_save_literal( &bmp, "result.bmp" );


	n_type_gfx n = 6;


	n_bmp shadow; n_bmp_zero( &shadow ); n_bmp_new_fast( &shadow, sx+n,sy+n );
	n_bmp_flush( &shadow, color_bg );

	n_bmp_squircle( &shadow, 0,0,sx+n,sy+n, n_bmp_argb( 128,0,0,0 ), 5.25 );
	n_bmp_resizer( &shadow, bmpsx,bmpsy, color_bg, N_BMP_RESIZER_CENTER );

	n_type_gfx i = 0;
	n_posix_loop
	{
		n_bmp_flush_antialias( &shadow, 1.0 );

		i++;
		if ( i >= n ) { break; }
	}


	n_bmp b; n_bmp_zero( &b ); n_bmp_new_fast( &b, bmpsx,bmpsy );
	n_bmp_flush( &b, color_bg );

	n_type_gfx o = 3 * ( bmpsy / 256 );

	n_bmp_transcopy( &shadow, &b, 0,0,bmpsx,bmpsy, 0,0 );
	n_bmp_transcopy( &shadow, &b, 0,0,bmpsx,bmpsy, 0,o );
	n_bmp_flush_transcopy( &bmp, &b );


	n_bmp_free( &bmp    );
	n_bmp_free( &shadow );


	n_bmp_free( bmp_ret );
	n_bmp_alias( &b, bmp_ret );


	return;
}




void
n_paint_bmp_contour( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		int a = n_bmp_a( color );
		//int r = n_bmp_r( color );
		//int g = n_bmp_g( color );
		//int b = n_bmp_b( color );

		if ( a != 0 )
		{
			color = n_bmp_black;
			n_bmp_ptr_set_fast( bmp, x,y, color );
		}

		x++;
		if ( x >= N_BMP_SX( bmp ) )
		{
			x = 0;
			y++;
			if ( y >= N_BMP_SY( bmp ) ) { break; }
		}
	}


	const n_type_gfx contour = 1;

	n_bmp bmp_old; n_bmp_zero( &bmp_old ); n_bmp_carboncopy( bmp, &bmp_old );

	x = -contour;
	y = -contour;
	n_posix_loop
	{

		n_bmp_transcopy( &bmp_old, bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), x,y );

		x++;
		if ( x > contour )
		{
			x = -contour;
			y++;
			if ( y > contour ) { break; }
		}
	}

	n_bmp_free( &bmp_old );


	return;
}




void
n_paint_bmp_line( n_bmp *bmp, n_type_gfx fx, n_type_gfx fy, n_type_gfx tx, n_type_gfx ty, u32 color, n_type_real blend )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_real step = 0;
	n_type_real unit = (n_type_real) n_posix_abs_n_type_gfx( fy - ty ) / n_posix_max_n_type_gfx( 1, n_posix_abs_n_type_gfx( fx - tx ) );


	n_posix_loop
	{

		// [!] : need to draw the first pos

		u32 c;
		n_bmp_ptr_get( bmp, fx,fy, &c );

		c = n_bmp_blend_pixel( c, color, blend );

		n_bmp_ptr_set( bmp, fx,fy,  c );


		// [!] : need to draw tx,ty once or more

		if ( ( fx == tx )&&( fy == ty ) ) { break; }


		// [!] : don't use "else" : avoid zig-zag

		if ( step < 1 )
		{
			step += unit;
			if ( fx > tx ) { fx--; } else if ( fx < tx ) { fx++; }
		}

		if ( step >= 1 )
		{
			step -= 1;
			if ( fy > ty ) { fy--; } else if ( fy < ty ) { fy++; }
		}

	}


	return;
}

