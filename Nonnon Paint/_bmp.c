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
n_paint_bmp_flush_posterization( n_bmp *bmp, int param )
{

	if ( n_bmp_error( bmp ) ) { return; }


	if ( param <= 0 ) { return;}


	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		int a =       n_bmp_a( color );
		int r = (int) n_bmp_r( color ) / param * param;
		int g = (int) n_bmp_g( color ) / param * param;
		int b = (int) n_bmp_b( color ) / param * param;

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
n_paint_bmp_thicken( n_bmp *arg, int kernel )
{

	// [!] : DeepSeek AI : Morphology Delation


	if ( kernel < 3 ) { return; }


	n_bmp bmp; n_bmp_carboncopy( arg, &bmp );


	//const int kernel = 3;
	const int offset = kernel / 2;


	const n_type_gfx sx = N_BMP_SX( arg );
	const n_type_gfx sy = N_BMP_SY( arg );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		int a = 0, r = 0, g = 0, b = 0;

		n_type_gfx kx = -offset;
		n_type_gfx ky = -offset;
		n_posix_loop
		{

			int nx = x + kx;
			int ny = y + ky;
			if ( n_bmp_ptr_is_accessible( arg, nx,ny ) )
			{
				u32 color; n_bmp_ptr_get_fast( arg, nx,ny, &color );

				if ( a < n_bmp_a( color ) ) { a = n_bmp_a( color ); }
				if ( r < n_bmp_r( color ) ) { r = n_bmp_r( color ); }
				if ( g < n_bmp_g( color ) ) { g = n_bmp_g( color ); };
				if ( b < n_bmp_b( color ) ) { b = n_bmp_b( color ); };
			}

			kx++;
			if ( kx > offset )
			{
				kx = -offset;

				ky++;
				if ( ky > offset ) { break; }
			}
		}

		n_bmp_ptr_set( &bmp, x,y, n_bmp_argb( a,r,g,b ) );


		x++;
		if ( x >= sx )
		{
			x = 0;

			y++;
			if ( y >= sy ) { break; }
		}
	}


	n_bmp_free_fast( arg );
	n_bmp_alias( &bmp, arg );


	return;
}

void
n_paint_bmp_thin( n_bmp *arg, int kernel )
{

	// [!] : DeepSeek AI : Morphology Erosion


	if ( kernel < 3 ) { return; }


	n_bmp bmp; n_bmp_carboncopy( arg, &bmp );


	//const int kernel = 3;
	const int offset = kernel / 2;


	const n_type_gfx sx = N_BMP_SX( arg );
	const n_type_gfx sy = N_BMP_SY( arg );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		int a = 255, r = 255, g = 255, b = 255;

		n_type_gfx kx = -offset;
		n_type_gfx ky = -offset;
		n_posix_loop
		{

			int nx = x + kx;
			int ny = y + ky;
			if ( n_bmp_ptr_is_accessible( arg, nx,ny ) )
			{
				u32 color; n_bmp_ptr_get_fast( arg, nx,ny, &color );

				if ( a > n_bmp_a( color ) ) { a = n_bmp_a( color ); }
				if ( r > n_bmp_r( color ) ) { r = n_bmp_r( color ); }
				if ( g > n_bmp_g( color ) ) { g = n_bmp_g( color ); };
				if ( b > n_bmp_b( color ) ) { b = n_bmp_b( color ); };
			}

			kx++;
			if ( kx > offset )
			{
				kx = -offset;

				ky++;
				if ( ky > offset ) { break; }
			}
		}

		n_bmp_ptr_set( &bmp, x,y, n_bmp_argb( a,r,g,b ) );


		x++;
		if ( x >= sx )
		{
			x = 0;

			y++;
			if ( y >= sy ) { break; }
		}
	}


	n_bmp_free_fast( arg );
	n_bmp_alias( &bmp, arg );


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




