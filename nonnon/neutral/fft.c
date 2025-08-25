// Nonnon Wave
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Fast Fourier Transform
//
// "C-gengo ni yoru saisin arugorizumu ziten" by Okumura Haruhiko
// ISBN4-87408-414-1 C3055
// based on "fft.c" at page 346


// [!] : based on "nonnon/project/toybox/fft/fft_2.c"




#ifndef _H_NONNON_NEUTRAL_FFT
#define _H_NONNON_NEUTRAL_FFT




#include <math.h>


#include "./string.c"




typedef struct {

	n_type_real x,y;

} n_fft;




#define n_fft_zero( p ) n_memory_zero( p, sizeof( n_fft ) )

void
n_fft_bulk_zero( n_fft *p, int count )
{

	if ( p == NULL ) { return; }
	if ( count <= 0 ) { return; }

	int i = 0;
	n_posix_loop
	{

		n_fft_zero( &p[ i ] );

		i++;
		if ( i >= count ) { break; }
	}
}

#define N_FFT_WINDOW_RECTANGULAR 0
#define N_FFT_WINDOW_HANNING     1
#define N_FFT_WINDOW_HAMMING     2

void
n_fft_window( n_fft *f, int n, int mode )
{

	if ( f == NULL ) { return; }
	if ( n <=    1 ) { return; }


	if ( mode == N_FFT_WINDOW_RECTANGULAR ) { return; }


	const n_type_real pp = M_PI * 2;

	n_type_real m    = (n_type_real) n - 1;
	n_type_real unit = pp / m;

	int i = 0;
	n_posix_loop
	{

		n_type_real d = 0;

		if ( mode == N_FFT_WINDOW_HANNING )
		{
			d  = 0.50;
			d *= 1.00 - cos( (n_type_real) unit * i );
		} else

		if ( mode == N_FFT_WINDOW_HAMMING )
		{
			d  = 0.54;
			d -= 0.46 * cos( (n_type_real) unit * i );
		} //else


		f[ i ].x *= d;


		i++;
		if ( i >= n ) { break; }
	}


	return;
}

n_fft
n_fft_add( n_fft f, n_fft t )
{

	n_fft ret;


	ret.x = f.x + t.x;
	ret.y = f.y + t.y;


	return ret;
}

n_fft
n_fft_sub( n_fft f, n_fft t )
{

	n_fft ret;


	ret.x = f.x - t.x;
	ret.y = f.y - t.y;


	return ret;
}

n_fft
n_fft_mul( n_fft f, n_fft t )
{

	n_fft ret;


	ret.x = ( f.x * t.x ) - ( f.y * t.y );
	ret.y = ( f.x * t.y ) + ( f.y * t.x );


	return ret;
}

n_fft
n_fft_div( n_fft f, n_fft t )
{

	n_fft ret;


	n_type_real r2 = ( t.x * t.x ) + ( t.y * t.y );


	ret.x  = ( f.x * t.x ) + ( f.y * t.y );
	ret.y  = ( f.y * t.x ) - ( f.x * t.y );
	ret.x /= r2;
	ret.y /= r2;


	return ret;
}

n_fft
n_fft_cartesian2polar( n_fft f )
{

	n_fft ret;


	ret.x = sqrt( pow( f.x, 2 ) + pow( f.y, 2 ) );
	ret.y = atan2( f.y, f.x );


	return ret;
}

n_fft
n_fft_polar2cartesian( n_fft f )
{

	n_fft ret;


	ret.x = f.x * cos( f.y );
	ret.y = f.x * sin( f.y );


	return ret;
}

n_type_real
n_fft_abs( n_fft f )
{

	n_type_real t,ret;


	if ( f.x == 0 ) { return fabs( f.y ); }
	if ( f.y == 0 ) { return fabs( f.x ); }


	if ( fabs( f.y ) > fabs( f.x ) )
	{
		t   = f.x / f.y;
		t   = t * t;
		ret = fabs( f.y ) * sqrt( 1 + t );

	} else {

		t   = f.y / f.x;
		t   = t * t;
		ret = fabs( f.x ) * sqrt( 1 + t );
	}


	return ret;
}

