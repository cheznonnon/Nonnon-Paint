// Nonnon Wave
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_WAV_SAMPLE
#define _H_NONNON_NEUTRAL_WAV_SAMPLE




#include "./_error.c"


#include "../random.c"




#include <complex.h>




#define N_WAV_2PI ( 2.0 * M_PI )
#define N_WAV_AMP ( SHRT_MAX   )




void
n_wav_sample_get( n_wav *wav, u32 i, n_type_real *l, n_type_real *r )
{

	const u32 p = i * N_WAV_STEREO( wav );


	s16 *ptr = (s16*) N_WAV_PTR( wav );


	if ( l != NULL ) { (*l) = (n_type_real) ptr[ p + 0 ]; }
	if ( r != NULL ) { (*r) = (n_type_real) ptr[ p + 1 ]; }


	return;
}

void
n_wav_sample_set( n_wav *wav, u32 i, n_type_real l, n_type_real r )
{

	const u32 p = i * N_WAV_STEREO( wav );


	s16 *ptr = (s16*) N_WAV_PTR( wav );


	ptr[ p + 0 ] = (s16) l;
	ptr[ p + 1 ] = (s16) r;


	return;
}

#define n_wav_sample_clamp( n ) n_posix_minmax( SHRT_MIN, SHRT_MAX, n )

void
n_wav_sample_add( n_wav *wav, u32 i, n_type_real l, n_type_real r )
{

	const u32 p = i * N_WAV_STEREO( wav );


	s16 *ptr = (s16*) N_WAV_PTR( wav );


	ptr[ p + 0 ] = n_wav_sample_clamp( (int) l + ptr[ p + 0 ] );
	ptr[ p + 1 ] = n_wav_sample_clamp( (int) r + ptr[ p + 1 ] );


	return;
}

void
n_wav_sample_mix( n_wav *wav, u32 i, n_type_real l, n_type_real r, n_type_real ratio_l, n_type_real ratio_r )
{

	const u32 p = i * N_WAV_STEREO( wav );


	s16 *ptr = (s16*) N_WAV_PTR( wav );


	l *= ratio_l;
	r *= ratio_r;

	l += (n_type_real) ptr[ p + 0 ] * ( 1.0 - ratio_l );
	r += (n_type_real) ptr[ p + 1 ] * ( 1.0 - ratio_r );


	ptr[ p + 0 ] = (s16) l;
	ptr[ p + 1 ] = (s16) r;


	return;
}

n_type_real
n_wav_sample_hz2sample( n_type_real hz )
{

	// [!] : cache for performance

	static n_type_real p_hz = 0;
	static n_type_real ret  = 0;

	if ( p_hz != hz ) { ret = 44100.0 / hz; p_hz = hz; }


	return ret;
}

n_type_real
n_wav_sample_cosine( n_type_real hz, u32 x )
{

	// [!] : I : <complex.h> complex number

	n_type_real t = n_wav_sample_hz2sample( hz );
	n_type_real d;

#ifdef _MSC_VER

	d = (n_type_real) x / t;
	d = cos( N_WAV_2PI * d );
	d = N_WAV_AMP * d;

#else // #ifdef _MSC_VER

	d = (n_type_real) x / t;
	d = cexp( I * ( N_WAV_2PI * d ) );
	d = N_WAV_AMP * d;

#endif // #ifdef _MSC_VER

	return d;
}

n_type_real
n_wav_sample_sine_coeff( n_type_real hz, u32 x )
{

	n_type_real t = n_wav_sample_hz2sample( hz );
	n_type_real d;

	d = (n_type_real) x / t;
	d = sin( N_WAV_2PI * d );


	return d;
}

n_type_real
n_wav_sample_sine( n_type_real hz, u32 x )
{

	n_type_real d = n_wav_sample_sine_coeff( hz, x );

	d = N_WAV_AMP * d;


	return d;
}

n_type_real
n_wav_sample_sawtooth( n_type_real hz, u32 x )
{

	// [!] : currently cosine compatible curve will generate

	n_type_real t = n_wav_sample_hz2sample( hz );
	n_type_real d;

	d = fmod( x, t );
	d = d / t;
	d = N_WAV_AMP * d;
	d = d - ( N_WAV_AMP / 2 );
	d = d * -1;


	return d;
}

n_type_real
n_wav_sample_square( n_type_real hz, u32 x )
{

	n_type_real d = n_wav_sample_sine_coeff( hz, x );

	if ( d > 0 ) { d =  N_WAV_AMP; } else
	if ( d < 0 ) { d = -N_WAV_AMP; }


	return d;
}

n_type_real
n_wav_sample_sandstorm( n_type_real hz, u32 x )
{

	// [!] : human recognition
	//
	//	20Hz to 20,000Hz

	//n_type_real t = n_wav_sample_hz2sample( hz );
	n_type_real d = n_wav_sample_sine( n_random_range( (u32) hz ), x );


	return d;
}


#endif // _H_NONNON_NEUTRAL_WAV_SAMPLE

