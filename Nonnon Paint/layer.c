// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#include "../nonnon/mac/n_txtbox.c"




#define N_PAINT_LAYER_INI_FILENAME n_posix_literal( "nonnon_paint.ini" )
#define N_PAINT_LAYER_INI_SECTION  n_posix_literal( "[Nonnon Paint Layer]" )
#define N_PAINT_LAYER_INI_INDEX    n_posix_literal( "index  " )
#define N_PAINT_LAYER_INI_NUMBER   n_posix_literal( "number " )
#define N_PAINT_LAYER_INI_NAME     n_posix_literal( "name   " )
#define N_PAINT_LAYER_INI_VISIBLE  n_posix_literal( "visible" )
#define N_PAINT_LAYER_INI_PERCENT  n_posix_literal( "percent" )
#define N_PAINT_LAYER_INI_BLUR     n_posix_literal( "blur   " )




#define n_paint_layer_ini_read(  i ) n_paint_layer_ini_main( i, TRUE  )
#define n_paint_layer_ini_write( i ) n_paint_layer_ini_main( i, FALSE )

// internal
void
n_paint_layer_ini_main( n_ini *ini, BOOL is_read )
{

	n_paint *paint = n_paint_global.paint;


	if ( is_read )
	{
		n_posix_char str[ 100 ];
		n_ini_value_str( ini, N_PAINT_LAYER_INI_SECTION, N_PAINT_LAYER_INI_INDEX, N_STRING_EMPTY, str, 100 );
	} else {
		n_ini_new( ini );
	}


	n_type_int i = paint->layer_count - 1;
	n_posix_loop
	{

		n_posix_char str_i[ N_STRING_INT2STR_CCH_MAX ]; n_string_int2str( str_i, i );

		n_posix_char section[ 100 ];
		n_posix_sprintf_literal( section, "[%s]", str_i );

		n_ini_section_add( ini, section );

		if ( is_read )
		{
			n_posix_char str[ N_PAINT_LAYER_CCH ];
			n_ini_value_str( ini, section, N_PAINT_LAYER_INI_NAME, N_STRING_EMPTY, str, N_PAINT_LAYER_CCH );
			n_string_copy( str, paint->layer_data[ i ].name );

			BOOL visible = n_ini_value_int( ini, section, N_PAINT_LAYER_INI_VISIBLE, TRUE );
			int  percent = n_ini_value_int( ini, section, N_PAINT_LAYER_INI_PERCENT,  100 );
			int     blur = n_ini_value_int( ini, section, N_PAINT_LAYER_INI_BLUR   ,    0 );
//NSLog( @"#%lld : %d %d %d", i, visible, percent, blur );

			paint->layer_data[ i ].visible = visible;
			paint->layer_data[ i ].percent = percent;
			paint->layer_data[ i ].blur    = blur;
			paint->layer_data[ i ].blend   = (n_type_real) percent * 0.01;
		} else {
			n_posix_char str[ N_PAINT_LAYER_CCH ];
			n_string_copy( paint->layer_data[ i ].name, str );
//NSLog( @"ini_write() : %lld : %s", i, paint->layer_data[ i ].name );

			BOOL visible = paint->layer_data[ i ].visible;
			int  percent = paint->layer_data[ i ].percent;
			int     blur = paint->layer_data[ i ].blur;

			n_ini_key_add_str( ini, section, N_PAINT_LAYER_INI_NAME, str );

			n_ini_key_add_int( ini, section, N_PAINT_LAYER_INI_VISIBLE, visible );
			n_ini_key_add_int( ini, section, N_PAINT_LAYER_INI_PERCENT, percent );
			n_ini_key_add_int( ini, section, N_PAINT_LAYER_INI_BLUR   , blur    );
		}

		i--;
		if ( i < 0 ) { break; }
	}


	if ( is_read )
	{
		//
	} else {
		n_ini_section_add( ini, N_PAINT_LAYER_INI_SECTION );
		n_ini_key_add_int( ini, N_PAINT_LAYER_INI_SECTION, N_PAINT_LAYER_INI_INDEX , (int) paint->layer_index );
		n_ini_key_add_int( ini, N_PAINT_LAYER_INI_SECTION, N_PAINT_LAYER_INI_NUMBER, (int) paint->layer_count );
	}


	return;
}




void
n_paint_layer_config_default_single( n_paint *paint, n_type_int i )
{

	paint->layer_data[ i ].visible = n_posix_true;
	paint->layer_data[ i ].percent = 100;
	paint->layer_data[ i ].blend   = 1.0;
	paint->layer_data[ i ].blur    =   0;


	return;
}

void
n_paint_layer_config_default( n_paint *paint )
{

	n_type_int i = 0;
	n_posix_loop
	{

		n_paint_layer_config_default_single( paint, i );

		i++;
		if ( i >= paint->layer_count ) { break; }
	}


	return;
}




