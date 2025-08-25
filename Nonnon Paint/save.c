// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




void
n_paint_layer_save_as_one( n_paint *paint, n_bmp *bmp_ret )
{

	// [!] : performance
	//
	//	non-threaded : 4903 msec.
	//	    threaded :  860 msec.

//u32 tick = n_posix_tickcount();


	u32 color_bg = n_bmp_white_invisible;

	n_type_gfx bmpsx = N_BMP_SX( &paint->layer_data[ 0 ].bmp_data );
	n_type_gfx bmpsy = N_BMP_SY( &paint->layer_data[ 0 ].bmp_data );

	n_bmp_new( bmp_ret, bmpsx, bmpsy );


	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;

	n_type_gfx y = 0;
	n_posix_loop
	{

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_type_gfx x = 0;
		n_posix_loop
		{

			u32 color = n_bmp_layer_ptr_get( paint->layer_data, x, y, n_posix_true, color_bg, n_posix_false );
			n_bmp_ptr_set_fast( bmp_ret, x, y, color );

			x++;
			if ( x >= bmpsx ) { break; }
		}

		}];
		[paint->queue addOperation:o];

		y++;
		if ( y >= bmpsy ) { break; }
	}


	[paint->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;


//NSLog( @"%d", n_posix_tickcount() - tick );


	return;
}

void
n_paint_save_carboncopy( n_bmp *bmp, n_bmp *bmp_ret )
{

	if ( n_paint_global.paint->layer_onoff )
	{
		n_paint *paint = n_paint_global.paint;

		if ( paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			n_paint_layer_save_as_one( paint, bmp_ret );
		} else {
			n_type_gfx x,y,sx,sy; n_paint_grabber_system_get( NULL, NULL, &sx, &sy, &x, &y );
			n_bmp bmp; n_bmp_zero( &bmp ); 
			n_paint_layer_save_as_one( paint, &bmp );

			n_bmp_new_fast( bmp_ret, sx, sy );
			n_bmp_fastcopy( &bmp, bmp_ret, x,y,sx,sy, 0,0 );

			n_bmp_free( &bmp );
		}
	} else {
		n_bmp_carboncopy( bmp, bmp_ret );
	}


	return;
}

n_posix_bool
n_paint_save_png( n_bmp *bmp, n_posix_char *path, n_posix_bool n_bmp_carboncopy_onoff )
{

	n_posix_bool ret = n_posix_true;


	n_bmp tmp; n_bmp_zero( &tmp );


	if ( n_bmp_carboncopy_onoff )
	{
		n_bmp_carboncopy       ( bmp, &tmp );
	} else {
		n_paint_save_carboncopy( bmp, &tmp );
	}


	// [x] : not-the-same image is saved

	//n_bmp_flush_mirror( &tmp, N_BMP_MIRROR_UPSIDE_DOWN );

	//ret = n_mac_image_save( &tmp, path, N_MAC_IMAGE_SAVE_PNG );


	n_bmp_mac_color( &tmp );

	n_png png = n_png_template;

	ret  = n_png_compress( &png, &tmp );
	ret += n_png_save( &png, path );

	n_png_free( &png );


	n_bmp_free_fast( &tmp );


	return ret;
}

n_posix_bool
n_paint_save( n_posix_char *path, n_bmp *bmp, n_curico *curico )
{

	n_posix_bool ret = n_posix_true;

//n_string_path_ext_mod( ".bmp\0\0", path );

	if ( n_string_path_ext_is_same_literal( ".BMP\0\0", path ) )
	{

		n_bmp tmp; n_bmp_zero( &tmp ); n_paint_save_carboncopy( bmp, &tmp );

		n_bmp_mac_color( &tmp );
		ret = n_bmp_save( &tmp, path );

		n_bmp_free_fast( &tmp );

	} else
	if ( n_string_path_ext_is_same_literal( ".ICO\0\0", path ) )
	{

		n_bmp tmp; n_bmp_zero( &tmp ); n_paint_save_carboncopy( bmp, &tmp );

		n_bmp_mac_color( &tmp );

		curico->type = N_CURICO_TYPE_ICO;

		ret = n_curico_save( curico, &tmp, path );

		n_bmp_free_fast( &tmp );

	} else
	if ( n_string_path_ext_is_same_literal( ".CUR\0\0", path ) )
	{

		n_bmp tmp; n_bmp_zero( &tmp ); n_paint_save_carboncopy( bmp, &tmp );

		n_bmp_mac_color( &tmp );

		curico->type = N_CURICO_TYPE_CUR;

		ret = n_curico_save( curico, &tmp, path );

		n_bmp_free_fast( &tmp );

	} else
	if ( n_string_path_ext_is_same_literal( ".JPG\0\0", path ) )
	{

		n_bmp tmp; n_bmp_zero( &tmp ); n_paint_save_carboncopy( bmp, &tmp );

		// [!] : crash

		//n_jpg_quality = 100;
		//ret = n_jpg_bmp2jpg( path, &tmp );

		n_bmp_flush_mirror( &tmp, N_BMP_MIRROR_UPSIDE_DOWN );

		ret = n_mac_image_save( &tmp, path, N_MAC_IMAGE_SAVE_JPG );

		n_bmp_free_fast( &tmp );

	} else
	if ( n_string_path_ext_is_same_literal( ".PNG\0\0", path ) )
	{

		ret = n_paint_save_png( bmp, path, n_posix_false );

	} else
	if ( n_string_path_ext_is_same_literal( ".LYR\0\0", path ) )
	{
//NSLog( @"Layer" );

		n_paint *p = n_paint_global.paint;

		n_posix_char *str = n_string_path_carboncopy( path );
		n_string_path_ext_del( str );
//NSLog( @"%s", str );

		ret = n_paint_layer_save( p, str );

		n_string_path_free( str );

	} else {
	
		// [!] : default

		n_posix_char *default_name = n_string_path_carboncopy( path );
		n_string_path_ext_mod_literal( ".png\0\0", default_name );

		ret = n_paint_save_png( bmp, default_name, n_posix_false );

		n_string_path_free( default_name );

	}


	return ret;
}