n_fft
n_fft_sqrt( n_fft f )
{

	n_fft ret;


	n_type_real r = n_fft_abs( f );
	n_type_real w = sqrt( r + fabs( f.x ) );

	if ( f.x >= 0 )
	{
		ret.x = M_SQRT1_2 * w;
		ret.y = M_SQRT1_2 * f.y / w;

	} else {

		ret.x = M_SQRT1_2 * fabs( f.y ) / w;

		if ( f.y >= 0 )
		{
			ret.y =  M_SQRT1_2 * w;
		} else {
			ret.y = -M_SQRT1_2 * w;
		}
	}


	return ret;
}

// internal
void
n_fft_sintbl( n_type_real *sintbl, int n )
{

	const int n2 = n / 2;
	const int n4 = n / 4;
	const int n8 = n / 8;


	n_type_real unit = sin( M_PI / n );
	n_type_real dc   = 2 * unit * unit;
	n_type_real ds   = sqrt( dc * ( 2 - dc ) );
 	n_type_real t    = 2 * dc;
	n_type_real c    = sintbl[ n4 ] = 1;
	n_type_real s    = sintbl[  0 ] = 0;


	int i;


	i = 1;
	n_posix_loop
	{

		c -= dc; dc += t * c;
		s += ds; ds -= t * s;

		sintbl[      i ] = s;
		sintbl[ n4 - i ] = c;


		i++;
		if ( i >= n8 ) { break; }
	}

	if ( n8 != 0 ) { sintbl[ n8 ] = sqrt( 0.5 ); }


	i = 0;
	n_posix_loop
	{

		sintbl[ n2 - i ] = sintbl[ i ];

		i++;
		if ( i >= n4 ) { break; }
	}


	i = 0;
	n_posix_loop
	{

		sintbl[ i + n2 ] = -sintbl[ i ];

		i++;
		if ( i >= ( n2 + n4 ) ) { break; }
	}


	return;
}

// internal
void
n_fft_bitrev( int *bitrev, int n )
{

	const int half = n / 2;


	int i = 0;
	int j = 0;
	n_posix_loop
	{

		bitrev[ i ] = j;

		i++;
		if ( i >= n ) { break; }

		int k = half;
		n_posix_loop
		{
			if ( k > j ) { j += k; break; }

			j -= k;
			k /= 2;
		}

	}


	return;
}

#define N_FFT_CODEC_ENCODE 0
#define N_FFT_CODEC_DECODE 1