void
n_paint_layer_text_set( n_paint *paint, n_type_int i, n_posix_char *str )
{

	n_posix_sprintf_literal( paint->layer_data[ i ].name, "%s", str );

	// [!] : bug or spec : there is a problem in behavior when empty

	if ( n_string_is_empty( str ) )
	{
		n_txt_add( &paint->layer_txt, i, str );
	} else {
		n_txt_set( &paint->layer_txt, i, str );
	}
//NSLog( @"%s", n_txt_get( &paint->layer_txt, i ) );


	n_posix_char deco[ 2 ] = { 0, 0 };

	if ( paint->layer_data[ i ].visible )
	{
		deco[ 0 ] = 'B';
	} else {
		deco[ 0 ] = ' ';
	}

	n_txt_set( &paint->layer_decoration_txt, i, deco );


	return;
}

void
n_paint_layer_text_mod( n_paint *paint, n_type_int i, n_posix_char *str )
{


	n_posix_sprintf_literal( paint->layer_data[ i ].name, "%s", str );
//NSLog( @"%s %s", paint->layer_data[ i ].name, str );

	n_txt_mod( &paint->layer_txt, i, str );


	n_posix_char deco[ 2 ] = { 0, 0 };

	if ( paint->layer_data[ i ].visible )
	{
		deco[ 0 ] = 'B';
	} else {
		deco[ 0 ] = ' ';
	}

	n_txt_mod( &paint->layer_decoration_txt, i, deco );


	return;
}




void
n_bmp_layer_free( n_paint *paint )
{

	n_type_int i = 0;
	n_posix_loop
	{

		n_bmp_free( &paint->layer_data[ i ].bmp_data );
		n_bmp_free( &paint->layer_data[ i ].bmp_grab );

		n_bmp_free( &paint->layer_data[ i ].bmp_blur );

		i++;
		if ( i >= paint->layer_count ) { break; }
	}


	return;
}

void
n_bmp_layer_turn_off( n_paint *paint )
{

	paint->layer_onoff = FALSE;

	paint->layer_load_as_layer = FALSE;

	n_bmp_layer_free( paint );
	//n_memory_free( paint->layer_data );

	paint->pen_bmp_data = &paint->bmp_data;
	paint->pen_bmp_grab = &paint->bmp_grab;


	return;
}

