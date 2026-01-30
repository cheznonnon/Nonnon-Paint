// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




u32
n_mac_bmp_ahsl2argb( u32 hsl )
{

	n_type_real a = (n_type_real)              n_bmp_a( hsl );
	n_type_real h = n_bmp_table_coeff_channel( n_bmp_h( hsl ) );
	n_type_real s = n_bmp_table_coeff_channel( n_bmp_s( hsl ) );
	n_type_real l = n_bmp_table_coeff_channel( n_bmp_l( hsl ) );


	n_type_real min,max;

	if ( l <= 0.5 )
	{
		max = ( l * ( 1.0 + s ) );
	} else {
		max = ( l * ( 1.0 - s ) ) + s;
	}

	min = ( 2 * l ) - max;


//n_posix_debug_literal( "%f %f %f", h,s,l );

	n_type_real r,g,b;

	if ( s == 0.0 )
	{

		r = g = b = l;

	} else {

		h *= 360;

		r = h + 120.0;
		g = h;
		b = h - 120.0;

		if ( r >= 360.0 ) { r -= 360.0; }
		if ( b <    0.0 ) { b += 360.0; }

		r = n_bmp_ahsl2argb_channel( min, max, r );
		g = n_bmp_ahsl2argb_channel( min, max, g );
		b = n_bmp_ahsl2argb_channel( min, max, b );

	}


	// Normalization

	r *= 255.0;
	g *= 255.0;
	b *= 255.0;


	// [Needed] : Mac : cast to int

	return n_bmp_argb_mac( a, (int) r, (int) g, (int) b );
}

u32
n_mac_bmp_argb2ahsl( u32 color )
{

	// [!] : range of return value : 0 => 255

	n_type_real a = (n_type_real)              n_bmp_a( color );
	n_type_real r = n_bmp_table_coeff_channel( n_bmp_b( color ) );
	n_type_real g = n_bmp_table_coeff_channel( n_bmp_g( color ) );
	n_type_real b = n_bmp_table_coeff_channel( n_bmp_r( color ) );


	// [!] : this code is slower than below : 0.5%

	//n_type_real min = n_posix_min_n_type_real( r, n_posix_min_n_type_real( g, b ) );
	//n_type_real max = n_posix_max_n_type_real( r, n_posix_max_n_type_real( g, b ) );

	n_type_real min,max;
	min = max = r;

	if ( min > g ) { min = g; }
	if ( min > b ) { min = b; }

	if ( max < g ) { max = g; }
	if ( max < b ) { max = b; }


	// Hue

	n_type_real h = 0.0;

	if ( max == min )
	{

		h = 0.0;

	} else
	if ( max == r )
	{

		h = 60.0 * ( ( g - b ) / ( max - min ) );
		h = fmod( h, 360.0 );

	} else
	if ( max == g )
	{

		h = 60.0 * ( ( b - r ) / ( max - min ) );
		h = h + 120.0;

	} else
	if ( max == b )
	{

		h = 60.0 * ( ( r - g ) / ( max - min ) );
		h = h + 240.0;

	}


	// Lightness

	n_type_real l = ( max + min ) / 2.0;


	// Saturation

	n_type_real s = 0;

	if ( max == min )
	{
		//s = 0;
	} else
	if ( l <= 0.5 )
	{
		s = ( max - min ) / ( max + min );
	} else {
		s = ( max - min ) / ( 2.0 - ( max + min ) );
	}


	// Normalization

//n_posix_debug_literal( "%f %f %f", h,s,l );

	h *= 1.0f / 360.0f;
	//h /= 360.0;

	h *= 255.0;
	s *= 255.0;
	l *= 255.0;


	// [Needed] : Mac : cast to int

	return n_bmp_ahsl( a, (int) h, (int) s, (int) l );
}

#define n_mac_bmp_hue_wheel_tweak_pixel( color, h ) n_mac_bmp_hsl_tweak_pixel( color, h, 0, 0 )
#define n_mac_bmp_vividness_tweak_pixel( color, s ) n_mac_bmp_hsl_tweak_pixel( color, 0, s, 0 )
#define n_mac_bmp_lightness_tweak_pixel( color, l ) n_mac_bmp_hsl_tweak_pixel( color, 0, 0, l )

u32
n_mac_bmp_hsl_tweak_pixel( u32 color, int hue, int saturation, int lightness )
{

	color = n_mac_bmp_argb2ahsl( color );

	int a = n_bmp_a( color );
	int h = n_bmp_h( color );
	int s = n_bmp_s( color );
	int l = n_bmp_l( color );


	// [!] : saturation : no touch when grayscale

	h = n_bmp_clamp_channel( h + hue );
	if ( s != 0 ) { s = n_bmp_clamp_channel( s + saturation ); }
	l = n_bmp_clamp_channel( l + lightness );


	color = n_mac_bmp_ahsl2argb( n_bmp_ahsl( a,h,s,l ) );


	return color;
}




typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	int         hue, saturation, lightness;
	u32         oy, cores;

} n_mac_bmp_tweaker_hsl_thread_struct;

void
n_mac_bmp_tweaker_hsl_thread_main( n_mac_bmp_tweaker_hsl_thread_struct *p )
{

	n_type_index c = N_BMP_SX( p->bmp ) * N_BMP_SY( p->bmp );
	n_type_index i = p->oy; if ( i >= c ) { return; }
	n_posix_loop
	{

		u32 color = N_BMP_PTR( p->bmp )[ i ];
		if ( FALSE == n_bmp_is_trans( p->bmp, color ) )
		{
			N_BMP_PTR( p->bmp )[ i ] = n_mac_bmp_hsl_tweak_pixel( color, p->hue, p->saturation, p->lightness );
		}

		i += p->cores;
		if ( i >= c ) { break; }
	}


	return;
}

#define n_mac_bmp_flush_tweaker_hsl( bmp, h, s, l ) \
	n_mac_bmp_tweaker_hsl( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), h, s, l )

void
n_mac_bmp_tweaker_hsl( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, int hue, int saturation, int lightness )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [!] : nothing to do

	if ( ( hue == 0 )&&( saturation == 0 )&&( lightness == 0 ) ) { return; }


	n_mac_bmp_tweaker_hsl_thread_struct tmp = { bmp, x,y,sx,sy, hue, saturation, lightness, 0,1 };
	n_mac_bmp_tweaker_hsl_thread_main( &tmp );


	return;
}