int
n_fft_codec( n_fft *f, int count, int codec )
{

	{

		int *bitrev = n_memory_new_closed( count * sizeof( int ) );

		n_fft_bitrev( bitrev, count );

		int i = 0;
		n_posix_loop
		{

			int j = bitrev[ i ];

			if ( i < j )
			{
				n_type_real swap_x = f[ i ].x;  f[ i ].x = f[ j ].x;  f[ j ].x = swap_x;
				n_type_real swap_y = f[ i ].y;  f[ i ].y = f[ j ].y;  f[ j ].y = swap_y;
			}


			i++;
			if ( i >= count ) { break; }
		}

		n_memory_free_closed( bitrev );

	}


	const int quarter = count / 4;


	n_type_real *sintbl = n_memory_new_closed( ( count + quarter ) * sizeof( n_type_real ) );

	n_fft_sintbl( sintbl, count );


	// [!] : this will be 2 4 8 16 32 ...

	int power_prv = 1;
	int power_cur = power_prv * 2;


	int i = 0;
	n_posix_loop
	{    

		// [!] : n = 32
		//
		//	#0 : ( 32 /  2 ) * 0 = 0
		//	#1 : ( 32 /  4 ) * 1 = 8
		//	#2 : ( 32 /  8 ) * 2 = 8
		//	#3 : ( 32 / 16 ) * 3 = 6
		//	#4 : ( 32 / 32 ) * 4 = 4

		int h = ( count / power_cur ) * i;

		n_type_real coeff_cos = sintbl[ h + quarter ];
		n_type_real coeff_sin = sintbl[ h           ];

		if ( codec == N_FFT_CODEC_DECODE ) { coeff_sin *= -1; }


		int ii = i;
		n_posix_loop
		{

			int t = ii + power_prv;

			n_type_real dx = ( coeff_sin * f[ t ].y ) + ( coeff_cos * f[ t ].x );
			n_type_real dy = ( coeff_cos * f[ t ].y ) - ( coeff_sin * f[ t ].x );

			f[ t ].x = f[ ii ].x - dx; f[ ii ].x += dx;
			f[ t ].y = f[ ii ].y - dy; f[ ii ].y += dy;

			ii += power_cur;
			if ( ii >= count ) { break; }
		}


		i++;
		if ( i >= power_prv )
		{

			i = 0;

			power_prv = power_cur;
			power_cur = power_prv * 2;

			if ( power_prv >= count ) { break; }
		}
	}


	n_memory_free_closed( sintbl );


	if ( codec == N_FFT_CODEC_ENCODE )
	{

		int i = 0;
		n_posix_loop
		{

			f[ i ].x /= count;
			f[ i ].y /= count;

			i++;
			if ( i >= count ) { break; }
		}

	}


	return 0;
}

// internal
void
n_fft_debug_format( n_type_real d, n_posix_char *ret )
{

	// [!] : 11.4 == [ 6 + 1 + 4 ]
	//
	//	11 includes ".4" length


	n_posix_sprintf_literal( ret, "% 11.4f", fabs( d ) );

	if ( d == 0 )
	{
		//
	} else
	if ( d > 0 )
	{
		ret[ 0 ] = n_posix_literal( '+' );
	} else
	if ( d < 0 )
	{
		ret[ 0 ] = n_posix_literal( '-' );
	}


	return;
}

void
n_fft_debug_sample( n_fft *l, n_fft *r, int i, n_posix_char *ret )
{

	n_posix_char str_lx[ 100 ];
	n_posix_char str_ly[ 100 ];
	n_posix_char str_rx[ 100 ];
	n_posix_char str_ry[ 100 ];

	n_fft_debug_format( l[ i ].x, str_lx );
	n_fft_debug_format( l[ i ].y, str_ly );
	n_fft_debug_format( r[ i ].x, str_rx );
	n_fft_debug_format( r[ i ].y, str_ry );

	n_posix_sprintf_literal( ret, "%4d : %s : %s : %s : %s :", i, str_lx, str_ly, str_rx, str_ry );


	return;
}

void
n_fft_debug_bitrev_iotest( int count )
{

#ifdef _H_NONNON_NEUTRAL_TXT


	n_txt txt;
	n_txt_zero( &txt );
	n_txt_new ( &txt );


	int *bitrev = n_memory_new_closed( count * sizeof( int ) );
	n_fft_bitrev( bitrev, count );


	n_posix_char str[ 100 ];

	int i = 0;
	n_posix_loop
	{

		n_posix_sprintf_literal( str, "#%02d #%02d : %02d", i, count - 1 - i, bitrev[ i ] );
		n_txt_add( &txt, i, str );

		i++;
		if ( i >= count ) { break; }
	}

	n_memory_free_closed( bitrev );


	n_posix_sprintf_literal( str, "./bitrev_%02d.txt", count );

	n_txt_save( &txt, str );
	n_txt_free( &txt );


#endif // #ifdef _H_NONNON_NEUTRAL_TXT


	return;
}


#endif // _H_NONNON_NEUTRAL_FFT