n_posix_bool
n_paint_layer_load( n_paint *paint, const n_posix_char *cmdline )
{

	n_posix_char *name     = NULL;
	n_posix_char *name_ini = NULL;

	if ( n_posix_stat_is_dir( cmdline ) )
	{
//NSLog( @"Folder" );
		name     = n_string_path_carboncopy( cmdline );
		name_ini = n_string_path_make_new( name, N_PAINT_LAYER_INI_FILENAME );
	} else
	if ( n_string_path_ext_is_same_literal( ".INI\0\0", cmdline ) )
	{
//NSLog( @"INI" );
		name     = n_string_path_upperfolder_new( cmdline );
		name_ini = n_string_path_make_new( name, N_PAINT_LAYER_INI_FILENAME );

//NSLog( @"%s : %s", cmdline, name_ini );
		if ( n_string_is_same_literal( cmdline, name_ini ) )
		{
//NSLog( @"OK" );
			//
		} else {
//NSLog( @"Error" );
			n_string_path_free( name     );
			n_string_path_free( name_ini );

			return n_posix_true;
		}
	} else {
		return n_posix_true;
	}
//NSLog( @"%s : %s", name, name_ini );

//return n_posix_true;


	n_ini ini_sniffer; n_ini_zero( &ini_sniffer );
	n_posix_bool ret = n_txt_load_utf8( (void*) &ini_sniffer, name_ini );
//NSLog( @"%d", ret );
	if ( ini_sniffer.readonly ) { ret = n_posix_true; }


	if ( ret == n_posix_false )
	{
		ret = n_ini_section_chk( &ini_sniffer, N_PAINT_LAYER_INI_SECTION );
		if ( ret ) { ret = n_posix_false; } else { ret = n_posix_true; }
	}


//NSLog( @"%d", ret ); ret = TRUE;
	if ( ret == n_posix_false )
	{

		// [!] : prepare for newer data

		n_ini_free( &paint->layer_ini );
		n_memory_copy( &ini_sniffer, &paint->layer_ini, sizeof( n_ini ) );

		n_paint_layer_ini_read( &paint->layer_ini );

		n_bmp_layer_free( paint );
		n_memory_free( paint->layer_data );

		paint->filename = [NSString stringWithFormat:@"%s.lyr", name];

		n_txt_new( &paint->layer_txt );
		n_txt_new( &paint->layer_decoration_txt );


		// [!] : load

		paint->layer_count = n_ini_value_int( &paint->layer_ini, N_PAINT_LAYER_INI_SECTION, N_PAINT_LAYER_INI_NUMBER, N_PAINT_LAYER_MAX );
		if ( paint->layer_count < N_PAINT_LAYER_MAX ) { paint->layer_count = N_PAINT_LAYER_MAX; }
//NSLog( @"Layer Cout : %d", paint->layer_count );

		paint->layer_data = n_memory_new( sizeof( n_paint_layer ) * paint->layer_count );
		n_memory_zero( paint->layer_data, sizeof( n_paint_layer ) * paint->layer_count );


		// [!] : progress indicator

		n_type_real step = 100.0 / paint->layer_count;
		n_type_real pc   = 0;


		n_type_int i = 0;
		n_posix_loop
		{

			n_posix_char str_oy[ N_STRING_INT2STR_CCH_MAX ]; n_string_int2str( str_oy, i );

			n_posix_char str[ 100 ]; n_posix_sprintf_literal( str, "%s.png", str_oy );
			n_posix_char *name_img = n_string_path_make_new( name, str );
//NSLog( @"%s", name_img );

			n_bmp_free_fast( &paint->layer_data[ i ].bmp_data );
			n_png_png2bmp( name_img, &paint->layer_data[ i ].bmp_data );

//n_posix_bool png_ret = n_png_png2bmp( name_img, &paint->layer_data[ i ].bmp_data );
//NSLog( @"%d", png_ret );

			if ( n_bmp_error( &paint->layer_data[ i ].bmp_data ) )
			{
				n_type_gfx sx = N_BMP_SX( &paint->layer_data[ 0 ].bmp_data );
				n_type_gfx sy = N_BMP_SY( &paint->layer_data[ 0 ].bmp_data );

				n_bmp_new_fast( &paint->layer_data[ i ].bmp_data, sx, sy );
				n_bmp_flush( &paint->layer_data[ i ].bmp_data, n_bmp_white_invisible );
			} else {
				n_bmp_mac_color( &paint->layer_data[ i ].bmp_data );
			}

			n_string_path_free( name_img );


			n_posix_char sec[ N_PAINT_LAYER_CCH ]; n_posix_sprintf_literal( sec, "[%lld]", i );


			BOOL visible = n_ini_value_int( &paint->layer_ini, sec, N_PAINT_LAYER_INI_VISIBLE, TRUE );
			int  percent = n_ini_value_int( &paint->layer_ini, sec, N_PAINT_LAYER_INI_PERCENT,  100 );
			int     blur = n_ini_value_int( &paint->layer_ini, sec, N_PAINT_LAYER_INI_BLUR   ,    0 );
//NSLog( @"%s : %s : %d %d %d", sec, nam, visible, percent, blur );

			paint->layer_data[ i ].visible = visible;
			paint->layer_data[ i ].percent = percent;
			paint->layer_data[ i ].blend   = (n_type_real) percent * 0.01;
			paint->layer_data[ i ].blur    = blur;


			n_posix_char nam[ N_PAINT_LAYER_CCH ];
			n_ini_value_str( &paint->layer_ini, sec, N_PAINT_LAYER_INI_NAME, N_STRING_EMPTY, nam, N_PAINT_LAYER_CCH );

			n_paint_layer_text_set( paint, i, nam );


			// [!] : progress indicator

			pc += step;
			if ( pc >= 100 ) { pc = 100; }

			paint->layer_load_onoff   = TRUE;
			paint->layer_load_percent = pc;
			[n_paint_global display];
			[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate date]];


			i++;
			if ( i >= paint->layer_count ) { break; }
		}


		paint->layer_index = n_ini_value_int( &paint->layer_ini, N_PAINT_LAYER_INI_SECTION, N_PAINT_LAYER_INI_INDEX, 0 );
		paint->layer_index = n_posix_minmax_n_type_int( 0, paint->layer_count - 1, paint->layer_index );
//NSLog( @"%d", paint->layer_index );

		paint->pen_bmp_data = &paint->layer_data[ paint->layer_index ].bmp_data;
		paint->pen_bmp_grab = &paint->layer_data[ paint->layer_index ].bmp_grab;


		paint->layer_onoff = TRUE;

		paint->layer_load_as_layer = TRUE;


		[n_paint_global display_optimized];

	} else {

		n_bmp_layer_turn_off( paint );

		n_ini_free( &ini_sniffer );

	}


	n_string_path_free( name     );
	n_string_path_free( name_ini );


	return ret;
}