n_type_index
n_bmp_fill_special( n_bmp *bmp, n_bmp *ret, n_type_gfx x, n_type_gfx y, u32 color, u32 white )
{

	// [!] : return value : processed pixel count


	if ( n_bmp_error( bmp ) ) { return 0; }
	if ( n_bmp_error( ret ) ) { return 0; }


	// [!] : fail-safe

	u32 color_to;

	n_bmp_ptr_get( bmp, x,y, &color_to );
	if ( color == color_to ) { return 0; }


	const int threshold = 256;


	u8 *map = (u8*) n_memory_new_closed( N_BMP_SX( bmp ) * N_BMP_SY( bmp ) * sizeof( u8 ) );


	int move = 0;
	int stop = 0;

	n_type_index count = 0;
	n_type_index i     = 0;
	n_posix_loop
	{

		u32 c1 = 0;
		u32 c2 = 0;

		if (
			( FALSE == n_bmp_ptr_get( bmp, x,y, &c1 ) )
			&&
			( FALSE == n_bmp_ptr_get( ret, x,y, &c2 ) )
			&&
			(
				//( c1 == color_to )
				//||
				( n_bmp_color_similarity( c1, color_to, threshold ) )
			)
			&&
			( c2 != white )
		)
		{

			stop = 0;

			n_bmp_ptr_set_fast( bmp, x,y, color ); count++;
			n_bmp_ptr_set_fast( ret, x,y, white );

			map[ i ] = move;

			i++;

		} else {

			if ( move == 0 ) { y++; } else
			if ( move == 1 ) { x--; } else
			if ( move == 2 ) { y--; } else
			if ( move == 3 ) { x++; }

			stop++;
			if ( stop >= 4 )
			{

				stop = 0;

				if ( i <= 1 ) { break; }


				i--;

				move = map[ i ];
				if ( move == 0 ) { y++; } else
				if ( move == 1 ) { x--; } else
				if ( move == 2 ) { y--; } else
				if ( move == 3 ) { x++; }

			}

			move++;
			if ( move >= 4 ) { move = 0; }

		}

		if ( move == 0 ) { y--; } else
		if ( move == 1 ) { x++; } else
		if ( move == 2 ) { y++; } else
		if ( move == 3 ) { x--; }

	}


	n_memory_free_closed( map );


	return count;
}




void
n_paint_bmp_flush_outline( n_bmp *bmp_arg, u32 color_outline )
{

	// [!] : Thx : DeepSeek AI : Simplified Sobel Filter


	// [x] : this logic cannot control thickness


	if ( n_bmp_error( bmp_arg ) ) { return; }


	n_bmp bmp; n_bmp_carboncopy( bmp_arg, &bmp );

	u32 c; n_bmp_ptr_get( &bmp, 0,0, &c );
	n_bmp_flush_replacer( &bmp, c, n_bmp_black_invisible );


	n_bmp ret; n_bmp_carboncopy( bmp_arg, &ret );
	n_bmp_flush( &ret, n_bmp_black_invisible );


	const int threshold = 100;


	n_type_gfx sx = N_BMP_SX( &bmp );
	n_type_gfx sy = N_BMP_SY( &bmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{//break;

		u32 color_tl = 0; n_bmp_ptr_get( &bmp, x-1,y-1, &color_tl );
		u32 color_tm = 0; n_bmp_ptr_get( &bmp, x-0,y-1, &color_tm );
		u32 color_tr = 0; n_bmp_ptr_get( &bmp, x+1,y-1, &color_tr );

		u32 color_ml = 0; n_bmp_ptr_get( &bmp, x-1,y-0, &color_ml );
		//u32 color_mm = 0; n_bmp_ptr_get( &bmp, x-0,y-0, &color_mm );
		u32 color_mr = 0; n_bmp_ptr_get( &bmp, x+1,y-0, &color_mr );

		u32 color_bl = 0; n_bmp_ptr_get( &bmp, x-1,y+1, &color_bl );
		u32 color_bm = 0; n_bmp_ptr_get( &bmp, x-0,y+1, &color_bm );
		u32 color_br = 0; n_bmp_ptr_get( &bmp, x+1,y+1, &color_br );

		int tl = n_bmp_a( color_tl );
		int tm = n_bmp_a( color_tm );
		int tr = n_bmp_a( color_tr );

		int ml = n_bmp_a( color_ml );
		//int mm = n_bmp_a( color_mm );
		int mr = n_bmp_a( color_mr );

		int bl = n_bmp_a( color_bl );
		int bm = n_bmp_a( color_bm );
		int br = n_bmp_a( color_br );

		n_type_gfx gx = -tl + tr - 2*ml + 2*mr - bl + br;
		n_type_gfx gy = -tl - 2*tm - tr + bl + 2*bm + br;

		int mag = (int) sqrt( gx*gx + gy*gy );

		if ( mag > threshold )
		{
			n_bmp_ptr_set_fast( &ret, x,y, n_bmp_white );
		}

		x++;
		if ( x >= sx )
		{
			x = 0;
			y++;
			if ( y >= sy ) { break; }
		}
	}


	n_bmp_flush_antialias( &ret, 1.0 );
	n_paint_bmp_thicken( &ret, 7 );

	n_bmp_rasterizer( &ret, bmp_arg, 0,0, color_outline );

	n_bmp_free_fast( &bmp );
	n_bmp_free_fast( &ret );


	return;
}
