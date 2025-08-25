// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_PAINT_COLORHISTORY_OFFSET (  2 )
#define N_PAINT_COLORHISTORY_COUNT  ( 10 )

#define N_PAINT_COLORHISTORY_SIZE   ( 250 )




static n_bmp n_paint_colorhistory_nbmp;
static u32   n_paint_colorhistory_color[ N_PAINT_COLORHISTORY_COUNT ];




void
n_paint_colorhistory_init( void )
{
//NSLog( @"n_paint_colorhistory_init()" );

	n_paint_colorhistory_color[ 0 ] = n_bmp_black;
	n_paint_colorhistory_color[ 1 ] = n_bmp_white;
	n_paint_colorhistory_color[ 2 ] = n_bmp_rgb(   0,200,255 );
	n_paint_colorhistory_color[ 3 ] = n_bmp_rgb( 255,  0,200 );
	n_paint_colorhistory_color[ 4 ] = n_bmp_rgb( 200,255,  0 );
	n_paint_colorhistory_color[ 5 ] = n_bmp_rgb(   0,255,200 );
	n_paint_colorhistory_color[ 6 ] = n_bmp_rgb( 200,  0,255 );
	n_paint_colorhistory_color[ 7 ] = n_bmp_rgb( 255,200,  0 );
	n_paint_colorhistory_color[ 8 ] = n_bmp_rgb(   0,200,255 );
	n_paint_colorhistory_color[ 9 ] = n_bmp_rgb(   0,200,255 );


	return;
}

void
n_paint_colorhistory_add( u32 color )
{

	color = n_bmp_color_mac( color );

//NSLog( @"n_paint_colorhistory_add() : %x", color );

	int i = 0;
	n_posix_loop
	{

		if ( n_paint_colorhistory_color[ i ] == color ) { return; }


		i++;
		if ( i >= N_PAINT_COLORHISTORY_COUNT ) { break; }
	}


	memmove
	(
		&n_paint_colorhistory_color[ N_PAINT_COLORHISTORY_OFFSET + 1 ],
		&n_paint_colorhistory_color[ N_PAINT_COLORHISTORY_OFFSET + 0 ],
		sizeof( u32 ) * ( N_PAINT_COLORHISTORY_COUNT - 1 - N_PAINT_COLORHISTORY_OFFSET )
	);

	n_paint_colorhistory_color[ N_PAINT_COLORHISTORY_OFFSET ] = color;


//NSLog( @"!" );
	return;
}

void
n_paint_colorhistory_text( n_bmp *bmp, n_type_gfx sx, n_type_gfx sy, n_type_gfx ctl, u32 color )
{

	n_gdi gdi; n_gdi_zero( &gdi );

	gdi.sx                  = sx;
	gdi.sy                  = ctl;


	int a = n_bmp_a( color );
	int r = n_bmp_r( color );
	int g = n_bmp_g( color );
	int b = n_bmp_b( color );


	n_posix_char str[ 100 ]; n_posix_sprintf_literal( str, "%d %d %d %d", a, r, g, b );

	gdi.text                = str;
	gdi.text_font           = "Trebuchet MS";
	gdi.text_size           = 14;
	gdi.text_style          = N_GDI_TEXT_SMOOTH | N_GDI_TEXT_CONTOUR;
	gdi.text_color_main     = n_bmp_white;
	gdi.text_color_contour  = n_bmp_black;
	gdi.text_fxsize2        = 1;


	n_bmp_free( bmp );

	n_gdi_bmp( &gdi, bmp );


	return;
}




@interface NonnonPaintColorHistory : NSView

@end


@implementation NonnonPaintColorHistory

- (instancetype)initWithCoder:(NSCoder *)coder
{
	self = [super initWithCoder:coder];

	if ( self )
	{
		n_type_gfx csx = N_PAINT_COLORHISTORY_SIZE;
		n_type_gfx csy = N_PAINT_COLORHISTORY_SIZE;

		n_paint_colorhistory_init();
		n_bmp_new( &n_paint_colorhistory_nbmp, csx,csy );
	}

	return self;
}

-(void) drawRect:(NSRect) rect
{

	// [x] : Sonoma : problem happens

	rect = [super bounds];


	n_type_gfx csx = N_PAINT_COLORHISTORY_SIZE;
	n_type_gfx csy = N_PAINT_COLORHISTORY_SIZE;


	n_type_gfx ctl = ( N_PAINT_COLORHISTORY_SIZE / 10 );
	n_type_gfx rst = ( N_PAINT_COLORHISTORY_SIZE % 10 );


	n_bmp bmp_main; n_bmp_zero( &bmp_main ); n_bmp_new( &bmp_main, csx,csy );


	n_type_gfx tsx = ( csx / 5 ) + ( csx % 5 );
	n_type_gfx tsy = ctl;

	int i = 0;
	n_posix_loop
	{

		u32 color   = n_paint_colorhistory_color[ i ];// NSLog( @"%x", color );
		u32 color_0 = n_bmp_gamma_pixel( color, 0.5 );
		u32 color_1 = n_bmp_gamma_pixel( color, 1.0 );
		u32 color_2 = n_bmp_gamma_pixel( color, 1.5 );

		color_0 = n_bmp_color_mac( color_0 );
		color_1 = n_bmp_color_mac( color_1 );
		color_2 = n_bmp_color_mac( color_2 );

		if ( i == ( N_PAINT_COLORHISTORY_COUNT - 1 ) ) { tsy += rst; }

		n_type_gfx x = 0;
		n_bmp_box( &n_paint_colorhistory_nbmp, x, ctl * i, tsx * 1, tsy, color_0 ); x += tsx;
		n_bmp_box( &n_paint_colorhistory_nbmp, x, ctl * i, tsx * 3, tsy, color_1 ); x += tsx * 3;
		n_bmp_box( &n_paint_colorhistory_nbmp, x, ctl * i, tsx * 1, tsy, color_2 ); //x += tsx;

		n_bmp_transcopy( &n_paint_colorhistory_nbmp, &bmp_main, 0,ctl * i,csx,tsy, 0,ctl * i );


		n_bmp bmp; n_bmp_zero( &bmp );

		n_paint_colorhistory_text( &bmp, csx,csy, ctl, color );
//n_bmp_save_literal( &bmp, "ret.bmp" );
		n_bmp_transcopy( &bmp, &bmp_main, 0,0,csx,ctl, 0,ctl * i );

		n_bmp_free_fast( &bmp );


		i++;
		if ( i >= N_PAINT_COLORHISTORY_COUNT ) { break; }
	}


	n_mac_image_nbmp_direct_draw( &bmp_main, &rect, YES );

	n_bmp_free_fast( &bmp_main );

}




- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog( @"mouseDown" );

	NSPoint pt = n_mac_cursor_position_get( self );

	u32 color; n_bmp_ptr_get( &n_paint_colorhistory_nbmp, pt.x, 256 - pt.y, &color );

//NSLog( @"Color : %d %d %d", n_bmp_r( color ), n_bmp_g( color ), n_bmp_b( color ) );

	[n_paint_global n_paint_picked_color_set:color];

	n_mac_window_hide( [self window] );

}


@end