BOOL
n_paint_layer_save( n_paint *paint, const n_posix_char *cmdline )
{

	n_posix_char *name = n_string_path_carboncopy( cmdline );


	if ( n_posix_false == n_posix_stat_is_exist( name ) )
	{
		if ( n_posix_mkdir( name, 0777 ) ) { return TRUE; }
	}


	if ( paint->layer_load_as_layer )
	{

		n_paint_layer_ini_write( &paint->layer_ini );

	} else {

		paint->layer_load_as_layer = TRUE;

		paint->layer_onoff = n_posix_true;
		paint->layer_count = N_PAINT_LAYER_MAX;

		//paint->layer_first_index = 0;

		n_type_gfx bmpsx = N_BMP_SX( &paint->bmp_data );
		n_type_gfx bmpsy = N_BMP_SY( &paint->bmp_data );
//NSLog( @"%d : %d %d", paint->layer_count, bmpsx, bmpsy );

		n_type_int i = 0;
		n_posix_loop
		{
			if ( i == 0 )
			{
				n_bmp_carboncopy( &paint->bmp_data, &paint->layer_data[ i ].bmp_data );
			} else {
				n_bmp_new_fast( &paint->layer_data[ i ].bmp_data, bmpsx, bmpsy );
				n_bmp_flush( &paint->layer_data[ i ].bmp_data, n_bmp_white_invisible );
			}

			i++;
			if ( i >= paint->layer_count ) { break; }
		}

		n_paint_layer_config_default( paint );

		i = 0;
		n_posix_loop
		{

			n_string_truncate( paint->layer_data[ i ].name );

			if ( i == 0 )
			{
				n_posix_strcat( paint->layer_data[ i ].name, n_posix_literal( "Background" ) );
			} else
			if ( i == ( paint->layer_count - 1 ) )
			{
				n_posix_strcat( paint->layer_data[ i ].name, n_posix_literal( "Top" ) );
			}

			i++;
			if ( i >= paint->layer_count ) { break; }
		}


		n_ini_new( &paint->layer_ini );

		n_paint_layer_ini_write( &paint->layer_ini );

	}


	n_posix_char *ini_name = n_string_path_make_new( name, N_PAINT_LAYER_INI_FILENAME );
//n_posix_debug_literal( " %s ", ini_name );


	// [!] : progress indicator

	u32 wait = n_posix_tickcount();

	n_type_real step = 100.0 / paint->layer_count;
	n_type_real pc   = 0;


	n_type_int i = 0;
	n_posix_loop
	{//break;

		n_posix_char *path = n_string_new( n_posix_strlen( name ) * 2 );
		n_posix_sprintf_literal( path, "%s/%lld.png", name, i );

		n_bmp bmp_check; n_bmp_zero( &bmp_check );
		n_png_png2bmp( path, &bmp_check );
		n_bmp_mac_color( &bmp_check );

		n_type_int cb_a = N_BMP_SIZE( &bmp_check );
		n_type_int cb_b = N_BMP_SIZE( &paint->layer_data[ i ].bmp_data );
		n_type_int cb_c = n_posix_min_n_type_int( cb_a, cb_b );

		if (
			( cb_a == cb_b )
			&&
			( n_memory_is_same( N_BMP_PTR( &bmp_check ), N_BMP_PTR( &paint->layer_data[ i ].bmp_data ), cb_c ) )
			&&
			( N_BMP_SX( &bmp_check ) == N_BMP_SX( &paint->layer_data[ i ].bmp_data ) )
			&&
			( N_BMP_SY( &bmp_check ) == N_BMP_SY( &paint->layer_data[ i ].bmp_data ) )
		)
		{
//NSLog( @"Same : %lld : cb %lld %lld", i, cb_a, cb_b );
		} else {
//NSLog( @"Not the Same : %lld", i );

			n_bmp *bmp = &paint->layer_data[ i ].bmp_data;

			extern n_posix_bool n_paint_save_png( n_bmp *bmp, n_posix_char *path, n_posix_bool n_bmp_carboncopy_onoff );
			n_paint_save_png( bmp, path, n_posix_true );

		}


		// [!] : progress indicator

		pc += step;
		if ( pc >= 100 ) { pc = 100; }

		if ( n_game_timer_once( &wait, 200 ) )
		{
			paint->layer_load_onoff   = TRUE;
			paint->layer_load_percent = pc;
			[n_paint_global display];
			[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate date]];
		}

		i++;
		if ( i >= paint->layer_count ) { break; }
	}


	n_ini_save( &paint->layer_ini, ini_name );


	n_string_path_free( ini_name );
	n_string_path_free(     name );


	[n_paint_global display];


	return FALSE;
}




void
n_paint_layer_bmp_alpha( int mode )
{

	n_paint *paint = n_paint_global.paint;

	if ( paint->layer_whole_grab_onoff )
	{

		n_type_int i = 0;
		n_posix_loop
		{

			if ( paint->layer_data[ i ].visible )
			{

				if ( paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
				{
					if ( mode == N_PAINT_FILTER_ALPHA_CLR )
					{
						n_bmp_alpha_visible( &paint->layer_data[ i ].bmp_data );
					} else
					if ( mode == N_PAINT_FILTER_ALPHA_REV )
					{
						n_bmp_alpha_reverse( &paint->layer_data[ i ].bmp_data );
					}// else
				} else {
					if ( mode == N_PAINT_FILTER_ALPHA_CLR )
					{
						n_bmp_alpha_visible( &paint->layer_data[ i ].bmp_grab );
					} else
					if ( mode == N_PAINT_FILTER_ALPHA_REV )
					{
						n_bmp_alpha_reverse( &paint->layer_data[ i ].bmp_grab );
					}// else
				}
			}

			i++;
			if ( i >= paint->layer_count ) { break; }
		}

	} else
	if ( paint->layer_onoff )
	{

		n_type_int y = paint->layer_index;

		if ( paint->layer_data[ y ].visible )
		{

			if ( paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
			{
				if ( mode == N_PAINT_FILTER_ALPHA_CLR )
				{
					n_bmp_alpha_visible( paint->pen_bmp_data );
				} else
				if ( mode == N_PAINT_FILTER_ALPHA_REV )
				{
					n_bmp_alpha_reverse( paint->pen_bmp_data );
				}// else
			} else {
				if ( mode == N_PAINT_FILTER_ALPHA_CLR )
				{
					n_bmp_alpha_visible( paint->pen_bmp_grab );
				} else
				if ( mode == N_PAINT_FILTER_ALPHA_REV )
				{
					n_bmp_alpha_reverse( paint->pen_bmp_grab );
				}// else
			}
		}

	} else {

		if ( paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			if ( mode == N_PAINT_FILTER_ALPHA_CLR )
			{
				n_bmp_alpha_visible( paint->pen_bmp_data );
			} else
			if ( mode == N_PAINT_FILTER_ALPHA_REV )
			{
				n_bmp_alpha_reverse( paint->pen_bmp_data );
			}// else
		} else {
			if ( mode == N_PAINT_FILTER_ALPHA_CLR )
			{
				n_bmp_alpha_visible( paint->pen_bmp_grab );
			} else
			if ( mode == N_PAINT_FILTER_ALPHA_REV )
			{
				n_bmp_alpha_reverse( paint->pen_bmp_grab );
			}// else
		}

	}


	return;
}




u32
n_paint_layer_blur_pixel( n_bmp *bmp, n_type_gfx x, n_type_gfx y, int blur )
{

	u32 ret;


	if ( blur == 0 )
	{
		n_bmp_ptr_get( bmp, x, y, &ret );
	} else
	if ( blur == 1 )
	{
		ret = n_bmp_antialias_pixel( bmp, x, y, 1.0 );
	} else {
		ret = n_bmp_blur_pixel( bmp, x, y, blur, 1, 1.0 );
	}


	return ret;
}

u32
n_paint_layer_blur_pixel_fast( n_bmp *bmp, n_type_gfx x, n_type_gfx y, int blur )
{

	u32 ret;


	if ( blur == 0 )
	{
		n_bmp_ptr_get_fast( bmp, x, y, &ret );
	} else
	if ( blur == 1 )
	{
		ret = n_bmp_antialias_pixel( bmp, x, y, 1.0 );
	} else {
		ret = n_bmp_blur_pixel( bmp, x, y, blur, 1, 1.0 );
	}


	return ret;
}

#define n_paint_layer_blur_pixel_cached(      p, i, x,y ) n_paint_layer_blur_pixel_cached_internal( p, i, x,y,  NO )
#define n_paint_layer_blur_pixel_cached_fast( p, i, x,y ) n_paint_layer_blur_pixel_cached_internal( p, i, x,y, YES )

// internal
u32
n_paint_layer_blur_pixel_cached_internal( n_paint *p, n_type_int index, n_type_gfx x, n_type_gfx y, BOOL is_fast )
{

	u32 ret;


	int blur = p->layer_data[ index ].blur;

	if ( blur == 0 )
	{
		if ( is_fast )
		{
			n_bmp_ptr_get_fast( &p->layer_data[ index ].bmp_data, x, y, &ret );
		} else {
			n_bmp_ptr_get     ( &p->layer_data[ index ].bmp_data, x, y, &ret );
		}
	} else {
		u32          chk = 0;
		n_posix_bool err = n_bmp_ptr_get( &p->layer_data[ index ].bmp_blur, x,y, &chk );

		if ( err )
		{
			if ( blur == 1 )
			{
				ret = n_bmp_antialias_pixel( &p->layer_data[ index ].bmp_data, x, y, 1.0 );
			} else {
				ret = n_bmp_blur_pixel( &p->layer_data[ index ].bmp_data, x, y, blur, 1, 1.0 );
			}
		} else
		if ( chk == 0 )
		{
			if ( blur == 1 )
			{
				ret = n_bmp_antialias_pixel( &p->layer_data[ index ].bmp_data, x, y, 1.0 );
			} else {
				ret = n_bmp_blur_pixel( &p->layer_data[ index ].bmp_data, x, y, blur, 1, 1.0 );
			}
//ret = n_bmp_rgb_mac( 0,200,255 );

			n_bmp_ptr_set_fast( &p->layer_data[ index ].bmp_blur, x,y, ret );
		} else {
			ret = chk;
//ret = n_bmp_rgb_mac( 0,200,255 );
		}
	}


	return ret;
}

void
n_paint_layer_blur_cache( void )
{
//NSLog( @"n_paint_layer_blur_cache" );

	n_paint *paint = n_paint_global.paint;


	n_type_int index = paint->layer_index;


	n_bmp *bmp = &paint->layer_data[ index ].bmp_blur;


	u32 timeout = n_posix_tickcount() + 5000;


	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );


#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD


	// [x] : I know that this is not accurate in multi-threading

	n_type_int __block processed_count_max = sx * sy;
	n_type_int __block processed_count     = 0;


	n_type_gfx cores = n_posix_cpu_count();

	n_type_gfx i = 0;
	n_posix_loop
	{

#ifdef N_PAINT_CANVAS_MULTITHREAD

		// [!] : multi-thread

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD

		n_posix_loop
		{
			n_type_gfx x = n_game_random( sx );
			n_type_gfx y = n_game_random( sy );

			n_paint_layer_blur_pixel_cached_fast( paint, index, x,y );

			processed_count++;
			if ( processed_count >= processed_count_max ) { break; }

			if ( timeout < n_posix_tickcount() ) { break; }
		}

#ifdef N_PAINT_CANVAS_MULTITHREAD

		// [!] : multi-thread

		}];
		[paint->queue addOperation:o];

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD

		i++;
		if ( i >= cores ) { break; }
	}


#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	[paint->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD


//NSLog( @"%lld / %lld", processed_count, processed_count_max );


	return;
}

// internal
void
n_paint_layer_grabber_pixel_get( n_bmp *grab, n_type_gfx tx, n_type_gfx ty, u32 picked, u32 *before, u32 *after, n_type_real blend, n_type_int i )
{

	// [!] : this module is based on n_paint_grabber_pixel_get()


	n_paint *paint = n_paint_global.paint;


	u32 color_b, color_a;


	// [x] : don't use _fast()

	if ( n_paint_global.paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{

		color_b = n_bmp_antialias_pixel( paint->pen_bmp_data, tx,ty, blend );
		color_a = picked;

	} else {

		n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( &x,&y, &sx,&sy, NULL,NULL );


		n_type_gfx gx = tx - x;
		n_type_gfx gy = ty - y;

		if ( n_bmp_ptr_is_accessible( grab, gx,gy ) )
		{
			color_b = n_paint_layer_blur_pixel_fast( &paint->layer_data[ i ].bmp_grab, gx, gy, paint->layer_data[ i ].blur );
		} else {
			color_b = n_paint_layer_blur_pixel     ( &paint->layer_data[ i ].bmp_data, tx, ty, paint->layer_data[ i ].blur );
		}


		// [!] : Eraser

		if ( N_BMP_ALPHA_CHANNEL_INVISIBLE == n_bmp_a( picked ) )
		{
			n_bmp_ptr_get( paint->pen_bmp_data, tx,ty, &color_a );
		} else {
			color_a = picked;
		}

	}


	if ( before != NULL ) { (*before) = color_b; }
	if ( after  != NULL ) { (*after ) = color_a; }


	return;
}

u32
n_paint_layer_canvas_grabber_pixel( n_bmp *grab, n_type_gfx tx, n_type_gfx ty, u32 color, n_type_int i )
{

	// [!] : this module is based on n_paint_canvas_grabber_pixel()


	n_paint *paint = n_paint_global.paint;

 
	u32 color_grab; n_paint_layer_grabber_pixel_get( grab, tx, ty, 0, &color_grab, NULL, 0.0, i );


	if ( paint->grabber_per_pixel_alpha_onoff )
	{

		n_type_gfx x,y; n_paint_grabber_system_get( &x,&y, NULL,NULL, NULL,NULL );

		n_bmp *data = &paint->layer_data[ i ].bmp_data;

		n_type_gfx gx = tx - x;
		n_type_gfx gy = ty - y;

		if (
			( n_bmp_ptr_is_accessible( grab, gx,gy ) )
			&&
			( n_bmp_ptr_is_accessible( data, tx,ty ) )
		)
		{
			color_grab = n_bmp_composite_pixel_fast( grab, data, gx,gy, tx,ty, n_posix_false,n_posix_false, n_posix_true, n_posix_true, 0.0, NULL );
		}
	}


	if ( paint->grabber_blend )
	{
		color_grab = n_bmp_blend_pixel( color_grab, color, paint->grabber_blend_ratio );
	}


	return color_grab;
}

u32
n_bmp_layer_ptr_get( n_paint_layer *p, n_type_gfx tx, n_type_gfx ty, n_posix_bool use_color_bg, u32 color_bg, n_posix_bool is_ui )
{

	n_paint *paint = n_paint_global.paint;


	u32 ret = n_bmp_white_invisible;
//return n_bmp_rgb( 0,200,255 );

	if ( n_posix_false == n_bmp_ptr_is_accessible( &p[ 0 ].bmp_data, tx, ty ) ) { return ret; }


	u32 color_prev;
	if (1)//( n_paint_layer_is_locked( 0 ) )
	{
		color_prev = n_paint_layer_blur_pixel_cached_fast( paint, 0, tx,ty );
	} else {
		color_prev = n_paint_layer_blur_pixel_fast( &p[ 0 ].bmp_data, tx, ty, p[ 0 ].blur );
	}

	if ( ( use_color_bg )&&( N_BMP_ALPHA_CHANNEL_INVISIBLE == n_bmp_a( color_prev ) ) )
	{
		ret = color_prev = color_bg;
//return n_bmp_rgb( 0,200,255 );
	} else {
		ret = color_prev;
		if ( n_posix_false == p[ 0 ].visible ) { color_prev = color_bg; }
//return n_bmp_rgb( 255,0,200 );
	}


	// [x] : don't use _fast() : crash in some cases

	u32 check = 0;
	if ( is_ui )
	{
		n_bmp_ptr_get( &paint->layer_cache_bmp_data, tx,ty, &check );
	}

	if ( check ) { return check; }


	n_type_int     i = 0;
	n_type_int count = paint->layer_count;


	BOOL cache_onoff = is_ui;
	if ( paint->layer_whole_grab_onoff ) { cache_onoff = FALSE; }

//NSLog( @"%d %d", ( NULL == N_BMP_PTR( &paint->layer_cache_bmp_pre ) ), ( NULL == N_BMP_PTR( &paint->layer_cache_bmp_pst ) ) );

	BOOL cache_onoff_pre = cache_onoff;
	BOOL cache_onoff_pst = cache_onoff;

	if ( NULL == N_BMP_PTR( &paint->layer_cache_bmp_pre ) )
	{
		cache_onoff_pre = FALSE;
	}

	if ( NULL == N_BMP_PTR( &paint->layer_cache_bmp_pst ) )
	{
		cache_onoff_pst = FALSE;
	}


	if ( cache_onoff_pre )
	{
		n_bmp_ptr_get_fast( &paint->layer_cache_bmp_pre, tx,ty, &check );

		if ( check )
		{
//check = n_bmp_black;
			color_prev = check;

			i = paint->layer_index;
		}
	}

	u32 color_post = 0;


	n_posix_loop
	{//break;

		if ( p[ i ].visible )
		{

			u32 color;

			if ( i == 0 )
			{
				color = color_prev;
			} else {
				if (1)//( n_paint_layer_is_locked( i ) )
				{
					color = n_paint_layer_blur_pixel_cached_fast( paint, i, tx,ty );
				} else {
					color = n_paint_layer_blur_pixel_fast( &p[ i ].bmp_data, tx, ty, p[ i ].blur );
				}
			}

			if ( ( is_ui )&&( paint->grabber_mode ) )
			{
				color = n_paint_layer_canvas_grabber_pixel( &paint->layer_data[ i ].bmp_grab, tx, ty, color, i );
//color = n_bmp_rgb( 0,200,255 );
			}

			ret = n_bmp_composite_pixel_postprocess( color_prev, color, n_bmp_a( color ), n_posix_true, n_posix_true, n_posix_true, 1.0 - p[ i ].blend, NULL );

//if ( n_win_is_input( VK_LBUTTON ) ) { ret = n_bmp_rgb( 0,200,255 ); }

			color_prev = ret;

			if ( ( cache_onoff_pst )&&( i > paint->layer_index ) )
			{
				color_post = n_bmp_composite_pixel_postprocess( color_post, color, n_bmp_a( color_post ), n_posix_true, n_posix_true, n_posix_true, 1.0 - p[ i ].blend, NULL );
			}

		} else
		if ( paint->layer_whole_preview_onoff )
		{

			u32 color;
			if (1)//( n_paint_layer_is_locked( i ) )
			{
				color = n_paint_layer_blur_pixel_cached_fast( paint, i, tx,ty );
			} else {
				color = n_paint_layer_blur_pixel_fast( &p[ i ].bmp_data, tx, ty, p[ i ].blur );
			}
//color = n_bmp_rgb( 0,200,255 );

			ret = n_bmp_composite_pixel_postprocess( color_prev, color, n_bmp_a( color ), n_posix_true, n_posix_true, n_posix_true, 1.0 - p[ i ].blend, NULL );

			color_prev = ret;

		} else {

			ret = color_prev;

		}

		if ( ( cache_onoff_pre )&&( i == ( paint->layer_index - 1 ) ) )
		{
			n_bmp_ptr_set_fast( &paint->layer_cache_bmp_pre, tx,ty, ret );
		}


		i++;

		if ( ( cache_onoff_pst )&&( i > paint->layer_index ) )
		{
			n_bmp_ptr_get_fast( &paint->layer_cache_bmp_pst, tx,ty, &check );
			if ( check )
			{
				ret = n_bmp_composite_pixel_postprocess( color_prev, check, n_bmp_a( color_prev ), n_posix_true, n_posix_true, n_posix_true, 1.0 - p[ i ].blend, NULL );
				break;
			}
		}

		if ( i >= count )
		{
			if ( cache_onoff_pst ) { n_bmp_ptr_set_fast( &paint->layer_cache_bmp_pst, tx,ty, color_post ); }
			break;
		}
	}


	if ( NULL != N_BMP_PTR( &paint->layer_cache_bmp_data ) )
	{
		n_bmp_ptr_set_fast( &paint->layer_cache_bmp_data, tx,ty, ret );
	}


	return ret;
}




#define n_paint_layer_swap_up(   y ) n_paint_layer_swap( y, n_posix_true  )
#define n_paint_layer_swap_down( y ) n_paint_layer_swap( y, n_posix_false )

// internal
BOOL
n_paint_layer_swap( n_type_int y, n_posix_bool is_up )
{

	n_paint *paint = n_paint_global.paint;


	BOOL ret = FALSE;

	if ( is_up )
	{
		if ( y <= 0 ) { return ret; }

		n_paint_layer     data_tmp = paint->layer_data[ y - 1 ];
		paint->layer_data[ y - 1 ] = paint->layer_data[ y     ];
		paint->layer_data[ y     ] = data_tmp;

		n_posix_char         *line_tmp = paint->layer_txt.line[ y - 1 ];
		paint->layer_txt.line[ y - 1 ] = paint->layer_txt.line[ y     ];
		paint->layer_txt.line[ y     ] = line_tmp;

		n_posix_char                    *deco_tmp = paint->layer_decoration_txt.line[ y - 1 ];
		paint->layer_decoration_txt.line[ y - 1 ] = paint->layer_decoration_txt.line[ y     ];
		paint->layer_decoration_txt.line[ y     ] = deco_tmp;

		y--;
		paint->layer_index--;

		ret = TRUE;
	} else {
		if ( y >= ( paint->layer_count - 1 ) ) { return ret; }

		n_paint_layer     data_tmp = paint->layer_data[ y + 1 ];
		paint->layer_data[ y + 1 ] = paint->layer_data[ y     ];
		paint->layer_data[ y     ] = data_tmp;

		n_posix_char         *line_tmp = paint->layer_txt.line[ y + 1 ];
		paint->layer_txt.line[ y + 1 ] = paint->layer_txt.line[ y     ];
		paint->layer_txt.line[ y     ] = line_tmp;

		n_posix_char                    *deco_tmp = paint->layer_decoration_txt.line[ y + 1 ];
		paint->layer_decoration_txt.line[ y + 1 ] = paint->layer_decoration_txt.line[ y     ];
		paint->layer_decoration_txt.line[ y     ] = deco_tmp;

		y++;
		paint->layer_index++;

		ret = TRUE;
	}


	paint->pen_bmp_data = &paint->layer_data[ y ].bmp_data;
	paint->pen_bmp_grab = &paint->layer_data[ y ].bmp_grab;


	[n_paint_global display_optimized];


	return ret;
}




void
n_paint_layer_visibility_onoff_all( BOOL onoff )
{

	n_paint *paint = n_paint_global.paint;


	n_type_int i = 0;
	n_posix_loop
	{

		n_posix_char *nam = n_txt_get( &paint->layer_txt, i );

		if ( onoff )
		{
			paint->layer_data[ i ].visible = n_posix_true;
			n_paint_layer_text_mod( paint, i, nam );
		} else {
			paint->layer_data[ i ].visible = n_posix_false;
			n_paint_layer_text_mod( paint, i, nam );
		}


		i++;
		if ( i >= paint->layer_count ) { break; }
	}

	[n_paint_global.txtbox display];

	[n_paint_global display_optimized];


	return;
}




void
n_bmp_layer_raw_free( n_paint_layer *p, n_type_int count )
{

	if ( p == NULL ) { return; }

	n_type_int i = 0;
	n_posix_loop
	{

		n_bmp_free( &p[ i ].bmp_data );
		n_bmp_free( &p[ i ].bmp_grab );

		n_bmp_free( &p[ i ].bmp_blur );

		i++;
		if ( i >= count ) { break; }
	}

	n_memory_free( p );


	return;
}

n_paint_layer*
n_bmp_layer_raw_copy( n_paint_layer *f, n_type_int count )
{

	n_paint_layer *t = n_memory_new( sizeof( n_paint_layer ) * count );

	n_memory_copy( f, t, sizeof( n_paint_layer ) * count );

#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	n_paint *paint = n_paint_global.paint;

	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD

	n_type_int i = 0;
	n_posix_loop
	{//break;

#ifdef N_PAINT_CANVAS_MULTITHREAD

		// [!] : multi-thread

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD

		n_bmp_carboncopy( &f[ i ].bmp_data, &t[ i ].bmp_data );
		n_bmp_carboncopy( &f[ i ].bmp_grab, &t[ i ].bmp_grab );

#ifdef N_PAINT_CANVAS_MULTITHREAD

		// [!] : multi-thread

		}];
		[paint->queue addOperation:o];

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD

		i++;
		if ( i >= count ) { break; }
	}


#ifdef N_PAINT_CANVAS_MULTITHREAD

	// [!] : multi-thread

	[paint->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;

#endif // #ifdef N_PAINT_CANVAS_MULTITHREAD


	return t;
}




void
n_paint_layer_info( void )
{
//NSLog( @"n_paint_layer_info()" );


	n_paint *p = n_paint_global.paint;


	if ( p->layer_onoff == FALSE ) { return; }


	int byte = p->layer_count * sizeof( int );
	int *ptr = n_memory_new( byte );


	NSPoint    pt = [n_paint_global n_paint_point_on_bitmap];
	n_type_gfx tx = pt.x;
	n_type_gfx ty = pt.y;


	n_type_int i = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get( &p->layer_data[ i ].bmp_data, tx, ty, &color );
//NSLog( @"#%lld : %08x", i, color );

		ptr[ i ] = n_bmp_a( color );

		i++;
		if ( i >= p->layer_count ) { break; }
	}


	i = 0;
	n_posix_loop
	{//break;

		n_posix_char *str = n_txt_get( &p->layer_decoration_txt, i );

		n_posix_char deco[ 3 ] = { str[ 0 ], 0, 0 };

		if ( N_BMP_ALPHA_CHANNEL_INVISIBLE != ptr[ i ] )
		{
			deco[ 1 ] = '!';
		} else {
			deco[ 1 ] = 0;
		}

		n_txt_mod( &p->layer_decoration_txt, i, deco );

		i++;
		if ( i >= p->layer_count ) { break; }
	}

	[n_layer_listbox_global display];

/*
	n_posix_char str[ 1024 ]; n_string_zero( str, 1024 );
	n_type_int   cat = 0;

	i = 0;
	n_posix_loop
	{
//NSLog( @"#%lld : %d", i, ptr[ i ] );

		if ( N_BMP_ALPHA_CHANNEL_INVISIBLE != ptr[ i ] )
		{
			cat += n_posix_sprintf_literal( &str[ cat ], "%lld ", i );
		}

		i++;
		if ( i >= p->layer_count ) { break; }
	}
//NSLog( @"%s", str );

	n_mac_window_dialog_ok( str );
*/

	n_memory_free( ptr );


	return;
}


