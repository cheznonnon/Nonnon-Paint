//
//  AppDelegate.m
//  Nonnon Paint
//
//  Created by のんのん on 2022/09/13.
//

#import "AppDelegate.h"



#include "../nonnon/neutral/bmp/all.c"
#include "../nonnon/neutral/curico.c"
#include "../nonnon/neutral/jpg.c"
#include "../nonnon/neutral/png.c"


#include "../nonnon/mac/_mac.c"
#include "../nonnon/mac/draw.c"
#include "../nonnon/mac/image.c"
#include "../nonnon/mac/window.c"

#include "../nonnon/mac/n_button.c"
#include "../nonnon/mac/n_scrollbar.c"
#include "../nonnon/mac/n_txtbox.c"

#include "../nonnon/project/ini2gdi.c"




#define N_PAINT_LIMIT_SIZE ( 20000 )//N_BMP_LIMIT_SIZE

void
n_paint_memory_limit( void )
{
//NSLog( @"Nonnon Paint : Memory Error" );

	n_mac_window_dialog_ok( "Nonnon Paint : Memory is insufficient" );


	return;
}




#define N_PAINT_ID_PENSIZE (  1 )
#define N_PAINT_ID_MIX     (  2 )
#define N_PAINT_ID_BOOST   (  3 )
#define N_PAINT_ID_AIR     (  4 )
#define N_PAINT_ID_ZOOM    (  5 )
#define N_PAINT_ID_COLOR_A (  6 )
#define N_PAINT_ID_COLOR_R (  7 )
#define N_PAINT_ID_COLOR_G (  8 )
#define N_PAINT_ID_COLOR_B (  9 )
#define N_PAINT_ID_BLEND   ( 10 )

#define N_PAINT_ID_RESIZER_ROTATE     ( 100 )
#define N_PAINT_ID_RESIZER_GAMMA      ( 101 )
#define N_PAINT_ID_RESIZER_COLORWHEEL ( 102 )
#define N_PAINT_ID_RESIZER_VIVIDNESS  ( 103 )
#define N_PAINT_ID_RESIZER_SHARPNESS  ( 104 )
#define N_PAINT_ID_RESIZER_CONTRAST   ( 105 )

#define N_PAINT_ID_LAYER_BLUR         ( 200 )
#define N_PAINT_ID_LAYER_BLEND        ( 201 )

#define N_PAINT_ID_CLEAR_A ( 300 )
#define N_PAINT_ID_CLEAR_R ( 301 )
#define N_PAINT_ID_CLEAR_G ( 302 )
#define N_PAINT_ID_CLEAR_B ( 303 )

#define N_PAINT_ID_REPLACER_FR_A ( 400 )
#define N_PAINT_ID_REPLACER_FR_R ( 401 )
#define N_PAINT_ID_REPLACER_FR_G ( 402 )
#define N_PAINT_ID_REPLACER_FR_B ( 403 )
#define N_PAINT_ID_REPLACER_TO_A ( 404 )
#define N_PAINT_ID_REPLACER_TO_R ( 405 )
#define N_PAINT_ID_REPLACER_TO_G ( 406 )
#define N_PAINT_ID_REPLACER_TO_B ( 407 )

#define N_PAINT_ID_FORMATTER_CUR_X ( 500 )
#define N_PAINT_ID_FORMATTER_CUR_Y ( 501 )




n_posix_bool
n_paint_load_ini2gdi( n_posix_char *path, n_bmp *bmp )
{

	n_posix_bool ret = n_ini2gdi_load( path, bmp );
/*
	if ( ret == n_posix_false )
	{
		n_string_path_ext_mod_literal( ".png", path );
	}
*/
	return ret;
}

n_posix_bool
n_paint_load( n_posix_char *path, n_bmp *bmp, n_curico *curico )
{

	if ( n_posix_stat_is_dir( path ) ) { return n_posix_true; }

	if (
		( n_png_png2bmp( path, bmp ) )
		&&
		( n_jpg_jpg2bmp( path, bmp ) )
		&&
		( n_curico_load( curico, bmp, path ) )
		&&
		( n_bmp_load( bmp, path ) )
		&&
		( n_paint_load_ini2gdi( path, bmp ) )
		&&
		( n_mac_image_load( path, bmp ) )
	)
	{
		return n_posix_true;
	} else {
		n_bmp_mac_color( bmp );
	}

// [!] : for debugging
//n_bmp_scaler_big( bmp, 2 );


	return n_posix_false;
}

n_posix_bool
n_mac_window_is_hovered_by_nwwindow( NSWindow *window )
{

	NSPoint pt = [ NSEvent mouseLocation ];
//NSLog( @"%f %f %f", globalLocation.x, [window frame].origin.x, [window frame].origin.x + [window frame].size.width );

	return NSPointInRect( pt, [window frame] );
}




NonnonTxtbox *n_layer_listbox_global = NULL;




#include "_bmp.c"
#include "_hsl.c"

#include "canvas.c"
#include "save.c"

#include "key_redirector.c"

#include "xmouse.c"

#include "preview.c"

#include "colorhistory.c"




@interface NonnonPaintColorPreview : NSView

@property u32 *color;

@end


@implementation NonnonPaintColorPreview

@synthesize color;

- (instancetype)initWithCoder:(NSCoder *)coder
{
	self = [super initWithCoder:coder];

	if ( self )
	{
		color = NULL;
	}

	return self;
}

-(void) drawRect:(NSRect) dirtyRect
{
	// [x] : color will be "bg" when alpha is zero

	//NSColor *bg = [NSColor windowFrameColor];
	//n_mac_draw_roundrect( bg, rect, 4 );

	//const CGFloat border = 1;


	// [x] : macOS Sonoma : dirtyRect has parent window size at the first time

	NSRect rect_inner = [super bounds];

	//rect_inner.origin.x    += border;
	//rect_inner.origin.y    += border;
	//rect_inner.size.width  -= border * 2;
	//rect_inner.size.height -= border * 2;

	NSColor *clr = n_mac_argb2nscolor( n_bmp_color_mac( *color ) );
	n_mac_draw_roundrect( clr, rect_inner, 4 );
}

@end




@interface AppDelegate ()


@property (strong) IBOutlet NSWindow *window;

@property (strong) IBOutlet NonnonPaintStub *n_window_stub;
@property (strong) IBOutlet NonnonPaintStub *n_tool_stub;
@property (strong) IBOutlet NonnonPaintStub *n_layer_stub;
@property (strong) IBOutlet NonnonPaintStub *n_resizer_stub;
@property (strong) IBOutlet NonnonPaintStub *n_formatter_bmp_stub;
@property (strong) IBOutlet NonnonPaintStub *n_formatter_cur_stub;
@property (strong) IBOutlet NonnonPaintStub *n_clear_stub;
@property (strong) IBOutlet NonnonPaintStub *n_replacer_stub;
@property (strong) IBOutlet NonnonPaintStub *n_alpha_tweaker_stub;
@property (strong) IBOutlet NonnonPaintStub *n_preview_stub;
@property (strong) IBOutlet NonnonPaintStub *n_renamer_stub;
@property (strong) IBOutlet NonnonPaintStub *n_colorhistory_stub;




// Tool Window

@property (weak) IBOutlet NonnonPaintCanvas *n_paint_canvas;

@property (weak) IBOutlet NSWindow        *n_toolwindow;

@property (weak) IBOutlet NSTextField     *n_size_value;
@property (weak) IBOutlet NonnonScrollbar *n_size_scrollbar;

@property (weak) IBOutlet NSTextField     *n_mix_value;
@property (weak) IBOutlet NonnonScrollbar *n_mix_scrollbar;

@property (weak) IBOutlet NSTextField     *n_boost_value;
@property (weak) IBOutlet NonnonScrollbar *n_boost_scrollbar;

@property (weak) IBOutlet NSTextField     *n_air_value;
@property (weak) IBOutlet NonnonScrollbar *n_air_scrollbar;

@property (weak) IBOutlet NSTextField     *n_zoom_value;
@property (weak) IBOutlet NonnonScrollbar *n_zoom_scrollbar;

@property (weak) IBOutlet NonnonPaintColorPreview *n_color_preview;

@property (weak) IBOutlet NSTextField     *n_color_a_value;
@property (weak) IBOutlet NonnonScrollbar *n_color_a_scrollbar;

@property (weak) IBOutlet NSTextField     *n_color_r_value;
@property (weak) IBOutlet NonnonScrollbar *n_color_r_scrollbar;

@property (weak) IBOutlet NSTextField     *n_color_g_value;
@property (weak) IBOutlet NonnonScrollbar *n_color_g_scrollbar;

@property (weak) IBOutlet NSTextField     *n_color_b_value;
@property (weak) IBOutlet NonnonScrollbar *n_color_b_scrollbar;

@property (weak) IBOutlet NonnonButton    *n_button_0_0;
@property (weak) IBOutlet NonnonButton    *n_button_1_0;
@property (weak) IBOutlet NonnonButton    *n_button_2_0;
@property (weak) IBOutlet NonnonButton    *n_button_3_0;
@property (weak) IBOutlet NonnonButton    *n_button_4_0;

@property (weak) IBOutlet NonnonButton    *n_button_3_1;
@property (weak) IBOutlet NonnonButton    *n_button_4_1;

@property (weak) IBOutlet NonnonButton    *n_button_0_2;
@property (weak) IBOutlet NonnonButton    *n_button_1_2;
@property (weak) IBOutlet NonnonButton    *n_button_2_2;
@property (weak) IBOutlet NonnonButton    *n_button_4_2;

@property (weak) IBOutlet NSButton        *n_ppa_check;

@property (weak) IBOutlet NSTextField     *n_grabber_blend_label;
@property (weak) IBOutlet NSTextField     *n_grabber_blend_value;
@property (weak) IBOutlet NonnonScrollbar *n_grabber_blend_scrollbar;
@property (weak) IBOutlet NSTextField     *n_grabber_frame_mode_label;
@property (weak) IBOutlet NSComboBox      *n_grabber_frame_mode_combo;

@property (weak) IBOutlet NSTextField     *n_status1;
@property (weak) IBOutlet NSTextField     *n_status2;
@property (weak) IBOutlet NSTextField     *n_status3;


// Menu

@property (weak) IBOutlet NSMenuItem *n_menu_grid;
@property (weak) IBOutlet NSMenuItem *n_menu_pixelgrid;
@property (weak) IBOutlet NSMenuItem *n_menu_alpha_emphasizer;

@property (weak) IBOutlet NSMenuItem *n_menu_clear_canvas;
@property (weak) IBOutlet NSMenuItem *n_menu_color_replacer;
@property (weak) IBOutlet NSMenuItem *n_menu_alpha_tweaker;

@property (weak) IBOutlet NSMenuItem *n_menu_about;


// Resizer

@property (weak) IBOutlet NSWindow        *n_resizer_window;
@property (weak) IBOutlet NSComboBox      *n_resizer_combo;
@property (weak) IBOutlet NSTextField     *n_resizer_sx;
@property (weak) IBOutlet NSTextField     *n_resizer_sy;
@property (weak) IBOutlet NSTextField     *n_resizer_rotate_value;
@property (weak) IBOutlet NonnonScrollbar *n_resizer_rotate_scrollbar;
@property (weak) IBOutlet NSComboBox      *n_resizer_color_combobox;

@property (weak) IBOutlet NSTextField     *n_resizer_gamma_value;
@property (weak) IBOutlet NonnonScrollbar *n_resizer_gamma_scrollbar;

@property (weak) IBOutlet NSTextField     *n_resizer_colorwheel_value;
@property (weak) IBOutlet NonnonScrollbar *n_resizer_colorwheel_scrollbar;

@property (weak) IBOutlet NSTextField     *n_resizer_vividness_value;
@property (weak) IBOutlet NonnonScrollbar *n_resizer_vividness_scrollbar;

@property (weak) IBOutlet NSTextField     *n_resizer_sharpness_value;
@property (weak) IBOutlet NonnonScrollbar *n_resizer_sharpness_scrollbar;

@property (weak) IBOutlet NSTextField     *n_resizer_contrast_value;
@property (weak) IBOutlet NonnonScrollbar *n_resizer_contrast_scrollbar;

@property (weak) IBOutlet NSButton        *n_resizer_button_go;

// Formatter

@property (weak) IBOutlet NSWindow        *n_formatter_bmp_window;

@property (weak) IBOutlet NSWindow        *n_formatter_cur_window;

@property (weak) IBOutlet NSTextField     *n_formatter_cur_hotspot_x_value;
@property (weak) IBOutlet NonnonScrollbar *n_formatter_cur_hotspot_x_scrollbar;

@property (weak) IBOutlet NSTextField     *n_formatter_cur_hotspot_y_value;
@property (weak) IBOutlet NonnonScrollbar *n_formatter_cur_hotspot_y_scrollbar;

@property (weak) IBOutlet NSButton        *n_formatter_cur_button_go;

@property (weak) IBOutlet NonnonButton    *n_formatter_icon_cur;
@property (weak) IBOutlet NonnonButton    *n_formatter_icon_etc;


// Layer

@property (weak) IBOutlet NSWindow        *n_layer_window;

@property (weak) IBOutlet NonnonTxtbox    *n_layer_listbox;

@property (weak) IBOutlet NSButton        *n_layer_whole_preview_checkbox;
@property (weak) IBOutlet NSButton        *n_layer_whole_grab_checkbox;
@property (weak) IBOutlet NSButton        *n_layer_whole_save_checkbox;

@property (weak) IBOutlet NSTextField     *n_layer_blur_value;
@property (weak) IBOutlet NonnonScrollbar *n_layer_blur_scrollbar;

@property (weak) IBOutlet NSTextField     *n_layer_blend_value;
@property (weak) IBOutlet NonnonScrollbar *n_layer_blend_scrollbar;

@property (weak) IBOutlet NSMenu          *n_layer_popup_menu;
@property (weak) IBOutlet NSMenuItem      *n_layer_popup_menu_up;
@property (weak) IBOutlet NSMenuItem      *n_layer_popup_menu_down;



// Clear Canvas

@property (weak) IBOutlet NSWindow        *n_clear_window;

@property (weak) IBOutlet NonnonPaintColorPreview *n_clear_preview;

@property (weak) IBOutlet NSTextField     *n_clear_a_value;
@property (weak) IBOutlet NonnonScrollbar *n_clear_a_scrollbar;

@property (weak) IBOutlet NSTextField     *n_clear_r_value;
@property (weak) IBOutlet NonnonScrollbar *n_clear_r_scrollbar;

@property (weak) IBOutlet NSTextField     *n_clear_g_value;
@property (weak) IBOutlet NonnonScrollbar *n_clear_g_scrollbar;

@property (weak) IBOutlet NSTextField     *n_clear_b_value;
@property (weak) IBOutlet NonnonScrollbar *n_clear_b_scrollbar;

@property (weak) IBOutlet NSButton        *n_clear_go;


// Color Replacer

@property (weak) IBOutlet NSWindow        *n_replacer_window;

@property (weak) IBOutlet NonnonPaintColorPreview *n_replacer_fr_preview;

@property (weak) IBOutlet NSTextField     *n_replacer_fr_a_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_fr_a_scrollbar;

@property (weak) IBOutlet NSTextField     *n_replacer_fr_r_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_fr_r_scrollbar;

@property (weak) IBOutlet NSTextField     *n_replacer_fr_g_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_fr_g_scrollbar;

@property (weak) IBOutlet NSTextField     *n_replacer_fr_b_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_fr_b_scrollbar;

@property (weak) IBOutlet NonnonPaintColorPreview *n_replacer_to_preview;

@property (weak) IBOutlet NSTextField     *n_replacer_to_a_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_to_a_scrollbar;

@property (weak) IBOutlet NSTextField     *n_replacer_to_r_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_to_r_scrollbar;

@property (weak) IBOutlet NSTextField     *n_replacer_to_g_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_to_g_scrollbar;

@property (weak) IBOutlet NSTextField     *n_replacer_to_b_value;
@property (weak) IBOutlet NonnonScrollbar *n_replacer_to_b_scrollbar;

@property (weak) IBOutlet NSButton        *n_replacer_go;


// Alpha Tweaker

@property (weak) IBOutlet NSWindow     *n_alpha_tweaker_window;

@property (weak) IBOutlet NonnonButton *n_alpha_tweaker_icon;

@property (weak) IBOutlet NSButton     *n_alpha_tweaker_clear_checkbox;
@property (weak) IBOutlet NSButton     *n_alpha_tweaker_reverse_checkbox;

@property (weak) IBOutlet NSButton     *n_alpha_tweaker_go;


// Preview

@property (weak) IBOutlet NSWindow           *n_preview_window;
@property (weak) IBOutlet NonnonPaintPreview *n_preview_view;


// Layer Renamer

@property (weak) IBOutlet NSWindow    *n_renamer_window;
@property (weak) IBOutlet NSTextField *n_renamer_text;
@property (weak) IBOutlet NSButton    *n_renamer_button;


// Color History

@property (weak) IBOutlet NSWindow *n_colorhistory_window;
@property (weak) IBOutlet NSView   *n_colorhistory_canvas;


@end




@implementation AppDelegate {

	n_paint paint;

	BOOL    sheet_onoff;

	int     delayed_apply_id;
	u32     delayed_apply_timer;
	BOOL    delayed_apply_queue;

	// Resizer

	int  resizer_rotate;
	int  resizer_gamma;
	BOOL resizer_is_ok;



	// Layer

	BOOL layer_suppress_redraw;

	int  layer_click_phase;
	u32  layer_click_msec;

	// Renamer

	n_type_int renamer_index;

}




- (void) NonnonSettings : (BOOL) is_read
{

	NSString *str;

	if ( is_read )
	{

		str = n_mac_settings_read( @"pensize" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"5"]; }
		paint.pensize = [str intValue];

		str = n_mac_settings_read( @"mix" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"10"]; }
		paint.mix = [str intValue];

		str = n_mac_settings_read( @"boost" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"0"]; }
		paint.boost = [str intValue];

		str = n_mac_settings_read( @"air" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"0"]; }
		paint.air = [str intValue];


		str = n_mac_settings_read( @"a" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"255"]; }
		int a = [str intValue];

		str = n_mac_settings_read( @"r" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"255"]; }
		int r = [str intValue];

		str = n_mac_settings_read( @"g" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"255"]; }
		int g = [str intValue];

		str = n_mac_settings_read( @"b" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"255"]; }
		int b = [str intValue];

		paint.color = n_bmp_argb_mac( a,r,g,b );


		str = n_mac_settings_read( @"grid_onoff" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"0"]; }
		paint.grid_onoff = [str intValue];
		if ( paint.grid_onoff ) { [_n_menu_grid setState:NSControlStateValueOn]; }

		str = n_mac_settings_read( @"pixel_grid_onoff" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"0"]; }
		paint.pixel_grid_onoff = [str intValue];
		if ( paint.pixel_grid_onoff ) { [_n_menu_pixelgrid setState:NSControlStateValueOn]; }

		str = n_mac_settings_read( @"alpha_emphasizer_onoff" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"0"]; }
		paint.alpha_emphasizer_onoff = [str intValue];
		if ( paint.alpha_emphasizer_onoff ) { [_n_menu_alpha_emphasizer setState:NSControlStateValueOn]; }


		str = n_mac_settings_read( @"grabber_frame_mode" );
		if ( str.length == 0 ) { str = [NSString stringWithFormat:@"0"]; }
		paint.grabber_frame_mode = [str intValue];
		[_n_grabber_frame_mode_combo selectItemAtIndex:paint.grabber_frame_mode];

	} else {

		str = [NSString stringWithFormat:@"%d", paint.pensize];
		n_mac_settings_write( @"pensize", str );

		str = [NSString stringWithFormat:@"%d", paint.mix];
		n_mac_settings_write( @"mix", str );

		str = [NSString stringWithFormat:@"%d", paint.boost];
		n_mac_settings_write( @"boost", str );

		str = [NSString stringWithFormat:@"%d", paint.air];
		n_mac_settings_write( @"air", str );


		str = [NSString stringWithFormat:@"%d", n_bmp_a( paint.color )];
		n_mac_settings_write( @"a", str );

		str = [NSString stringWithFormat:@"%d", n_bmp_b( paint.color )];
		n_mac_settings_write( @"r", str );

		str = [NSString stringWithFormat:@"%d", n_bmp_g( paint.color )];
		n_mac_settings_write( @"g", str );

		str = [NSString stringWithFormat:@"%d", n_bmp_r( paint.color )];
		n_mac_settings_write( @"b", str );


		str = [NSString stringWithFormat:@"%d", paint.grid_onoff];
		n_mac_settings_write( @"grid_onoff", str );

		str = [NSString stringWithFormat:@"%d", paint.pixel_grid_onoff];
		n_mac_settings_write( @"pixel_grid_onoff", str );

		str = [NSString stringWithFormat:@"%d", paint.alpha_emphasizer_onoff];
		n_mac_settings_write( @"alpha_emphasizer_onoff", str );


		str = [NSString stringWithFormat:@"%d", paint.grabber_frame_mode];
		n_mac_settings_write( @"grabber_frame_mode", str );

	}

}




- (void) NonnonTxtbox_delegate_listbox_edited : (n_type_int) i;
{
//NSLog( @"NonnonTxtbox_delegate_listbox_edited" );
//NSLog( @"%s", paint.layer_data[ i ].name );

	n_posix_char *str = n_txt_get( _n_layer_listbox.n_txt_data, i );
//NSLog( @"%s", str );

	n_paint_layer_text_mod( &paint, i, str );


	[self NonnonPaintLayerMenuContext];

}

- (void) NonnonTxtbox_delegate_listbox_rename : (n_type_int) i;
{
//NSLog( @"%s", paint.layer_data[ i ].name );

	renamer_index = i;

	NSString *nsstring = n_mac_str2nsstring( paint.layer_data[ i ].name );
	[_n_renamer_text setStringValue:nsstring];

	n_mac_window_show( _n_renamer_window );

}

- (IBAction)n_renamer_method:(id)sender {

	NSString *nsstring = [_n_renamer_text stringValue];
	if ( nsstring.length >= N_PAINT_LAYER_CCH ) { return; }

	n_posix_char *str = n_mac_nsstring2str( nsstring );
	n_paint_layer_text_mod( &paint, renamer_index, str );
	n_string_free( str );

	[_n_layer_listbox display];


	n_mac_window_hide( _n_renamer_window );


	[self NonnonPaintLayerMenuContext];

}

- (void) NonnonTxtbox_delegate_edited:(id)txtbox onoff:(BOOL)onoff
{
}

- (void) NonnonTxtbox_delegate_find  :(id)txtbox
{
}

- (void) NonnonTxtbox_delegate_swap:(id)txtbox is_up:(BOOL)is_up
{
//NSLog( @"NonnonTxtbox_delegate_swap" );

	paint.layer_index = _n_layer_listbox.n_focus;

	if ( is_up )
	{
		if ( n_paint_layer_swap_up( paint.layer_index ) )
		{
			_n_layer_listbox.n_focus = paint.layer_index;
		}
	} else {
		if ( n_paint_layer_swap_down( paint.layer_index ) )
		{
			_n_layer_listbox.n_focus = paint.layer_index;
		}
	}

	[_n_layer_listbox NonnonTxtboxFocus2Caret];
	[_n_layer_listbox NonnonTxtboxCaretOutOfCanvasUpDown];

	[_n_paint_canvas display_optimized];

}

- (void) NonnonTxtbox_delegate_F3 :(id)txtbox is_left:(BOOL)is_left
{
}

- (BOOL)application:(NSApplication *)sender
           openFile:(NSString *)filename
{

	[self NonnonDragAndDrop_dropped:filename];

	return YES;
}

- (void) NonnonDragAndDrop_dropped : (NSString*) nsstr
{

	if ( paint.readonly )
	{
//NSLog( @"readonly" );
		return;
	}


	if ( paint.grabber_mode ) { return; }


	NSPoint    prv_scroll      = paint.scroll;
	n_type_int prv_layer_index = paint.layer_index;


	NSString *path = n_mac_alias_resolve( nsstr );
//return;


	n_posix_char *str = n_mac_nsstring2str( path );
//NSLog( @"%s", str );

	if ( paint.tooltype == N_PAINT_TOOL_TYPE_GRABBER )
	{

		n_bmp bmp; n_bmp_zero( &bmp );
		n_curico curico; n_curico_zero( &curico );
		if ( n_paint_load( str, &bmp, &curico ) )
		{
			//
		} else {
//NSLog( @"Here" );

			BOOL is_zero = n_mac_window_is_hovered_by_nwwindow( _n_toolwindow );
//NSLog( @"%d", is_zero );

			n_paint_grabber_select_drop( &bmp, is_zero, TRUE );

			paint.grabber_mode = N_PAINT_GRABBER_DRAG_OK;

			[n_paint_global n_paint_layer_window_onoff:FALSE];

			paint.layer_whole_preview_onoff = n_posix_false;
			paint.layer_whole_grab_onoff    = n_posix_false;
			[paint.whole_preview_checkbox setState:NSControlStateValueOff];
			[paint.whole_grab_checkbox    setState:NSControlStateValueOff];


			paint.grabber_frame_lock = TRUE;
			[_n_paint_canvas n_paint_grabber_frame_anim_on];


			n_bmp_flush( &paint.layer_cache_bmp_data, 0 );

			paint.grabber_cache_redraw_x  = 0;
			paint.grabber_cache_redraw_y  = 0;
			paint.grabber_cache_redraw_sx = 0;
			paint.grabber_cache_redraw_sy = 0;

			paint.grabber_resync_rect = NSMakeRect( 0,0,0,0 );


			[_n_paint_canvas display_optimized];
		}

	} else
	if ( n_paint_layer_load( &paint, str ) )
	{
//NSLog( @"n_paint_layer_load() : error" );

		if ( paint.grabber_mode != N_PAINT_GRABBER_NEUTRAL )
		{
			//
		} else
		if ( n_paint_load( str, &paint.bmp_data, &paint.curico ) )
		{
//NSLog( @"n_paint_load() : error" );
			//
		} else {
			paint.layer_save_count = 0;

			n_bmp_layer_turn_off( &paint );

			n_posix_char *s = n_mac_nsstring2str( path );
			if (
				( n_string_path_ext_is_same_literal( ".bmp", s ) )
				||
				( n_string_path_ext_is_same_literal( ".ico", s ) )
				||
				( n_string_path_ext_is_same_literal( ".cur", s ) )
				||
				( n_string_path_ext_is_same_literal( ".jpg", s ) )
				||
				( n_string_path_ext_is_same_literal( ".png", s ) )
			)
			{
				paint.filename = path;
			} else {
				n_string_path_ext_mod_literal( ".png", s );
				paint.filename = n_mac_str2nsstring( s );
			}
			n_string_path_free( s );

			n_bmp_free( &paint.bmp_grab );

			[self NonnonPaintTitle];

			paint.scroll = NSMakePoint( 0, 0 );
			[self NonnonPaintResize];

			[self NonnonPaintResizerReset];

			n_mac_window_hide( _n_layer_window );
		}

	} else {
//NSLog( @"layer" );

		paint.layer_save_count++;


		n_bmp_free( &paint.bmp_data );
		n_bmp_free( &paint.bmp_grab );

		[self NonnonPaintLayerSelect:paint.layer_index];


		n_posix_char *newname = n_string_alloccopy( n_posix_strlen( str ) * 2, str );

		if ( n_string_path_ext_is_same_literal( ".INI\0\0", newname ) )
		{
			n_posix_char *upper = n_string_path_upperfolder_new( newname );
			n_string_path_free( newname );
			newname = upper;
		}

		n_string_path_ext_add( ".lyr\0\0", newname );
//NSLog( @"%s", newname );

		paint.filename = n_mac_str2nsstring( newname );

		n_string_path_free( newname );


		[self NonnonPaintTitle];

		//paint.scroll = NSMakePoint( 0, 0 );
		paint.scroll = prv_scroll;
		[self NonnonPaintResize];

		[self NonnonPaintResizerReset];

		[_n_paint_canvas n_paint_canvas_reset_cache];


		static NSString *prv_filename = NULL;

		if ( [paint.filename compare:prv_filename] == 0 )
		{
//NSLog( @"Same" );
			paint.layer_index = prv_layer_index;
		} else {
//NSLog( @"Not the Same" );
			//
		}

		prv_filename = [paint.filename copy];


		_n_layer_listbox.n_focus = paint.layer_index;
//NSLog( @"%f", _n_layer_listbox.n_focus );
		[self NonnonPaintLayerSelect:paint.layer_index];


		[_n_layer_listbox NonnonTxtboxFocus2Caret];
		[_n_layer_listbox NonnonTxtboxCaretOutOfCanvasUpDown];

		[_n_layer_listbox display];


		paint.layer_whole_preview_onoff = n_posix_false;
		paint.layer_whole_grab_onoff    = n_posix_false;
		paint.layer_whole_save_onoff    = n_posix_true;

		[paint.whole_preview_checkbox setState:NSControlStateValueOff];
		[paint.whole_grab_checkbox    setState:NSControlStateValueOff];
		[paint.whole_save_checkbox    setState:NSControlStateValueOn ];


		[self NonnonPaintLayerRefresh];

		[self NonnonPaintWindowShowHide:TRUE];

	}

	[self NonnonPaintIconDisplay:_n_button_4_1];

	n_string_free( str );

}




- (void) NonnonPaintWindowShowHide:(BOOL) is_show
{

	if ( is_show )
	{

		n_mac_window_show( _window );

		if ( paint.resizer_onoff )
		{
			n_mac_window_show( _n_resizer_window );
			[_window addChildWindow:_n_resizer_window ordered:NSWindowAbove];
		} else {
			n_mac_window_hide( _n_resizer_window );
		}

		if ( paint.layer_onoff )
		{
			n_mac_window_show( _n_layer_window );
			[_window addChildWindow:_n_layer_window ordered:NSWindowAbove];
		} else {
			n_mac_window_hide( _n_layer_window );
		}

		n_type_int i = 1;
		n_posix_loop
		{
			if ( paint.nswindow[ i ] == nil ) { break; }

			if ( paint.nswindow[ i ] == _n_resizer_window )
			{
				//
			} else
			if ( paint.nswindow[ i ] == _n_layer_window )
			{
				//
			} else
			if ( paint.restore[ i ] )
			{
				paint.restore[ i ] = FALSE;

				n_mac_window_show( paint.nswindow[ i ] );
				[_window addChildWindow:paint.nswindow[ i ] ordered:NSWindowAbove];
			}

			i++;
		}
	} else {
		n_type_int i = 0;
		n_posix_loop
		{
			if ( paint.nswindow[ i ] == nil ) { break; }
//NSLog( @"%lld %d %lu", i, paint.nswindow[ i ].visible, paint.nswindow[ i ].occlusionState );

			if ( n_mac_window_is_visible( paint.nswindow[ i ] ) )
			{
				paint.restore[ i ] = TRUE;
			}

			n_mac_window_hide( paint.nswindow[ i ] );

			i++;
		}
	}

}

- (void) NonnonPaintTitle
{
//return;

	n_type_gfx bmpsx = N_BMP_SX( paint.pen_bmp_data );
	n_type_gfx bmpsy = N_BMP_SY( paint.pen_bmp_data );

	NSString *fname;

	n_posix_char *str = n_mac_nsstring2str( paint.filename );
	if ( n_string_path_ext_is_same_literal( ".LYR\0\0", str ) )
	{
		fname = [paint.filename stringByDeletingPathExtension];
	} else {
		fname = paint.filename;
	}
	n_string_free( str );

	NSString *title = [NSString stringWithFormat:@"%@ (%d x %d)", fname, bmpsx, bmpsy];
	[_window setTitle:title];

}

- (void) NonnonPaintZoomUI
{

	if ( paint.zoom > 0 )
	{
		[_n_zoom_value setIntegerValue:paint.zoom];
	} else {
		NSString *nsstr = [NSString stringWithFormat:@"1/%d", paint.zoom * -1];
		[_n_zoom_value setStringValue:nsstr];
	}

	[_n_paint_canvas display_optimized];

}

- (void) NonnonPaintResize
{

	CGFloat sx = N_BMP_SX( &paint.bmp_data );
	CGFloat sy = N_BMP_SY( &paint.bmp_data );

	CGFloat dsx,dsy; n_mac_desktop_size( &dsx, &dsy );

	{
//NSLog( @"%0.2f %0.2f", dsx, dsy );

		sx = dsx * 0.70;
		sy = dsy * 0.70;

		paint.inner_sx = sx;
		paint.inner_sy = sy;
	}

	if ( paint.margin_onoff )
	{
		paint.margin = n_posix_min_n_type_real( dsx, dsy ) * 0.1;

		sx += paint.margin;
		sy += paint.margin;
	}

	[self NonnonPaintZoomUI];

	[_window setContentSize:NSMakeSize( sx,sy )];
	n_mac_window_centering( _window );

	[_n_paint_canvas setFrame:NSMakeRect( 0,0,sx,sy )];
	[_n_paint_canvas display_optimized];

}

- (void) NonnonPaintColorSet
{

	int a = n_bmp_a( paint.color );
	int r = n_bmp_r( paint.color );
	int g = n_bmp_g( paint.color );
	int b = n_bmp_b( paint.color );
//NSLog( @"%d %d %d %d", a,r,g,b );

	[_n_color_a_value setIntegerValue:a];
	[_n_color_r_value setIntegerValue:b];
	[_n_color_g_value setIntegerValue:g];
	[_n_color_b_value setIntegerValue:r];

	[_n_color_a_scrollbar n_scrollbar_position_set:a redraw:YES];
	[_n_color_r_scrollbar n_scrollbar_position_set:b redraw:YES];
	[_n_color_g_scrollbar n_scrollbar_position_set:g redraw:YES];
	[_n_color_b_scrollbar n_scrollbar_position_set:r redraw:YES];


	[_n_color_preview display];

}

- (void) NonnonPaintClearCanvasColorSet
{

	int a = n_bmp_a( paint.clear_color );
	int r = n_bmp_r( paint.clear_color );
	int g = n_bmp_g( paint.clear_color );
	int b = n_bmp_b( paint.clear_color );
//NSLog( @"%d %d %d %d", a,r,g,b );

	[_n_clear_a_value setIntegerValue:a];
	[_n_clear_r_value setIntegerValue:b];
	[_n_clear_g_value setIntegerValue:g];
	[_n_clear_b_value setIntegerValue:r];

	[_n_clear_a_scrollbar n_scrollbar_position_set:a redraw:YES];
	[_n_clear_r_scrollbar n_scrollbar_position_set:b redraw:YES];
	[_n_clear_g_scrollbar n_scrollbar_position_set:g redraw:YES];
	[_n_clear_b_scrollbar n_scrollbar_position_set:r redraw:YES];


	[_n_color_preview display];

}

- (void) NonnonPaintColorReplacerColorSet
{

	{

		int a = n_bmp_a( paint.replacer_fr_color );
		int r = n_bmp_r( paint.replacer_fr_color );
		int g = n_bmp_g( paint.replacer_fr_color );
		int b = n_bmp_b( paint.replacer_fr_color );
//NSLog( @"%d %d %d %d", a,r,g,b );

		[_n_replacer_fr_a_value setIntegerValue:a];
		[_n_replacer_fr_r_value setIntegerValue:b];
		[_n_replacer_fr_g_value setIntegerValue:g];
		[_n_replacer_fr_b_value setIntegerValue:r];

		[_n_replacer_fr_a_scrollbar n_scrollbar_position_set:a redraw:YES];
		[_n_replacer_fr_r_scrollbar n_scrollbar_position_set:b redraw:YES];
		[_n_replacer_fr_g_scrollbar n_scrollbar_position_set:g redraw:YES];
		[_n_replacer_fr_b_scrollbar n_scrollbar_position_set:r redraw:YES];

		[_n_replacer_fr_preview display];

	}

	{

		int a = n_bmp_a( paint.replacer_to_color );
		int r = n_bmp_r( paint.replacer_to_color );
		int g = n_bmp_g( paint.replacer_to_color );
		int b = n_bmp_b( paint.replacer_to_color );
//NSLog( @"%d %d %d %d", a,r,g,b );

		[_n_replacer_to_a_value setIntegerValue:a];
		[_n_replacer_to_r_value setIntegerValue:b];
		[_n_replacer_to_g_value setIntegerValue:g];
		[_n_replacer_to_b_value setIntegerValue:r];

		[_n_replacer_to_a_scrollbar n_scrollbar_position_set:a redraw:YES];
		[_n_replacer_to_r_scrollbar n_scrollbar_position_set:b redraw:YES];
		[_n_replacer_to_g_scrollbar n_scrollbar_position_set:g redraw:YES];
		[_n_replacer_to_b_scrollbar n_scrollbar_position_set:r redraw:YES];

		[_n_replacer_to_preview display];

	}

}

- (void) NonnonPaintGrabberUIOnOff:(BOOL)onoff
{

	NSColor *color;
	if ( onoff )
	{
		color = [NSColor textColor];
	} else {
		color = [NSColor systemGrayColor];
	}

	[_n_ppa_check            setEnabled  :onoff];
	[_n_grabber_blend_label  setTextColor:color];
	[_n_grabber_blend_value  setTextColor:color];
	[_n_grabber_blend_scrollbar n_scrollbar_enable:onoff redraw:YES];

	[_n_grabber_frame_mode_label setTextColor:color];
	[_n_grabber_frame_mode_combo setEnabled  :onoff];

}

- (void) NonnonPaintStatus
{
//return;

	n_posix_char str1[ 100 ];
	n_posix_char str2[ 100 ];
	n_posix_char str3[ 100 ];

	if ( paint.tooltype == N_PAINT_TOOL_TYPE_PEN )
	{

		n_posix_sprintf_literal( str1, "Pen" );

		NSPoint pt = [_n_paint_canvas n_paint_point_on_bitmap];
		n_posix_sprintf_literal( str2, "X:%0.0f", pt.x );
		n_posix_sprintf_literal( str3, "Y:%0.0f", pt.y );

	} else
	if ( paint.tooltype == N_PAINT_TOOL_TYPE_FILL )
	{

		n_posix_sprintf_literal( str1, "Fill" );

		NSPoint pt = [_n_paint_canvas n_paint_point_on_bitmap];
		n_posix_sprintf_literal( str2, "X:%0.0f", pt.x );
		n_posix_sprintf_literal( str3, "Y:%0.0f", pt.y );

	} else
	if ( paint.tooltype == N_PAINT_TOOL_TYPE_GRABBER )
	{

		if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			n_posix_sprintf_literal( str1, "Neutral" );
		} else
		if ( paint.grabber_mode == N_PAINT_GRABBER_SELECTING )
		{
			n_posix_sprintf_literal( str1, "Selecting" );
		} else
		if ( paint.grabber_mode == N_PAINT_GRABBER_DRAG_OK )
		{
			n_posix_sprintf_literal( str1, "Drag OK" );
		} else
		if ( paint.grabber_mode == N_PAINT_GRABBER_DRAGGING )
		{
			n_posix_sprintf_literal( str1, "Dragging" );
		} else
		if ( paint.grabber_mode == N_PAINT_GRABBER_STRETCH_TRANSFORM )
		{
			n_posix_sprintf_literal( str1, "Stretching" );
		} else
		if ( paint.grabber_mode == N_PAINT_GRABBER_STRETCH_PROPORTIONAL )
		{
			n_posix_sprintf_literal( str1, "Stretching" );
		} else {
			n_posix_sprintf_literal( str1, "N/A" );
			//n_posix_sprintf_literal( str1, "%d", paint.grabber_mode );
		}


		n_type_gfx x,y,sx,sy,fx,fy;

		if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			x = y = sx = sy = fx = fy = 0;
		} else {
			n_paint_grabber_system_get( &x,&y, &sx,&sy, &fx,&fy );
		}

		n_posix_sprintf_literal( str2, "X:%d Y:%d", x,y );
		n_posix_sprintf_literal( str3, "%d x %d", sx,sy );

	}

	[_n_status1 setStringValue:n_mac_str2nsstring( str1 )];
	[_n_status2 setStringValue:n_mac_str2nsstring( str2 )];
	[_n_status3 setStringValue:n_mac_str2nsstring( str3 )];


	return;
}

- (void) NonnonPaintResizerReset
{

	[_n_resizer_combo selectItemAtIndex:2];


	n_type_gfx bmpsx;
	n_type_gfx bmpsy;

//NSLog( @"%d", n_paint_global.paint->grabber_mode );
	if ( n_paint_global.paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		bmpsx = N_BMP_SX( paint.pen_bmp_data );
		bmpsy = N_BMP_SY( paint.pen_bmp_data );
	} else {
		bmpsx = N_BMP_SX( paint.pen_bmp_grab );
		bmpsy = N_BMP_SY( paint.pen_bmp_grab );
	}

	[_n_resizer_sx setEnabled:TRUE];
	[_n_resizer_sy setEnabled:TRUE];

	NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
	formatter.hasThousandSeparators = NO;

	[_n_resizer_sx setFormatter:formatter];
	[_n_resizer_sy setFormatter:formatter];

	[_n_resizer_sx setStringValue:[NSString stringWithFormat:@"%d", bmpsx]];
	[_n_resizer_sy setStringValue:[NSString stringWithFormat:@"%d", bmpsy]];


	resizer_rotate = 360;

	_n_resizer_rotate_scrollbar.delegate = self;
	[_n_resizer_rotate_scrollbar n_scrollbar_parameter:N_PAINT_ID_RESIZER_ROTATE step:1 page:10 max:720 pos:resizer_rotate redraw:n_posix_true];
	[_n_resizer_rotate_scrollbar n_scrollbar_nswindow_set:_n_resizer_window];

	[self NonnonScrollbarMessage:N_PAINT_ID_RESIZER_ROTATE value:resizer_rotate reason:0];


	[_n_resizer_color_combobox selectItemAtIndex:0];

	resizer_gamma = 10;
	_n_resizer_gamma_scrollbar.delegate = self;
	[_n_resizer_gamma_scrollbar n_scrollbar_parameter:N_PAINT_ID_RESIZER_GAMMA step:1 page:5 max:20 pos:resizer_gamma redraw:n_posix_true];
	[_n_resizer_gamma_scrollbar n_scrollbar_nswindow_set:_n_resizer_window];
	[_n_resizer_gamma_value setIntegerValue:resizer_gamma];

	[self NonnonScrollbarMessage:N_PAINT_ID_RESIZER_GAMMA value:resizer_gamma reason:0];

	_n_resizer_colorwheel_scrollbar.delegate = self;
	[_n_resizer_colorwheel_scrollbar n_scrollbar_parameter:N_PAINT_ID_RESIZER_COLORWHEEL step:1 page:10 max:256 pos:128 redraw:n_posix_true];
	[_n_resizer_colorwheel_scrollbar n_scrollbar_nswindow_set:_n_resizer_window];
	[_n_resizer_colorwheel_value setIntegerValue:0];

	_n_resizer_vividness_scrollbar.delegate = self;
	[_n_resizer_vividness_scrollbar n_scrollbar_parameter:N_PAINT_ID_RESIZER_VIVIDNESS step:1 page:10 max:200 pos:100 redraw:n_posix_true];
	[_n_resizer_vividness_scrollbar n_scrollbar_nswindow_set:_n_resizer_window];
	[_n_resizer_vividness_value setIntegerValue:0];

	_n_resizer_sharpness_scrollbar.delegate = self;
	[_n_resizer_sharpness_scrollbar n_scrollbar_parameter:N_PAINT_ID_RESIZER_SHARPNESS step:1 page:10 max:200 pos:100 redraw:n_posix_true];
	[_n_resizer_sharpness_scrollbar n_scrollbar_nswindow_set:_n_resizer_window];
	[_n_resizer_sharpness_value setIntegerValue:0];

	_n_resizer_contrast_scrollbar.delegate = self;
	[_n_resizer_contrast_scrollbar n_scrollbar_parameter:N_PAINT_ID_RESIZER_CONTRAST step:1 page:10 max:100 pos:0 redraw:n_posix_true];
	[_n_resizer_contrast_scrollbar n_scrollbar_nswindow_set:_n_resizer_window];
	[_n_resizer_contrast_value setIntegerValue:0];


	n_paint_global.paint->grabber_rect_resizer = n_paint_global.paint->grabber_rect;


	resizer_is_ok = FALSE;


	return;
}

- (void) NonnonPaintIconSet:(NSString*) rc_name button:(NonnonButton*)button
{

	n_bmp bmp_icon; n_bmp_zero( &bmp_icon );

	n_mac_image_rc_load_bmp( rc_name, &bmp_icon );

	n_mac_button_system_themed( &bmp_icon );

	[button n_icon_free];
	[button n_icon_set:&bmp_icon];
	[button display];

}

- (void) NonnonPaintIconDisplay:(NonnonButton*)button
{
//return;

	n_posix_char *path = n_mac_nsstring2str( paint.filename );

	if ( n_string_path_ext_is_same_literal( ".BMP\0\0", path ) )
	{
		[self NonnonPaintIconSet:@"rc/save"     button:button];
	} else
	if ( n_string_path_ext_is_same_literal( ".ICO\0\0", path ) )
	{
		[self NonnonPaintIconSet:@"rc/save/ico" button:button];
	} else
	if ( n_string_path_ext_is_same_literal( ".CUR\0\0", path ) )
	{
		[self NonnonPaintIconSet:@"rc/save/cur" button:button];
	} else
	if ( n_string_path_ext_is_same_literal( ".JPG\0\0", path ) )
	{
		[self NonnonPaintIconSet:@"rc/save/jpg" button:button];
	} else
	if ( n_string_path_ext_is_same_literal( ".PNG\0\0", path ) )
	{
		[self NonnonPaintIconSet:@"rc/save/png" button:button];
	} else
	if ( n_string_path_ext_is_same_literal( ".LYR\0\0", path ) )
	{
		[self NonnonPaintIconSet:@"rc/save/lyr" button:button];
	}

	[button display];

	n_string_free( path );

}

- (void) NonnonPaintLayerSync
{

	n_type_int     index = paint.layer_index;
	n_paint_layer *layer = &paint.layer_data[ index ];

	_n_layer_listbox.n_focus = index;

	[_n_layer_listbox NonnonTxtboxFocus2Caret];
	[_n_layer_listbox NonnonTxtboxCaretOutOfCanvasUpDown];

	layer->blur    = (int) [_n_layer_blur_value  integerValue];
	layer->percent = (int) [_n_layer_blend_value integerValue];
	layer->blend   = (n_type_real) layer->percent * 0.01;

	[_n_paint_canvas display_optimized];

}

- (void) NonnonPaintLayerRefresh
{

	if ( paint.layer_onoff == FALSE ) { return; }


	n_type_int     index = paint.layer_index;
	n_paint_layer *layer = &paint.layer_data[ index ];

	_n_layer_listbox.n_focus = index;

	[_n_layer_listbox NonnonTxtboxFocus2Caret];
	[_n_layer_listbox display];

	[_n_layer_blur_value  setIntegerValue:layer->blur   ];
	[_n_layer_blend_value setIntegerValue:layer->percent];


	layer_suppress_redraw = TRUE;

	[_n_layer_blur_scrollbar  n_scrollbar_position_set:layer->blur    redraw:YES];
	[_n_layer_blend_scrollbar n_scrollbar_position_set:layer->percent redraw:YES];

	layer_suppress_redraw = FALSE;

}

- (void) NonnonPaintLayerSelect:(n_type_int)index
{

	paint.layer_index = index;
//NSLog( @"%d", paint.layer_index );

	paint.pen_bmp_data = &paint.layer_data[ paint.layer_index ].bmp_data;
	paint.pen_bmp_grab = &paint.layer_data[ paint.layer_index ].bmp_grab;

	//[_n_layer_listbox display];

}

- (void) NonnonPaintSave
{

	n_posix_char *path = n_mac_nsstring2str( paint.filename );

	if ( n_paint_save( path, &paint.bmp_data, &paint.curico ) )
	{
		// [!] : when error

		// [!] : Multi-Layer : not working under Sandbox : mkdir() will fail

		if ( n_string_path_ext_is_same_literal( ".LYR\0\0", path ) )
		{
			n_mac_window_dialog_ok( "Sorry, not supported." ); 
			n_string_free( path );
			return;
		}

		if ( sheet_onoff )
		{
			[_window endSheet:_n_formatter_cur_window];
			[_window endSheet:_n_formatter_bmp_window];
		}

		NSString *name = [paint.filename lastPathComponent];

		NSSavePanel *panel = [NSSavePanel savePanel];
		[panel setNameFieldStringValue:name];
		//panel.canCreateDirectories = YES;
		NSModalResponse result = [panel runModal];

		// [x] : return immediately
		//[panel beginSheetModalForWindow:_window completionHandler:^(NSInteger result)
		{
			if ( result == NSModalResponseOK )
			{
				NSString     *nsstring = [[panel URL] path];
				n_posix_char *str      = n_mac_nsstring2str( nsstring );
//NSLog( @"Saved : %s", str );
				n_paint_save( str, &self->paint.bmp_data, &self->paint.curico );

				self->paint.filename = nsstring;
				[self NonnonPaintTitle];

				n_string_free( str );
			}
		}
		//];
//NSLog( @"!" );

		[self NonnonPaintWindowShowHide:TRUE];

	} else {

		// [!] : no better ways : at first, layer initialization is needed

		if ( n_string_path_ext_is_same_literal( ".LYR\0\0", path ) )
		{
			if ( paint.layer_save_count == 0 )
			{
				paint.readonly = FALSE;

				NSString *fname = [paint.filename stringByDeletingPathExtension];
				[self NonnonDragAndDrop_dropped:fname];
			}

			paint.layer_save_count++;
			if ( paint.layer_save_count > 1 ) { paint.layer_save_count = 1; }
		}

	}

	n_string_free( path );

}

- (void) NonnonPaintPreview
{

	static BOOL is_init = FALSE;

	BOOL show;

	if ( is_init == FALSE )
	{
		is_init = TRUE;
		   show = TRUE;
	} else
	if ( [_n_preview_window isVisible] )
	{
		show = FALSE;;
	} else {
		show = TRUE;
	}

	if ( show )
	{
		n_paint_preview_cache_init( &paint, _n_preview_window, _n_preview_view );

		n_mac_window_centering( _n_preview_window );
		//n_mac_topmost( _n_preview_window, TRUE );

		[self NonnonPaintUIContext:nil onoff:FALSE];
		[self NonnonPaintWindowShowHide:FALSE];

		n_mac_window_show( _n_preview_window );	
	} else {
		[self NonnonPaintUIContext:nil onoff:TRUE];
		[self NonnonPaintWindowShowHide:TRUE];

		n_mac_window_hide( _n_preview_window );	
	}


	return;
}



/*
- (BOOL)validateMenuItem:(NSMenuItem *)menuItem {

	// [x] : deprecated

	return [menuItem isEnabled];

}
*/
- (void) NonnonPaintUIContext:(NSWindow*)window onoff:(BOOL)onoff
{

	if ( onoff )
	{

		_n_menu_clear_canvas  .enabled = TRUE;
		_n_menu_alpha_tweaker .enabled = TRUE;
		_n_menu_color_replacer.enabled = TRUE;
		_n_menu_about         .enabled = TRUE;

		[_n_button_3_1 n_enable:TRUE]; [_n_button_3_1 n_press:FALSE]; [_n_button_3_1 display];
		[_n_button_4_1 n_enable:TRUE]; [_n_button_4_1 n_press:FALSE]; [_n_button_4_1 display];

	} else {

		_n_menu_clear_canvas  .enabled = FALSE;
		_n_menu_alpha_tweaker .enabled = FALSE;
		_n_menu_color_replacer.enabled = FALSE;
		_n_menu_about         .enabled = FALSE;

		if ( window != _n_resizer_window )
		{
			[_n_button_3_1 n_enable:FALSE];
			[_n_button_3_1 display];
		}

		//if ( window != nil )
		{
			[_n_button_4_1 n_enable:FALSE];
			[_n_button_4_1 display];
		}

		n_mac_topmost( window, TRUE );

	}

}

- (void) NonnonPaintLayerMenuContext
{
//NSLog( @"!" );

	BOOL menu_onoff;
	if ( n_paint_layer_is_locked( paint.layer_index ) )
	{
		menu_onoff = FALSE;
	} else {
		menu_onoff =  TRUE;
	}

	_n_menu_clear_canvas  .enabled = menu_onoff;
	_n_menu_color_replacer.enabled = menu_onoff;
	_n_menu_alpha_tweaker .enabled = menu_onoff;

}




- (void)awakeFromNib
{

	// [!] : Global

	n_memory_limit_callback_func = n_paint_memory_limit;


	n_bmp_safemode = n_posix_false;

	// [!] : macOS 12 Monterey needs this
	[_n_toolwindow     setFrameAutosaveName:@"Nonnon.NonnonPaint.ToolWindow"];
	[_n_layer_window   setFrameAutosaveName:@"Nonnon.NonnonPaint.LayerWindow"];
	[_n_resizer_window setFrameAutosaveName:@"Nonnon.NonnonPaint.ResizerWindow"];

	sheet_onoff = TRUE;

	n_mac_image_window = _window;
	n_gdi_scale_factor = n_mac_image_window.backingScaleFactor;


	if ( n_mac_is_sandbox() )
	{
		_n_toolwindow.title = @"Nonnon Paint Lite";
	} else {
		_n_toolwindow.title = @"Nonnon Paint";
	}


	// [!] : Instance

	n_paint_zero( &paint );

	paint.margin_onoff = TRUE;

	paint.scroller_size = 12;

	paint.queue = [[NSOperationQueue alloc] init];
	paint.cores = n_posix_cpu_count();

	paint.nswindow[  0 ] = _window;
	paint.nswindow[  1 ] = _n_toolwindow;
	paint.nswindow[  2 ] = _n_layer_window;
	paint.nswindow[  3 ] = _n_resizer_window;
	paint.nswindow[  4 ] = _n_formatter_bmp_window;
	paint.nswindow[  5 ] = _n_formatter_cur_window;
	paint.nswindow[  6 ] = _n_clear_window;
	paint.nswindow[  7 ] = _n_replacer_window;
	paint.nswindow[  8 ] = _n_alpha_tweaker_window;
	paint.nswindow[  9 ] = _n_preview_window;
	paint.nswindow[ 10 ] = _n_renamer_window;
	paint.nswindow[ 11 ] = nil;
	

	// [!] : Resources

	paint.cursor_arrow = [NSCursor arrowCursor];
	paint.cursor_no    = [NSCursor operationNotAllowedCursor];
	paint.cursor_hand  = [NSCursor closedHandCursor];
	paint.cursor_blur  = [NSCursor pointingHandCursor];

	{
		n_bmp bmp; n_bmp_zero( &bmp );
		n_mac_image_rc_load_bmp( @"rc/cursor/pen", &bmp );

		NSImage *img = n_mac_image_nbmp2nsimage( &bmp );

		int hotspot_x = N_BMP_SX( &bmp ) / 2; hotspot_x--;
		int hotspot_y = N_BMP_SY( &bmp ) / 2; hotspot_y--;

		paint.cursor_pen_off = [[NSCursor alloc] initWithImage:img hotSpot:NSMakePoint( hotspot_x, hotspot_y )];
	}

	{
		n_bmp bmp; n_bmp_zero( &bmp );
		n_mac_image_rc_load_bmp( @"rc/cursor/pen_on", &bmp );

		NSImage *img = n_mac_image_nbmp2nsimage( &bmp );

		int hotspot_x = N_BMP_SX( &bmp ) / 2; hotspot_x--;
		int hotspot_y = N_BMP_SY( &bmp ) / 2; hotspot_y--;

		paint.cursor_pen_on = [[NSCursor alloc] initWithImage:img hotSpot:NSMakePoint( hotspot_x, hotspot_y )];
	}

	{
		n_bmp bmp; n_bmp_zero( &bmp );
		n_mac_image_rc_load_bmp( @"rc/cursor/eraser", &bmp );

		NSImage *img = n_mac_image_nbmp2nsimage( &bmp );

		int hotspot_x = N_BMP_SX( &bmp ) / 2; hotspot_x--;
		int hotspot_y = N_BMP_SY( &bmp ) / 2; hotspot_y--;

		paint.cursor_eraser = [[NSCursor alloc] initWithImage:img hotSpot:NSMakePoint( hotspot_x, hotspot_y )];
	}

	{
		n_bmp bmp; n_bmp_zero( &bmp );
		n_mac_image_rc_load_bmp( @"rc/cursor/resize_nw", &bmp );

		NSImage *img = n_mac_image_nbmp2nsimage( &bmp );

		int hotspot_x = N_BMP_SX( &bmp ) / 2; hotspot_x--;
		int hotspot_y = N_BMP_SY( &bmp ) / 2; hotspot_y--;

		paint.cursor_resize_nw = [[NSCursor alloc] initWithImage:img hotSpot:NSMakePoint( hotspot_x, hotspot_y )];
	}

	{
		n_bmp bmp; n_bmp_zero( &bmp );
		n_mac_image_rc_load_bmp( @"rc/cursor/resize_ne", &bmp );

		NSImage *img = n_mac_image_nbmp2nsimage( &bmp );

		int hotspot_x = N_BMP_SX( &bmp ) / 2; hotspot_x--;
		int hotspot_y = N_BMP_SY( &bmp ) / 2; hotspot_y--;

		paint.cursor_resize_ne = [[NSCursor alloc] initWithImage:img hotSpot:NSMakePoint( hotspot_x, hotspot_y )];
	}


	// [!] : Tool Window : Settings

	paint.color = n_bmp_rgb_mac( 0,200,255 );

	[self NonnonSettings:YES];


	// [!] : XMouse

	_n_window_stub = [[NonnonPaintStub alloc] init];
	[_n_window_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_window contentView] addSubview:_n_window_stub];

	_n_window_stub.delegate = self;
	_n_window_stub.delegate_canvas = _n_paint_canvas;


	_n_tool_stub = [[NonnonPaintStub alloc] init];
	[_n_tool_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_toolwindow contentView] addSubview:_n_tool_stub];

	_n_tool_stub.delegate = self;


	_n_layer_stub = [[NonnonPaintStub alloc] init];
	[_n_layer_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_layer_window contentView] addSubview:_n_layer_stub];

	_n_layer_stub.delegate = self;


	_n_resizer_stub = [[NonnonPaintStub alloc] init];
	[_n_resizer_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_resizer_window contentView] addSubview:_n_resizer_stub];

	_n_resizer_stub.delegate = self;


	_n_formatter_bmp_stub = [[NonnonPaintStub alloc] init];
	[_n_formatter_bmp_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_formatter_bmp_window contentView] addSubview:_n_formatter_bmp_stub];

	_n_formatter_bmp_stub.delegate = self;


	_n_formatter_cur_stub = [[NonnonPaintStub alloc] init];
	[_n_formatter_cur_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_formatter_cur_window contentView] addSubview:_n_formatter_cur_stub];

	_n_formatter_cur_stub.delegate = self;


	_n_clear_stub = [[NonnonPaintStub alloc] init];
	[_n_clear_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_clear_window contentView] addSubview:_n_clear_stub];

	_n_clear_stub.delegate = self;


	_n_replacer_stub = [[NonnonPaintStub alloc] init];
	[_n_replacer_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_replacer_window contentView] addSubview:_n_replacer_stub];

	_n_replacer_stub.delegate = self;


	_n_alpha_tweaker_stub = [[NonnonPaintStub alloc] init];
	[_n_alpha_tweaker_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_alpha_tweaker_window contentView] addSubview:_n_alpha_tweaker_stub];

	_n_alpha_tweaker_stub.delegate = self;


	_n_preview_stub = [[NonnonPaintStub alloc] init];
	[_n_preview_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_preview_window contentView] addSubview:_n_preview_stub];

	_n_preview_stub.delegate = self;


	_n_renamer_stub = [[NonnonPaintStub alloc] init];
	[_n_renamer_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_renamer_window contentView] addSubview:_n_renamer_stub];

	_n_renamer_stub.delegate = self;


	_n_colorhistory_stub = [[NonnonPaintStub alloc] init];
	[_n_colorhistory_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_colorhistory_window contentView] addSubview:_n_colorhistory_stub];

	_n_colorhistory_stub.delegate = self;


	// [!] : Tool Window

	paint.tooltype = N_PAINT_TOOL_TYPE_PEN;
	//paint.tooltype = N_PAINT_TOOL_TYPE_FILL;
	//paint.tooltype = N_PAINT_TOOL_TYPE_GRABBER;

	paint.zoom     = 1;
	paint.scroll   = NSMakePoint( 0, 0 );

	_n_size_scrollbar.delegate = self;
	[_n_size_scrollbar n_scrollbar_parameter:N_PAINT_ID_PENSIZE step:2 page:10 max:100 pos:paint.pensize redraw:n_posix_true];
	[_n_size_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_size_value setIntegerValue:paint.pensize];

	_n_mix_scrollbar.delegate = self;
	[_n_mix_scrollbar n_scrollbar_parameter:N_PAINT_ID_MIX      step:1 page:10 max:100 pos:paint.mix     redraw:n_posix_true];
	[_n_mix_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_mix_value setIntegerValue:paint.mix];

	_n_boost_scrollbar.delegate = self;
	[_n_boost_scrollbar n_scrollbar_parameter:N_PAINT_ID_BOOST  step:1 page:10 max:100 pos:paint.boost   redraw:n_posix_true];
	[_n_boost_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_boost_value setIntegerValue:paint.boost];

	_n_air_scrollbar.delegate = self;
	[_n_air_scrollbar n_scrollbar_parameter:N_PAINT_ID_AIR      step:1 page:10 max:100 pos:paint.air     redraw:n_posix_true];
	[_n_air_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_air_value setIntegerValue:paint.air];

	_n_zoom_scrollbar.delegate = self;
	[_n_zoom_scrollbar n_scrollbar_parameter:N_PAINT_ID_ZOOM    step:1 page:10 max:100 pos:paint.zoom+50 redraw:n_posix_true];
	[_n_zoom_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_zoom_value setIntegerValue:paint.zoom];


	_n_color_preview.color = &paint.color;

	[self NonnonPaintColorSet];

	_n_color_a_scrollbar.delegate = self;
	_n_color_r_scrollbar.delegate = self;
	_n_color_g_scrollbar.delegate = self;
	_n_color_b_scrollbar.delegate = self;

	[_n_color_a_scrollbar n_scrollbar_parameter:N_PAINT_ID_COLOR_A step:1 page:10 max:255 pos:n_bmp_a( paint.color ) redraw:n_posix_true];
	[_n_color_r_scrollbar n_scrollbar_parameter:N_PAINT_ID_COLOR_R step:1 page:10 max:255 pos:n_bmp_b( paint.color ) redraw:n_posix_true];
	[_n_color_g_scrollbar n_scrollbar_parameter:N_PAINT_ID_COLOR_G step:1 page:10 max:255 pos:n_bmp_g( paint.color ) redraw:n_posix_true];
	[_n_color_b_scrollbar n_scrollbar_parameter:N_PAINT_ID_COLOR_B step:1 page:10 max:255 pos:n_bmp_r( paint.color ) redraw:n_posix_true];

	[_n_color_a_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_color_r_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_color_g_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_color_b_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];

	u32 color1 = n_mac_nscolor2argb( [NSColor clearColor] );
	u32 color2 = n_mac_nscolor2argb( [NSColor lightGrayColor] );

	[_n_color_a_scrollbar n_scrollbar_gradient:nil from:color1      to:color2                  ];
	[_n_color_r_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 255,0,0 )];
	[_n_color_g_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,255,0 )];
	[_n_color_b_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,0,255 )];


	{
		[self NonnonPaintIconSet:@"rc/small" button:_n_button_0_0];

		[_n_button_0_0 n_enable:TRUE];

		[_n_button_0_0 n_border:TRUE];
		[_n_button_0_0 n_direct_click:YES];
		[_n_button_0_0 n_nswindow_set:_n_toolwindow];

		_n_button_0_0.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/large" button:_n_button_1_0];

		[_n_button_1_0 n_enable:TRUE];
		
		[_n_button_1_0 n_border:TRUE];
		[_n_button_1_0 n_direct_click:YES];
		[_n_button_1_0 n_nswindow_set:_n_toolwindow];

		_n_button_1_0.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/mirror" button:_n_button_2_0];

		[_n_button_2_0 n_enable:TRUE];

		[_n_button_2_0 n_border:TRUE];
		[_n_button_2_0 n_direct_click:YES];
		[_n_button_2_0 n_nswindow_set:_n_toolwindow];

		_n_button_2_0.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/rotate_left" button:_n_button_3_0];

		[_n_button_3_0 n_enable:TRUE];

		[_n_button_3_0 n_border:TRUE];
		[_n_button_3_0 n_direct_click:YES];
		[_n_button_3_0 n_nswindow_set:_n_toolwindow];

		_n_button_3_0.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/rotate_right" button:_n_button_4_0];

		[_n_button_4_0 n_enable:TRUE];

		[_n_button_4_0 n_border:TRUE];
		[_n_button_4_0 n_direct_click:YES];
		[_n_button_4_0 n_nswindow_set:_n_toolwindow];

		_n_button_4_0.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/new" button:_n_button_3_1];

		[_n_button_3_1 n_enable:TRUE];

		[_n_button_3_1 n_border:TRUE];
		[_n_button_3_1 n_direct_click:YES];
		[_n_button_3_1 n_nswindow_set:_n_toolwindow];

		_n_button_3_1.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/save/png" button:_n_button_4_1];

		[_n_button_4_1 n_enable:TRUE];

		[_n_button_4_1 n_border:TRUE];
		[_n_button_4_1 n_direct_click:YES];
		[_n_button_4_1 n_nswindow_set:_n_toolwindow];

		_n_button_4_1.delegate = self;
		_n_button_4_1.delegate_right = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/pen" button:_n_button_0_2];

		[_n_button_0_2 n_enable:TRUE];

		[_n_button_0_2 n_border:TRUE];
		[_n_button_0_2 n_direct_click:YES];
		[_n_button_0_2 n_nswindow_set:_n_toolwindow];

		[_n_button_0_2 n_fake:TRUE];

		_n_button_0_2.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/brush" button:_n_button_1_2];

		[_n_button_1_2 n_enable:TRUE];

		[_n_button_1_2 n_border:TRUE];
		[_n_button_1_2 n_direct_click:YES];
		[_n_button_1_2 n_nswindow_set:_n_toolwindow];

		_n_button_1_2.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/grabber" button:_n_button_2_2];

		[_n_button_2_2 n_enable:TRUE];

		[_n_button_2_2 n_border:TRUE];
		[_n_button_2_2 n_direct_click:YES];
		[_n_button_2_2 n_nswindow_set:_n_toolwindow];

		_n_button_2_2.delegate = self;
	}

	{
		[self NonnonPaintIconSet:@"rc/color" button:_n_button_4_2];

		[_n_button_4_2 n_enable:TRUE];

		[_n_button_4_2 n_border:TRUE];
		[_n_button_4_2 n_direct_click:YES];
		[_n_button_4_2 n_nswindow_set:_n_toolwindow];

		_n_button_4_2.delegate = self;
	}

	_n_grabber_blend_scrollbar.delegate = self;
	[_n_grabber_blend_scrollbar n_scrollbar_parameter:N_PAINT_ID_BLEND step:1 page:10 max:100 pos:paint.grabber_blend redraw:n_posix_true];
	[_n_grabber_blend_scrollbar n_scrollbar_nswindow_set:_n_toolwindow];
	[_n_grabber_blend_value setIntegerValue:paint.grabber_blend];

	[self NonnonPaintGrabberUIOnOff:FALSE];


	{
		NSArray      *paths   = NSSearchPathForDirectoriesInDomains( NSDesktopDirectory, NSUserDomainMask, YES );
		NSString     *desktop = [paths objectAtIndex:0];
		n_posix_char  tmpname[ 100 ]; n_string_path_tmpname( tmpname );

		paint.filename = [NSString stringWithFormat:@"%@/%s.png", desktop, tmpname];
//NSLog( @"%@", path );
	}

	n_bmp_new_fast( &paint.bmp_data, 512,512 );
	n_bmp_flush( &paint.bmp_data, n_bmp_white );

	paint.pen_bmp_data = &paint.bmp_data;
	paint.pen_bmp_grab = &paint.bmp_grab;

	_n_paint_canvas.delegate = self;
	_n_paint_canvas.paint    = &paint;

	[self NonnonPaintResize];
	[self NonnonPaintStatus];
	[self NonnonPaintTitle];


	// Resizer

	n_mac_timer_init( self, @selector( n_paint_delayed_apply_method ), 100 );


	paint.resizer_onoff = FALSE;

	[self NonnonPaintResizerReset];


	// Formatter

	_n_formatter_cur_hotspot_x_scrollbar.delegate = self;
	[_n_formatter_cur_hotspot_x_scrollbar n_scrollbar_parameter:N_PAINT_ID_FORMATTER_CUR_X step:1 page:10 max:255 pos:paint.curico.hotspotx redraw:n_posix_true];
	[_n_formatter_cur_hotspot_x_scrollbar n_scrollbar_nswindow_set:_n_formatter_cur_window];
	[_n_formatter_cur_hotspot_x_value setIntegerValue:paint.curico.hotspotx];

	_n_formatter_cur_hotspot_y_scrollbar.delegate = self;
	[_n_formatter_cur_hotspot_y_scrollbar n_scrollbar_parameter:N_PAINT_ID_FORMATTER_CUR_Y step:1 page:10 max:255 pos:paint.curico.hotspoty redraw:n_posix_true];
	[_n_formatter_cur_hotspot_y_scrollbar n_scrollbar_nswindow_set:_n_formatter_cur_window];
	[_n_formatter_cur_hotspot_y_value setIntegerValue:paint.curico.hotspoty];

	[_n_formatter_icon_cur n_enable:TRUE]; [_n_formatter_icon_cur n_border:TRUE]; [_n_formatter_icon_cur n_nswindow_set:_n_formatter_cur_window];
	[_n_formatter_icon_etc n_enable:TRUE]; [_n_formatter_icon_etc n_border:TRUE]; [_n_formatter_icon_etc n_nswindow_set:_n_formatter_bmp_window];


	// Layer

	layer_suppress_redraw = FALSE;

	layer_click_phase = 0;
	layer_click_msec  = 0;

	paint.layer_onoff  = false;
	paint.layer_count  = N_PAINT_LAYER_MAX;

	paint.layer_data  = n_memory_new( sizeof( n_paint_layer ) * paint.layer_count );
	n_memory_zero( paint.layer_data, sizeof( n_paint_layer ) * paint.layer_count );

	_n_layer_listbox.delegate_option     = N_MAC_TXTBOX_DELEGATE_MOUSEDOWN_LEFT | N_MAC_TXTBOX_DELEGATE_MOUSEUP_RIGHT | N_MAC_TXTBOX_DELEGATE_EDITED | N_MAC_TXTBOX_DELEGATE_LISTBOX_EDITED | N_MAC_TXTBOX_DELEGATE_LISTBOX_MOVED | N_MAC_TXTBOX_DELEGATE_SWAP;
	_n_layer_listbox.delegate            = self;
	_n_layer_listbox.n_mode              = N_MAC_TXTBOX_MODE_LISTBOX;
	_n_layer_listbox.n_txt_data          = &paint.layer_txt;
	_n_layer_listbox.n_txt_deco          = &paint.layer_decoration_txt;
	_n_layer_listbox.n_focus             = 0;
	_n_layer_listbox.n_option_linenumber = N_MAC_TXTBOX_DRAW_LINENUMBER_ZEROBASED_INDEX;

	NSFont *font = n_mac_stdfont();
	[_n_layer_listbox NonnonTxtboxFontChange:font];

	n_layer_listbox_global = _n_layer_listbox;

	[_n_layer_listbox NonnonTxtboxReset];

	_n_layer_blur_scrollbar.delegate  = self;
	_n_layer_blend_scrollbar.delegate = self;

	[_n_layer_blur_scrollbar  n_scrollbar_parameter:N_PAINT_ID_LAYER_BLUR  step:1 page: 5 max: 25 pos:0 redraw:n_posix_true];
	[_n_layer_blur_scrollbar n_scrollbar_nswindow_set:_n_layer_window];

	[_n_layer_blend_scrollbar n_scrollbar_parameter:N_PAINT_ID_LAYER_BLEND step:1 page:10 max:100 pos:0 redraw:n_posix_true];
	[_n_layer_blend_scrollbar n_scrollbar_nswindow_set:_n_layer_window];

	_n_paint_canvas.txtbox = _n_layer_listbox;

	// [!] : on/off state depends NonnonDragAndDrop_dropped method

	paint.whole_preview_checkbox   = _n_layer_whole_preview_checkbox;
	paint.whole_grab_checkbox      = _n_layer_whole_grab_checkbox;
	paint.whole_save_checkbox      = _n_layer_whole_save_checkbox;

	paint.layer_whole_save_onoff = n_posix_true;


	// Clear Canvas

	_n_clear_preview.color = &paint.clear_color;

	_n_clear_a_scrollbar.delegate = self;
	_n_clear_r_scrollbar.delegate = self;
	_n_clear_g_scrollbar.delegate = self;
	_n_clear_b_scrollbar.delegate = self;

	[_n_clear_a_scrollbar n_scrollbar_parameter:N_PAINT_ID_CLEAR_A step:1 page:10 max:255 pos:n_bmp_a( paint.clear_color ) redraw:n_posix_true];
	[_n_clear_r_scrollbar n_scrollbar_parameter:N_PAINT_ID_CLEAR_R step:1 page:10 max:255 pos:n_bmp_b( paint.clear_color ) redraw:n_posix_true];
	[_n_clear_g_scrollbar n_scrollbar_parameter:N_PAINT_ID_CLEAR_G step:1 page:10 max:255 pos:n_bmp_g( paint.clear_color ) redraw:n_posix_true];
	[_n_clear_b_scrollbar n_scrollbar_parameter:N_PAINT_ID_CLEAR_B step:1 page:10 max:255 pos:n_bmp_r( paint.clear_color ) redraw:n_posix_true];

	[_n_clear_a_scrollbar n_scrollbar_nswindow_set:_n_clear_window];
	[_n_clear_r_scrollbar n_scrollbar_nswindow_set:_n_clear_window];
	[_n_clear_g_scrollbar n_scrollbar_nswindow_set:_n_clear_window];
	[_n_clear_b_scrollbar n_scrollbar_nswindow_set:_n_clear_window];

	[_n_clear_r_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 255,0,0 )];
	[_n_clear_g_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,255,0 )];
	[_n_clear_b_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,0,255 )];


	// Color Replacer

	_n_replacer_fr_preview.color = &paint.replacer_fr_color;

	_n_replacer_fr_a_scrollbar.delegate = self;
	_n_replacer_fr_r_scrollbar.delegate = self;
	_n_replacer_fr_g_scrollbar.delegate = self;
	_n_replacer_fr_b_scrollbar.delegate = self;

	[_n_replacer_fr_a_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_FR_A step:1 page:10 max:255 pos:n_bmp_a( paint.replacer_fr_color ) redraw:n_posix_true];
	[_n_replacer_fr_r_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_FR_R step:1 page:10 max:255 pos:n_bmp_b( paint.replacer_fr_color ) redraw:n_posix_true];
	[_n_replacer_fr_g_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_FR_G step:1 page:10 max:255 pos:n_bmp_g( paint.replacer_fr_color ) redraw:n_posix_true];
	[_n_replacer_fr_b_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_FR_B step:1 page:10 max:255 pos:n_bmp_r( paint.replacer_fr_color ) redraw:n_posix_true];

	[_n_replacer_fr_a_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];
	[_n_replacer_fr_r_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];
	[_n_replacer_fr_g_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];
	[_n_replacer_fr_b_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];

	[_n_replacer_fr_r_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 255,0,0 )];
	[_n_replacer_fr_g_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,255,0 )];
	[_n_replacer_fr_b_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,0,255 )];

	_n_replacer_to_preview.color = &paint.replacer_to_color;

	_n_replacer_to_a_scrollbar.delegate = self;
	_n_replacer_to_r_scrollbar.delegate = self;
	_n_replacer_to_g_scrollbar.delegate = self;
	_n_replacer_to_b_scrollbar.delegate = self;

	[_n_replacer_to_a_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_TO_A step:1 page:10 max:255 pos:n_bmp_a( paint.replacer_to_color ) redraw:n_posix_true];
	[_n_replacer_to_r_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_TO_R step:1 page:10 max:255 pos:n_bmp_b( paint.replacer_to_color ) redraw:n_posix_true];
	[_n_replacer_to_g_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_TO_G step:1 page:10 max:255 pos:n_bmp_g( paint.replacer_to_color ) redraw:n_posix_true];
	[_n_replacer_to_b_scrollbar n_scrollbar_parameter:N_PAINT_ID_REPLACER_TO_B step:1 page:10 max:255 pos:n_bmp_r( paint.replacer_to_color ) redraw:n_posix_true];

	[_n_replacer_to_a_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];
	[_n_replacer_to_r_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];
	[_n_replacer_to_g_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];
	[_n_replacer_to_b_scrollbar n_scrollbar_nswindow_set:_n_replacer_window];

	[_n_replacer_to_r_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 255,0,0 )];
	[_n_replacer_to_g_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,255,0 )];
	[_n_replacer_to_b_scrollbar n_scrollbar_gradient:nil from:n_bmp_black to:n_bmp_rgb_mac( 0,0,255 )];


	// Alpha Tweaker

	{
		[self NonnonPaintIconSet:@"rc/switch" button:_n_alpha_tweaker_icon];

		[_n_alpha_tweaker_icon n_enable:TRUE];
		[_n_alpha_tweaker_icon n_border:TRUE];
		[_n_alpha_tweaker_icon n_nswindow_set:_n_alpha_tweaker_window];
	}


	// Preview

	_n_preview_view.delegate = self;


	// Renamer

	[_n_renamer_text setTarget:self];
	[_n_renamer_text setAction:@selector(n_renamer_method:)];


	// Window

	[_window setAcceptsMouseMovedEvents:YES];

	// [Needed] : set every time
	[_window addChildWindow:_n_toolwindow     ordered:NSWindowAbove];
	[_window addChildWindow:_n_resizer_window ordered:NSWindowAbove];
	[_window addChildWindow:_n_layer_window   ordered:NSWindowAbove];

	[[NSNotificationCenter defaultCenter]
	      addObserver: self
		 selector: @selector( windowWillClose: )
		     name: NSWindowWillCloseNotification
		   object: nil
	];

	[[NSNotificationCenter defaultCenter]
	      addObserver: self
		 selector: @selector( WindowWillMiniaturize: )
		     name: NSWindowWillMiniaturizeNotification
		   object: nil
	];

	[[NSDistributedNotificationCenter defaultCenter]
		addObserver: self
		   selector: @selector( accentColorChanged: )
		       name: @"AppleColorPreferencesChangedNotification"
		     object: nil
	];

	// [!] : dark mode
	[[NSDistributedNotificationCenter defaultCenter]
		addObserver: self
		   selector: @selector( darkModeChanged: )
		       name: @"AppleInterfaceThemeChangedNotification"
		     object: nil
	];

}




- (void) accentColorChanged:(NSNotification *)notification
{
//NSLog( @"accentColorChanged" );

	// [x] : Sonoma : buggy : old value is set

	n_mac_timer_init_once( self, @selector( n_timer_method_color ), 200 );

}

- (void) darkModeChanged:(NSNotification *)notification
{
//NSLog( @"darkModeChanged" );

	// [x] : Sonoma : buggy : old value is set

	n_mac_timer_init_once( self, @selector( n_timer_method_color ), 200 );

}

- (void) n_timer_method_color
{
//NSLog( @"n_timer_method_color" );

	[self NonnonPaintIconSet:@"rc/small"        button:_n_button_0_0];
	[self NonnonPaintIconSet:@"rc/large"        button:_n_button_1_0];
	[self NonnonPaintIconSet:@"rc/mirror"       button:_n_button_2_0];
	[self NonnonPaintIconSet:@"rc/rotate_left"  button:_n_button_3_0];
	[self NonnonPaintIconSet:@"rc/rotate_right" button:_n_button_4_0];

	[self NonnonPaintIconSet:@"rc/new"          button:_n_button_3_1];
	[self NonnonPaintIconSet:@"rc/save/png"     button:_n_button_4_1];

	[self NonnonPaintIconSet:@"rc/pen"          button:_n_button_0_2];
	[self NonnonPaintIconSet:@"rc/brush"        button:_n_button_1_2];
	[self NonnonPaintIconSet:@"rc/grabber"      button:_n_button_2_2];
	[self NonnonPaintIconSet:@"rc/color"        button:_n_button_4_2];

	n_win_scrollbar_on_settingchange( [_n_size_scrollbar  n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_mix_scrollbar   n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_boost_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_air_scrollbar   n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_zoom_scrollbar  n_scrollbar_struct_get], 0, n_posix_true );

	n_win_scrollbar_on_settingchange( [_n_color_a_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_color_r_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_color_g_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_color_b_scrollbar n_scrollbar_struct_get], 0, n_posix_true );

	n_win_scrollbar_on_settingchange( [_n_grabber_blend_scrollbar n_scrollbar_struct_get], 0, n_posix_true );

	n_win_scrollbar_on_settingchange( [_n_resizer_rotate_scrollbar     n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_resizer_gamma_scrollbar      n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_resizer_colorwheel_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_resizer_vividness_scrollbar  n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_resizer_sharpness_scrollbar  n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_resizer_contrast_scrollbar   n_scrollbar_struct_get], 0, n_posix_true );

	n_win_scrollbar_on_settingchange( [_n_formatter_cur_hotspot_x_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_formatter_cur_hotspot_y_scrollbar n_scrollbar_struct_get], 0, n_posix_true );

	n_win_scrollbar_on_settingchange( [_n_layer_blur_scrollbar  n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_layer_blend_scrollbar n_scrollbar_struct_get], 0, n_posix_true );

	n_win_scrollbar_on_settingchange( [_n_clear_a_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_clear_r_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_clear_g_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_clear_b_scrollbar n_scrollbar_struct_get], 0, n_posix_true );

	n_win_scrollbar_on_settingchange( [_n_replacer_fr_a_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_replacer_fr_r_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_replacer_fr_g_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_replacer_fr_b_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_replacer_to_a_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_replacer_to_r_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_replacer_to_g_scrollbar n_scrollbar_struct_get], 0, n_posix_true );
	n_win_scrollbar_on_settingchange( [_n_replacer_to_b_scrollbar n_scrollbar_struct_get], 0, n_posix_true );

}




- (void) WindowWillMiniaturize:(NSNotification *)notification
{
	
	if ( paint.resizer_onoff )
	{
		n_mac_window_hide( _n_toolwindow );

		n_mac_window_show( _n_resizer_window );
		[_window addChildWindow:_n_resizer_window ordered:NSWindowAbove];

		n_mac_window_hide( _n_layer_window );
	} else {
		n_mac_window_show( _n_toolwindow );
		[_window addChildWindow:_n_toolwindow ordered:NSWindowAbove];

		n_mac_window_hide( _n_resizer_window );

		if ( paint.layer_onoff )
		{
			n_mac_window_show( _n_layer_window );
			[_window addChildWindow:_n_layer_window ordered:NSWindowAbove];
		} else {
			n_mac_window_hide( _n_layer_window );
		}
	}

}

- (void) windowWillClose:(NSNotification *)notification
{
//NSLog( @"closed" );

	NSWindow *window = notification.object;
	if ( window == self.window )
	{
		[self NonnonSettings:NO];

		[NSApp terminate:self];
	} else
	if ( window == _n_resizer_window )
	{

		[self NonnonPaintResizerExit:resizer_is_ok];

		n_mac_window_show( _n_toolwindow     );
		n_mac_window_hide( _n_resizer_window );

		// [Needed] : set every time
		[_window addChildWindow:_n_toolwindow ordered:NSWindowAbove];

//NSLog( @"Layer On/Off : %d", paint.layer_onoff );
		if ( paint.layer_onoff )
		{
			[_window addChildWindow:_n_layer_window ordered:NSWindowAbove];
		} else {
			n_mac_window_hide( _n_layer_window );
		}

		if ( resizer_is_ok == FALSE )
		{
			n_paint_global.paint->grabber_rect = n_paint_global.paint->grabber_rect_resizer;
		}

		paint.readonly      = FALSE;
		paint.resizer_onoff = FALSE;

		[self NonnonPaintTitle];

		[_n_paint_canvas display_optimized];

	} else
	if ( window == _n_preview_window )
	{
		[self NonnonPaintUIContext:nil onoff:TRUE];
		[self NonnonPaintWindowShowHide:TRUE];
	} else
	if (
		( window == _n_formatter_bmp_window )
		||
		( window == _n_formatter_cur_window )
	)
	{
		paint.readonly = FALSE;

		[self NonnonPaintUIContext:nil onoff:TRUE];
	} else {
		[self NonnonPaintUIContext:nil onoff:TRUE];
	}

	if ( paint.is_about_window )
	{
		paint.is_about_window = FALSE;

		[self NonnonPaintWindowShowHide:TRUE];
	}

}




- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application

	// [x] : macOS Sonoma Xcode 15 : needed at initialization
	[self NonnonPaintWindowShowHide: TRUE];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application

//NSRect rect_window = [_window frame];
//NSLog( @"Window : %f %f", NSWidth( rect_window ), NSHeight( rect_window ) );

//NSRect rect_canvas = [_n_paint_canvas frame];
//NSLog( @"Canvas : %f %f", NSWidth( rect_canvas ), NSHeight( rect_canvas ) );

	paint.init = TRUE;
	[_n_paint_canvas display_optimized];

	[_window makeKeyWindow];

	n_mac_window_centering( _window );
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
}




- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
	return YES;
}




- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Key Code = %d", event.keyCode );

	switch( event.keyCode ) {

	case N_MAC_KEYCODE_F2: 
	{

		// [!] : fork

		n_posix_char *str = n_mac_nsstring2str( paint.filename );
		n_posix_char *dir = n_string_path_upperfolder_new( str );
		n_posix_char *ext = n_string_path_ext_get_new( str );
		n_posix_char *nam = n_string_path_tmpname_new( ext );
		n_posix_char *tmp = n_string_path_make_new( dir, nam );

//NSLog( @"str %@", paint.filename );
//NSLog( @"str %s", str );
//NSLog( @"dir %s", dir );
//NSLog( @"ext %s", ext );
//NSLog( @"nam %s", nam );
//NSLog( @"tmp %s", tmp );

		paint.filename = n_mac_str2nsstring( tmp );

		n_string_free( str );
		n_string_free( dir );
		n_string_free( ext );
		n_string_free( nam );
		n_string_free( tmp );

		[self NonnonPaintTitle];
	}
	break;

	case N_MAC_KEYCODE_NUMBER_1: 

		[self NonnonPaintPreview];

	break;

	case N_MAC_KEYCODE_ARROW_UP   :
	case N_MAC_KEYCODE_ARROW_DOWN :

		[self NonnonPaintLayerSelect:_n_layer_listbox.n_focus];

//NSLog( @"NonnonPaintLayerRefresh : %lld", paint.layer_index );
//break;
		[self NonnonPaintLayerRefresh];

	break;

	} // switch

}




- (IBAction)n_toolwindow_grabber_mode_mehtod:(NSComboBox *)sender {

	int index = (int) [sender indexOfSelectedItem];

	if ( index == 0 )
	{
//NSLog( @"index 0" );
		paint.grabber_frame_mode = N_PAINT_GRABBER_FRAME_MODE_EDGE;
	} else
	if ( index == 1 )
	{
//NSLog( @"index 1" );
		paint.grabber_frame_mode = N_PAINT_GRABBER_FRAME_MODE_PIXEL;
	}

	[_n_paint_canvas display_optimized];

}




- (IBAction)n_menu_preview_method:(id)sender {

	[self NonnonPaintPreview];

}

- (IBAction)n_menu_grid_method:(NSMenuItem *)sender {

	NSControlStateValue s = [_n_menu_grid state];
	if ( s == NSControlStateValueOff )
	{
		[_n_menu_grid setState:NSControlStateValueOn];
		paint.grid_onoff = TRUE;
	} else {
		[_n_menu_grid setState:NSControlStateValueOff];
		paint.grid_onoff = FALSE;
	}

	[_n_paint_canvas display_optimized];

}

- (IBAction)n_menu_pixelgrid_method:(id)sender {

	NSControlStateValue s = [_n_menu_pixelgrid state];
	if ( s == NSControlStateValueOff )
	{
		[_n_menu_pixelgrid setState:NSControlStateValueOn];
		paint.pixel_grid_onoff = TRUE;
	} else {
		[_n_menu_pixelgrid setState:NSControlStateValueOff];
		paint.pixel_grid_onoff = FALSE;
	}

	[_n_paint_canvas display_optimized];

}

- (IBAction)n_menu_alpha_emphasizer_method:(id)sender {

	NSControlStateValue s = [_n_menu_alpha_emphasizer state];
	if ( s == NSControlStateValueOff )
	{
		[_n_menu_alpha_emphasizer setState:NSControlStateValueOn];
		paint.alpha_emphasizer_onoff = TRUE;
	} else {
		[_n_menu_alpha_emphasizer setState:NSControlStateValueOff];
		paint.alpha_emphasizer_onoff = FALSE;
	}

	[_n_paint_canvas display_optimized];

}

- (IBAction)n_menu_clear_canvas_method:(id)sender {

	paint.clear_color = paint.color;
	[self NonnonPaintClearCanvasColorSet];

	[self NonnonPaintUIContext:_n_clear_window onoff:FALSE];

	n_mac_window_show( _n_clear_window );

}

- (IBAction)n_clear_go_method:(id)sender {

	u32 color = n_bmp_argb_mac(
		(int) [_n_clear_a_value integerValue],
		(int) [_n_clear_r_value integerValue],
		(int) [_n_clear_g_value integerValue],
		(int) [_n_clear_b_value integerValue]
	);


//n_paint_bmp_flush_alpha_base_color( paint.pen_bmp_data, color );

	if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		n_bmp_flush( paint.pen_bmp_data, color );
	} else {
		n_bmp_flush( paint.pen_bmp_grab, color );

		paint.grabber_frame_lock = TRUE;
	}

	[_n_paint_canvas display_optimized];

	n_mac_window_hide( _n_clear_window );

	[self NonnonPaintUIContext:_n_clear_window onoff:TRUE];

}

- (IBAction)n_replacer_method:(id)sender {

	paint.replacer_fr_color = paint.color;
	paint.replacer_to_color = paint.color;

	[self NonnonPaintColorReplacerColorSet];

	[self NonnonPaintUIContext:_n_replacer_window onoff:FALSE];

	n_mac_window_show( _n_replacer_window );

}

- (IBAction)n_replacer_go_method:(id)sender {

	u32 color_fr = n_bmp_argb_mac(
		(int) [_n_replacer_fr_a_value integerValue],
		(int) [_n_replacer_fr_r_value integerValue],
		(int) [_n_replacer_fr_g_value integerValue],
		(int) [_n_replacer_fr_b_value integerValue]
	);

	u32 color_to = n_bmp_argb_mac(
		(int) [_n_replacer_to_a_value integerValue],
		(int) [_n_replacer_to_r_value integerValue],
		(int) [_n_replacer_to_g_value integerValue],
		(int) [_n_replacer_to_b_value integerValue]
	);

	if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		n_bmp_flush_replacer( paint.pen_bmp_data, color_fr, color_to );
	} else {
		n_bmp_flush_replacer( paint.pen_bmp_grab, color_fr, color_to );

		paint.grabber_frame_lock = TRUE;
	}

	[_n_paint_canvas display_optimized];

	n_mac_window_hide( _n_replacer_window );

	[self NonnonPaintUIContext:_n_replacer_window onoff:TRUE];

}

- (IBAction)n_formatter_go_method:(NSButton *)sender {

	paint.curico.hotspotx = (int) [_n_formatter_cur_hotspot_x_value integerValue];
	paint.curico.hotspoty = (int) [_n_formatter_cur_hotspot_y_value integerValue];

	n_mac_window_hide( _n_formatter_cur_window );

	[self NonnonPaintSave];

	paint.readonly = FALSE;

	[self NonnonPaintUIContext:nil onoff:TRUE];

	if ( sheet_onoff )
	{
		[self NonnonPaintWindowShowHide:TRUE];
		[_window endSheet:_n_formatter_cur_window];
	}

}

- (IBAction)n_formatter_bmp_go_method:(id)sender {

	n_mac_window_hide( _n_formatter_bmp_window );

	[self NonnonPaintSave];

	paint.readonly = FALSE;

	[self NonnonPaintUIContext:nil onoff:TRUE];

	if ( sheet_onoff )
	{
		[self NonnonPaintWindowShowHide:TRUE];
		[_window endSheet:_n_formatter_bmp_window];
	}

}

- (IBAction)n_formatter_cancel_method:(id)sender {

	n_mac_window_hide( _n_formatter_cur_window );

	paint.readonly = FALSE;

	[self NonnonPaintUIContext:nil onoff:TRUE];

	if ( sheet_onoff )
	{
		[self NonnonPaintWindowShowHide:TRUE];
		[_window endSheet:_n_formatter_cur_window];
	}

}

- (IBAction)n_formatter_bmp_cancel_method:(id)sender {

	n_mac_window_hide( _n_formatter_bmp_window );

	paint.readonly = FALSE;

	[self NonnonPaintUIContext:nil onoff:TRUE];

	if ( sheet_onoff )
	{
		[self NonnonPaintWindowShowHide:TRUE];
		[_window endSheet:_n_formatter_bmp_window];
	}

}

- (IBAction)n_menu_alpha_tweaker_method:(id)sender {


	[self NonnonPaintUIContext:_n_alpha_tweaker_window onoff:FALSE];

	n_mac_window_show( _n_alpha_tweaker_window );

}

- (IBAction)n_alpha_tweaker_go_method:(id)sender {

	NSControlStateValue state_clear   = [_n_alpha_tweaker_clear_checkbox state];
	NSControlStateValue state_reverse = [_n_alpha_tweaker_reverse_checkbox state];

	BOOL onoff_clear;
	BOOL onoff_reverse;

	if ( state_clear == NSControlStateValueOff )
	{
		onoff_clear = FALSE;
	} else {
		onoff_clear = TRUE;
	}

	if ( state_reverse == NSControlStateValueOff )
	{
		onoff_reverse = FALSE;
	} else {
		onoff_reverse = TRUE;
	}

//NSLog( @"Clear %d : Reverse %d", onoff_clear, onoff_reverse );


	if ( ( onoff_clear )||( onoff_reverse ) )
	{
		paint.grabber_frame_lock = TRUE;
	}


	if ( onoff_clear )
	{
//NSLog( @"Clear" );
		n_paint_layer_bmp_alpha( N_PAINT_FILTER_ALPHA_CLR );
		[_n_paint_canvas display_optimized];
	}

	if ( onoff_reverse )
	{
//NSLog( @"Reverse" );
		n_paint_layer_bmp_alpha( N_PAINT_FILTER_ALPHA_REV );
		[_n_paint_canvas display_optimized];
	}


	n_mac_window_hide( _n_alpha_tweaker_window );

	[self NonnonPaintUIContext:_n_alpha_tweaker_window onoff:TRUE];

}

- (IBAction)n_menu_readme:(id)sender {

	NSString *helpFilePath = [[NSBundle mainBundle] pathForResource:@"nonnon_paint" ofType:@"html"];
	NSURL    *helpFileURL  = [NSURL fileURLWithPath:helpFilePath];

	[[NSWorkspace sharedWorkspace] openURL:helpFileURL];

}

- (IBAction)n_menu_about:(id)sender {
//NSLog( @"n_menu_about" );

	[paint.cursor_arrow set];

	paint.is_about_window = TRUE;
	[self NonnonPaintWindowShowHide:FALSE];

	[self NonnonPaintUIContext:nil onoff:FALSE];

	[NSApp orderFrontStandardAboutPanel:sender];

}



- (void) NonnonScrollbarZoomSync:(BOOL)redraw
{

	n_win_scrollbar *scr = [_n_zoom_scrollbar n_scrollbar_struct_get];
	scr->unit_pos = 50 + paint.zoom;

	if ( redraw ) { [_n_zoom_scrollbar display]; }

}

- (void) n_paint_delayed_apply_method_main
{

	if ( delayed_apply_id == N_PAINT_ID_LAYER_BLUR )
	{
		[self NonnonPaintLayerSync];
	} else
	if ( delayed_apply_id == N_PAINT_ID_LAYER_BLEND )
	{
		[self NonnonPaintLayerSync];
	} else
	if ( delayed_apply_id != 0 )
	{
		[self n_resizer_go];
	}

}

- (void) n_paint_delayed_apply_method
{

	if ( delayed_apply_queue == FALSE ) { return; }

	if ( n_game_timer( &delayed_apply_timer, 500 ) )
	{
		[self n_paint_delayed_apply_method_main];

		delayed_apply_id    = 0;
		delayed_apply_queue = FALSE;
		//delayed_apply_timer = n_posix_tickcount();
	}

}

- (void) n_paint_delayed_apply_timer_go:(int) n_id reason:(int) reason
{
//NSLog( @"%d", reason );

	if ( reason == N_WIN_SCROLLBAR_REASON_DRAG_DRAGGED ) { return; }

	delayed_apply_id    = n_id;
	delayed_apply_queue = TRUE;
	delayed_apply_timer = n_posix_tickcount();

//[self n_paint_delayed_apply_method_main];

}

- (void) NonnonScrollbarMessage:(int) n_id  value:(int) v reason:(int) reason
{
//NSLog( @"%d %d %d", n_id, v, reason );

	if ( n_id == N_PAINT_ID_PENSIZE )
	{
		paint.pensize = v;
		if ( ( v % 2 ) == 0 ) { paint.pensize++; }
		[_n_size_value setIntegerValue:paint.pensize];
	} else
	if ( n_id == N_PAINT_ID_MIX )
	{
		paint.mix = v;
		[_n_mix_value setIntegerValue:paint.mix];
	} else
	if ( n_id == N_PAINT_ID_BOOST )
	{
		paint.boost = v;
		[_n_boost_value setIntegerValue:paint.boost];
	} else
	if ( n_id == N_PAINT_ID_AIR )
	{
		paint.air = v;
		[_n_air_value setIntegerValue:paint.air];
	} else
	if ( n_id == N_PAINT_ID_ZOOM )
	{
//NSLog( @"%d", v );

		static int prev = 0;

		paint.zoom = v - 50;
//NSLog( @"Prv %d : Cur %d", prev, paint.zoom );

		if ( 1 >= abs( prev - paint.zoom ) )
		{
			if ( paint.zoom == -1 )
			{
				paint.zoom =  1;

				[self NonnonScrollbarZoomSync:FALSE];
			} else
			if ( paint.zoom == 0 )
			{
				if ( prev < 0 )
				{
					paint.zoom =  1;
				} else {
					paint.zoom = -2;
				}

				[self NonnonScrollbarZoomSync:FALSE];
			}
		} else {
			if ( paint.zoom == 0 )
			{
				paint.zoom =  1;
			} else
			if ( ( prev == 1 )&&( paint.zoom < 0 ) )
			{
				//
			} else
			if ( ( prev > 0 )&&( paint.zoom < 0 ) )
			{
				paint.zoom =  1;
			} else
			if ( ( prev < 0 )&&( paint.zoom > 0 ) )
			{
				paint.zoom =  1;
			}

			[self NonnonScrollbarZoomSync:FALSE];
		}

		prev = paint.zoom;

		[self NonnonPaintZoomUI];

	} else
	if ( n_id == N_PAINT_ID_COLOR_A )
	{
		[_n_color_a_value setIntegerValue:v];

		int a = v;
		int r = n_bmp_b( paint.color );
		int g = n_bmp_g( paint.color );
		int b = n_bmp_r( paint.color );

		paint.color = n_bmp_argb_mac( a,r,g,b );

		[_n_color_preview display];
	} else
	if ( n_id == N_PAINT_ID_COLOR_R )
	{
		[_n_color_r_value setIntegerValue:v];

		int a = n_bmp_a( paint.color );
		int r = v;
		int g = n_bmp_g( paint.color );
		int b = n_bmp_r( paint.color );

		paint.color = n_bmp_argb_mac( a,r,g,b );

		[_n_color_preview display];
	} else
	if ( n_id == N_PAINT_ID_COLOR_G )
	{
		[_n_color_g_value setIntegerValue:v];

		int a = n_bmp_a( paint.color );
		int r = n_bmp_b( paint.color );
		int g = v;
		int b = n_bmp_r( paint.color );

		paint.color = n_bmp_argb_mac( a,r,g,b );

		[_n_color_preview display];
	} else
	if ( n_id == N_PAINT_ID_COLOR_B )
	{
		[_n_color_b_value setIntegerValue:v];

		int a = n_bmp_a( paint.color );
		int r = n_bmp_b( paint.color );
		int g = n_bmp_g( paint.color );
		int b = v;

		paint.color = n_bmp_argb_mac( a,r,g,b );

		[_n_color_preview display];
	} else
	if ( n_id == N_PAINT_ID_BLEND )
	{

		paint.grabber_blend = v;
		[_n_grabber_blend_value setIntegerValue:paint.grabber_blend];

		paint.grabber_blend_ratio = (double) paint.grabber_blend * 0.01;

		[_n_paint_canvas display_optimized];
	} else
	if ( n_id == N_PAINT_ID_RESIZER_ROTATE )
	{
		resizer_rotate = v;

		NSString *nsstr = [NSString stringWithFormat:@"%d°", resizer_rotate-360];
		[_n_resizer_rotate_value setStringValue:nsstr];

		[self n_paint_delayed_apply_timer_go:n_id reason:reason];
	} else
	if ( n_id == N_PAINT_ID_RESIZER_GAMMA )
	{
		resizer_gamma = v;

		NSString *nsstr = [NSString stringWithFormat:@"%0.1f", (double) ( resizer_gamma - 10 ) * 0.1];
		[_n_resizer_gamma_value setStringValue:nsstr];

		[self n_paint_delayed_apply_timer_go:n_id reason:reason];
	} else
	if ( n_id == N_PAINT_ID_RESIZER_COLORWHEEL )
	{
		[_n_resizer_colorwheel_value setIntegerValue:v - 128];

		[self n_paint_delayed_apply_timer_go:n_id reason:reason];
	} else
	if ( n_id == N_PAINT_ID_RESIZER_VIVIDNESS )
	{
		[_n_resizer_vividness_value setIntegerValue:v - 100];

		[self n_paint_delayed_apply_timer_go:n_id reason:reason];
	} else
	if ( n_id == N_PAINT_ID_RESIZER_SHARPNESS )
	{
		[_n_resizer_sharpness_value setIntegerValue:v - 100];

		[self n_paint_delayed_apply_timer_go:n_id reason:reason];
	} else
	if ( n_id == N_PAINT_ID_RESIZER_CONTRAST )
	{
		[_n_resizer_contrast_value setIntegerValue:v];

		[self n_paint_delayed_apply_timer_go:n_id reason:reason];
	} else
	if ( n_id == N_PAINT_ID_FORMATTER_CUR_X )
	{
		[_n_formatter_cur_hotspot_x_value setIntegerValue:v];
	} else
	if ( n_id == N_PAINT_ID_FORMATTER_CUR_Y )
	{
		[_n_formatter_cur_hotspot_y_value setIntegerValue:v];
	} else
	if ( n_id == N_PAINT_ID_LAYER_BLUR )
	{
		[_n_layer_blur_value setIntegerValue:v];
		if ( layer_suppress_redraw == FALSE )
		{
			[self n_paint_delayed_apply_timer_go:n_id reason:reason];
		}
	} else
	if ( n_id == N_PAINT_ID_LAYER_BLEND )
	{
		[_n_layer_blend_value setIntegerValue:v];
		if ( layer_suppress_redraw == FALSE )
		{
			[self n_paint_delayed_apply_timer_go:n_id reason:reason];
		}
	} else
	if ( n_id == N_PAINT_ID_CLEAR_A )
	{
		[_n_clear_a_value setIntegerValue:v];

		int a = v;
		int r = n_bmp_b( paint.clear_color );
		int g = n_bmp_g( paint.clear_color );
		int b = n_bmp_r( paint.clear_color );

		paint.clear_color = n_bmp_argb_mac( a,r,g,b );

		[_n_clear_preview display];
	} else
	if ( n_id == N_PAINT_ID_CLEAR_R )
	{
		[_n_clear_r_value setIntegerValue:v];

		int a = n_bmp_a( paint.clear_color );
		int r = v;
		int g = n_bmp_g( paint.clear_color );
		int b = n_bmp_r( paint.clear_color );

		paint.clear_color = n_bmp_argb_mac( a,r,g,b );

		[_n_clear_preview display];
	} else
	if ( n_id == N_PAINT_ID_CLEAR_G )
	{
		[_n_clear_g_value setIntegerValue:v];

		int a = n_bmp_a( paint.clear_color );
		int r = n_bmp_b( paint.clear_color );
		int g = v;
		int b = n_bmp_r( paint.clear_color );

		paint.clear_color = n_bmp_argb_mac( a,r,g,b );

		[_n_clear_preview display];
	} else
	if ( n_id == N_PAINT_ID_CLEAR_B )
	{
		[_n_clear_b_value setIntegerValue:v];

		int a = n_bmp_a( paint.clear_color );
		int r = n_bmp_b( paint.clear_color );
		int g = n_bmp_g( paint.clear_color );
		int b = v;

		paint.clear_color = n_bmp_argb_mac( a,r,g,b );

		[_n_clear_preview display];
	} else

	if ( n_id == N_PAINT_ID_REPLACER_FR_A )
	{
		[_n_replacer_fr_a_value setIntegerValue:v];

		int a = v;
		int r = n_bmp_b( paint.replacer_fr_color );
		int g = n_bmp_g( paint.replacer_fr_color );
		int b = n_bmp_r( paint.replacer_fr_color );

		paint.replacer_fr_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_fr_preview display];
	} else
	if ( n_id == N_PAINT_ID_REPLACER_FR_R )
	{
		[_n_replacer_fr_r_value setIntegerValue:v];

		int a = n_bmp_a( paint.replacer_fr_color );
		int r = v;
		int g = n_bmp_g( paint.replacer_fr_color );
		int b = n_bmp_r( paint.replacer_fr_color );

		paint.replacer_fr_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_fr_preview display];
	} else
	if ( n_id == N_PAINT_ID_REPLACER_FR_G )
	{
		[_n_replacer_fr_g_value setIntegerValue:v];

		int a = n_bmp_a( paint.replacer_fr_color );
		int r = n_bmp_b( paint.replacer_fr_color );
		int g = v;
		int b = n_bmp_r( paint.replacer_fr_color );

		paint.replacer_fr_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_fr_preview display];
	} else
	if ( n_id == N_PAINT_ID_REPLACER_FR_B )
	{
		[_n_replacer_fr_b_value setIntegerValue:v];

		int a = n_bmp_a( paint.replacer_fr_color );
		int r = n_bmp_b( paint.replacer_fr_color );
		int g = n_bmp_g( paint.replacer_fr_color );
		int b = v;

		paint.replacer_fr_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_fr_preview display];
	} else

	if ( n_id == N_PAINT_ID_REPLACER_TO_A )
	{
		[_n_replacer_to_a_value setIntegerValue:v];

		int a = v;
		int r = n_bmp_b( paint.replacer_to_color );
		int g = n_bmp_g( paint.replacer_to_color );
		int b = n_bmp_r( paint.replacer_to_color );

		paint.replacer_to_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_to_preview display];
	} else
	if ( n_id == N_PAINT_ID_REPLACER_TO_R )
	{
		[_n_replacer_to_r_value setIntegerValue:v];

		int a = n_bmp_a( paint.replacer_to_color );
		int r = v;
		int g = n_bmp_g( paint.replacer_to_color );
		int b = n_bmp_r( paint.replacer_to_color );

		paint.replacer_to_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_to_preview display];
	} else
	if ( n_id == N_PAINT_ID_REPLACER_TO_G )
	{
		[_n_replacer_to_g_value setIntegerValue:v];

		int a = n_bmp_a( paint.replacer_to_color );
		int r = n_bmp_b( paint.replacer_to_color );
		int g = v;
		int b = n_bmp_r( paint.replacer_to_color );

		paint.replacer_to_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_to_preview display];
	} else
	if ( n_id == N_PAINT_ID_REPLACER_TO_B )
	{
		[_n_replacer_to_b_value setIntegerValue:v];

		int a = n_bmp_a( paint.replacer_to_color );
		int r = n_bmp_b( paint.replacer_to_color );
		int g = n_bmp_g( paint.replacer_to_color );
		int b = v;

		paint.replacer_to_color = n_bmp_argb_mac( a,r,g,b );

		[_n_replacer_to_preview display];
	}// else

}




- (void) n_toolwindow_mouseDown:(NSEvent*) theEvent
{

	if ( FALSE == n_mac_window_is_keywindow( _n_toolwindow ) ) { return; }

	if ( [_n_button_0_0 n_is_pressed] )
	{
		n_paint_grabber_filter( N_PAINT_FILTER_SCALE_LIL );
		[_n_paint_canvas display_optimized];
		[self NonnonPaintTitle];
	} else
	if ( [_n_button_1_0 n_is_pressed] )
	{
		n_paint_grabber_filter( N_PAINT_FILTER_SCALE_BIG );
		[_n_paint_canvas display_optimized];
		[self NonnonPaintTitle];
	} else
	if ( [_n_button_2_0 n_is_pressed] )
	{
		n_paint_grabber_filter( N_PAINT_FILTER_MIRROR );
		[_n_paint_canvas display_optimized];
	} else
	if ( [_n_button_3_0 n_is_pressed] )
	{
		n_paint_grabber_filter( N_PAINT_FILTER_ROTATE_L );
		[_n_paint_canvas display_optimized];
		[self NonnonPaintTitle];
	} else
	if ( [_n_button_4_0 n_is_pressed] )
	{
		n_paint_grabber_filter( N_PAINT_FILTER_ROTATE_R );
		[_n_paint_canvas display_optimized];
		[self NonnonPaintTitle];
	} else

	if ( [_n_button_0_2 n_is_pressed] )
	{
//NSLog( @"%d", n_mac_window_is_keywindow( _n_toolwindow ) );

		[self NonnonPaintIconSet:@"rc/grabber" button:_n_button_2_2];

		if ( n_paint_global.paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			[self NonnonPaintGrabberUIOnOff:FALSE];
		}

		paint.tooltype = N_PAINT_TOOL_TYPE_PEN;

		[_n_button_0_2 n_fake: TRUE]; [_n_button_0_2 display];
		[_n_button_1_2 n_fake:FALSE]; [_n_button_1_2 display];
		[_n_button_2_2 n_fake:FALSE]; [_n_button_2_2 display];

		[self NonnonPaintStatus];

		[n_paint_global display_optimized];
	} else
	if ( [_n_button_1_2 n_is_pressed] )
	{
		[self NonnonPaintIconSet:@"rc/grabber" button:_n_button_2_2];

		if ( n_paint_global.paint->grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			[self NonnonPaintGrabberUIOnOff:FALSE];
		}

		paint.tooltype = N_PAINT_TOOL_TYPE_FILL;

		[_n_button_0_2 n_fake:FALSE]; [_n_button_0_2 display];
		[_n_button_1_2 n_fake: TRUE]; [_n_button_1_2 display];
		[_n_button_2_2 n_fake:FALSE]; [_n_button_2_2 display];

		[self NonnonPaintStatus];

		[n_paint_global display_optimized];
	} else
	if ( [_n_button_2_2 n_is_pressed] )
	{
		[self NonnonPaintIconSet:@"rc/grabber_close" button:_n_button_2_2];

		[self NonnonPaintGrabberUIOnOff:TRUE];

		if ( paint.tooltype == N_PAINT_TOOL_TYPE_GRABBER )
		{
			n_paint_grabber_reset();
		}

		paint.tooltype = N_PAINT_TOOL_TYPE_GRABBER;

		[_n_button_0_2 n_fake:FALSE]; [_n_button_0_2 display];
		[_n_button_1_2 n_fake:FALSE]; [_n_button_1_2 display];
		[_n_button_2_2 n_fake: TRUE]; [_n_button_2_2 display];

		[self NonnonPaintStatus];

		[n_paint_global display_optimized];
	} else
	if ( [_n_button_4_2 n_is_pressed] )
	{
//NSLog( @"!" );

		[_n_colorhistory_canvas display];
		n_mac_window_show( _n_colorhistory_window );

/*
		// [x] : Color Well : unknown warning

		// [!] : for traditional Mac mice like Magic Mouse
		NSUInteger flags = [[NSApp currentEvent] modifierFlags];
		if ( flags & NSEventModifierFlagShift )
		{
			[_n_colorhistory_canvas display];
			n_mac_window_show( _n_colorhistory_window );
		} else {
			NSColorPanel *cp = [NSColorPanel sharedColorPanel];
			[cp setTarget:self];
			cp.showsAlpha = YES;
			[cp orderFront:nil];
			[cp setAction:@selector(colorUpdate:)];
		}
*/
	}// else

}

- (void) n_toolwindow_mouseUp:(NSEvent*) theEvent
{
//NSLog( @"n_toolwindow_mouseUp" ); return;

	if ( FALSE == n_mac_window_is_keywindow( _n_toolwindow ) ) { return; }

//NSLog( @"%d", [_n_button_3_1 n_is_pressed] );
	if ( [_n_button_3_1 n_is_pressed] )
	{

		if ( n_posix_false == [_n_button_3_1 n_is_enabled] ) { return; }

		[self NonnonPaintResizerReset];
		[self NonnonPaintResizerInit ];

		n_mac_window_hide( _n_toolwindow     );
		n_mac_window_show( _n_resizer_window );

		// [Needed] : set every time
		[_window addChildWindow:_n_resizer_window ordered:NSWindowAbove];

		// [Needed] : hide always
//NSLog( @"Layer On/Off : %d", paint.layer_onoff );
		if ( paint.layer_onoff )
		{
			n_mac_window_hide( _n_layer_window );
		} else {
			n_mac_window_hide( _n_layer_window );
		}

		paint.readonly      = TRUE;
		paint.resizer_onoff = TRUE;

//NSLog( @"%d", paint.readonly );
	} else
	if ( [_n_button_4_1 n_is_pressed] )
	{

		if ( n_posix_false == [_n_button_4_1 n_is_enabled] ) { return; }


		// [!] : for traditional Mac mice like Magic Mouse
		NSUInteger flags = [[NSApp currentEvent] modifierFlags];
		if ( flags & NSEventModifierFlagShift )
		{
			[self n_toolwindow_rightMouseUp:theEvent];
			return;
		}


		paint.readonly = TRUE;


		[self NonnonPaintUIContext:nil onoff:FALSE];


		n_posix_char *path = n_mac_nsstring2str( paint.filename );

		if ( n_string_path_ext_is_same_literal( ".CUR\0\0", path ) )
		{
			[_n_formatter_cur_hotspot_x_value setIntegerValue:paint.curico.hotspotx];
			[_n_formatter_cur_hotspot_y_value setIntegerValue:paint.curico.hotspoty];

			[_n_formatter_cur_hotspot_x_scrollbar n_scrollbar_position_set:paint.curico.hotspotx redraw:YES];
			[_n_formatter_cur_hotspot_y_scrollbar n_scrollbar_position_set:paint.curico.hotspotx redraw:YES];

			[self NonnonPaintIconDisplay:_n_formatter_icon_cur];

			if ( sheet_onoff )
			{
				[self NonnonPaintWindowShowHide:FALSE];
				[_window beginSheet:_n_formatter_cur_window completionHandler:nil];
			} else {
				n_mac_topmost( _n_formatter_cur_window, TRUE );
				n_mac_window_show( _n_formatter_cur_window );
			}
		} else {
			[self NonnonPaintIconDisplay:_n_formatter_icon_etc];

			if ( sheet_onoff )
			{
				[self NonnonPaintWindowShowHide:FALSE];
				[_window beginSheet:_n_formatter_bmp_window completionHandler:nil];
			} else {
				n_mac_topmost( _n_formatter_bmp_window, TRUE );
				n_mac_window_show( _n_formatter_bmp_window );
			}
		}

		n_string_free( path );

	}

}

- (void) n_layer_window_mouseDown:(NSEvent*) theEvent
{
//NSLog( @"n_layer_window_mouseDown" );

	if ( FALSE == n_mac_window_is_keywindow( _n_layer_window ) ) { return; }
//NSLog( @"n_layer_window_mouseDown" );

	if ( _n_layer_listbox.n_listbox_edit_onoff ) { return; }


	if ( n_mac_window_is_hovered( _n_layer_listbox ) )
	{
//NSLog( @"n_layer_window_mouseDown" );

		[self NonnonPaintLayerSelect:_n_layer_listbox.n_focus];


		BOOL double_click_onoff = FALSE;

		if ( layer_click_phase == 0 )
		{
			layer_click_phase = 1;
			layer_click_msec  = n_posix_tickcount();
		} else
		if ( layer_click_phase == 1 )
		{
			if ( n_posix_false == n_game_timer_once( &layer_click_msec, 500 ) )
			{
				double_click_onoff = TRUE;
				layer_click_phase = 0;
			} else {
				layer_click_msec  = n_posix_tickcount();
			}
		}

		//double_click_onoff = ( ( [theEvent clickCount] % 3 ) == 2 );

		if ( double_click_onoff )
		{
//NSLog( @"n_layer_window_mouseDown" );

			if ( paint.layer_whole_grab_onoff )
			{
				if ( paint.grabber_mode != N_PAINT_GRABBER_NEUTRAL ) { return; }
			}

			n_posix_char *str = n_txt_get( &paint.layer_decoration_txt, paint.layer_index );
//NSLog( @"%s", str );

			if ( paint.layer_data[ paint.layer_index ].visible )
			{
				paint.layer_data[ paint.layer_index ].visible = FALSE;
				str[ 0 ] = ' ';
			} else {
				paint.layer_data[ paint.layer_index ].visible = TRUE;
				str[ 0 ] = 'B';
			}

			[_n_paint_canvas display_optimized];
		}


		[_n_paint_canvas n_paint_canvas_reset_cache_layer];


		[self NonnonPaintLayerMenuContext];


		[self NonnonPaintLayerRefresh];

	}

}

- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog( @"mouseDown" );

	[self n_layer_window_mouseDown:theEvent];

	[self n_toolwindow_mouseDown:theEvent];

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog(@"delegate : mouseUp");

	[_n_paint_canvas mouseUp:theEvent];

	[self n_toolwindow_mouseUp:theEvent];

//return;

	[_n_size_scrollbar  mouseUp:theEvent];
	[_n_mix_scrollbar   mouseUp:theEvent];
	[_n_boost_scrollbar mouseUp:theEvent];
	[_n_air_scrollbar   mouseUp:theEvent];
	[_n_zoom_scrollbar  mouseUp:theEvent];

	[_n_color_a_scrollbar mouseUp:theEvent];
	[_n_color_r_scrollbar mouseUp:theEvent];
	[_n_color_g_scrollbar mouseUp:theEvent];
	[_n_color_b_scrollbar mouseUp:theEvent];

	[_n_grabber_blend_scrollbar mouseUp:theEvent];

	[_n_resizer_rotate_scrollbar     mouseUp:theEvent];
	[_n_resizer_gamma_scrollbar      mouseUp:theEvent];
	[_n_resizer_colorwheel_scrollbar mouseUp:theEvent];
	[_n_resizer_vividness_scrollbar  mouseUp:theEvent];
	[_n_resizer_sharpness_scrollbar  mouseUp:theEvent];
	[_n_resizer_contrast_scrollbar   mouseUp:theEvent];

	[_n_formatter_cur_hotspot_x_scrollbar mouseUp:theEvent];
	[_n_formatter_cur_hotspot_y_scrollbar mouseUp:theEvent];

	[_n_layer_blur_scrollbar  mouseUp:theEvent];
	[_n_layer_blend_scrollbar mouseUp:theEvent];

	[_n_clear_a_scrollbar mouseUp:theEvent];
	[_n_clear_r_scrollbar mouseUp:theEvent];
	[_n_clear_g_scrollbar mouseUp:theEvent];
	[_n_clear_b_scrollbar mouseUp:theEvent];

	[_n_replacer_fr_a_scrollbar mouseUp:theEvent];
	[_n_replacer_fr_r_scrollbar mouseUp:theEvent];
	[_n_replacer_fr_g_scrollbar mouseUp:theEvent];
	[_n_replacer_fr_b_scrollbar mouseUp:theEvent];
	[_n_replacer_to_a_scrollbar mouseUp:theEvent];
	[_n_replacer_to_r_scrollbar mouseUp:theEvent];
	[_n_replacer_to_g_scrollbar mouseUp:theEvent];
	[_n_replacer_to_b_scrollbar mouseUp:theEvent];


	if ( n_mac_window_is_keywindow( _n_preview_window ) )
	{
		n_type_gfx csx = n_paint_preview_default_sx;
		n_type_gfx csy = n_paint_preview_default_sy;

		[_n_preview_window setContentSize:NSMakeSize( csx,csy )];
		[_n_preview_view setFrame:NSMakeRect( 0,0,csx,csy )];

		[_n_preview_view display];

		n_mac_window_centering( _n_preview_window );
	}

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog(@"delegate : mouseDragged");

	[_n_paint_canvas mouseDragged:theEvent];

//return;

	[_n_size_scrollbar  mouseDragged:theEvent];
	[_n_mix_scrollbar   mouseDragged:theEvent];
	[_n_boost_scrollbar mouseDragged:theEvent];
	[_n_air_scrollbar   mouseDragged:theEvent];
	[_n_zoom_scrollbar  mouseDragged:theEvent];

	[_n_color_a_scrollbar mouseDragged:theEvent];
	[_n_color_r_scrollbar mouseDragged:theEvent];
	[_n_color_g_scrollbar mouseDragged:theEvent];
	[_n_color_b_scrollbar mouseDragged:theEvent];

	[_n_grabber_blend_scrollbar mouseDragged:theEvent];

	[_n_resizer_rotate_scrollbar     mouseDragged:theEvent];
	[_n_resizer_gamma_scrollbar      mouseDragged:theEvent];
	[_n_resizer_colorwheel_scrollbar mouseDragged:theEvent];
	[_n_resizer_vividness_scrollbar  mouseDragged:theEvent];
	[_n_resizer_sharpness_scrollbar  mouseDragged:theEvent];
	[_n_resizer_contrast_scrollbar   mouseDragged:theEvent];

	[_n_formatter_cur_hotspot_x_scrollbar mouseDragged:theEvent];
	[_n_formatter_cur_hotspot_y_scrollbar mouseDragged:theEvent];

	[_n_layer_listbox         mouseDragged:theEvent];
	[_n_layer_blur_scrollbar  mouseDragged:theEvent];
	[_n_layer_blend_scrollbar mouseDragged:theEvent];

	[_n_clear_a_scrollbar mouseDragged:theEvent];
	[_n_clear_r_scrollbar mouseDragged:theEvent];
	[_n_clear_g_scrollbar mouseDragged:theEvent];
	[_n_clear_b_scrollbar mouseDragged:theEvent];

	[_n_replacer_fr_a_scrollbar mouseDragged:theEvent];
	[_n_replacer_fr_r_scrollbar mouseDragged:theEvent];
	[_n_replacer_fr_g_scrollbar mouseDragged:theEvent];
	[_n_replacer_fr_b_scrollbar mouseDragged:theEvent];
	[_n_replacer_to_a_scrollbar mouseDragged:theEvent];
	[_n_replacer_to_r_scrollbar mouseDragged:theEvent];
	[_n_replacer_to_g_scrollbar mouseDragged:theEvent];
	[_n_replacer_to_b_scrollbar mouseDragged:theEvent];

}

- (void) n_toolwindow_rightMouseUp:(NSEvent*) theEvent
{

	if ( FALSE == n_mac_window_is_keywindow( _n_toolwindow ) ) { return; }

	if ( [_n_button_4_1 n_is_pressed_right] )
	{
//return;
		if ( n_posix_false == [_n_button_4_1 n_is_enabled] ) { return; }


		n_posix_char *path = n_mac_nsstring2str( paint.filename );
//NSLog( @"filename : %s", path );

		if ( n_string_path_ext_is_same_literal( ".BMP\0\0", path ) )
		{
			n_string_path_ext_mod_literal( ".ico\0\0", path );
		} else
		if ( n_string_path_ext_is_same_literal( ".ICO\0\0", path ) )
		{
			n_string_path_ext_mod_literal( ".cur\0\0", path );
		} else
		if ( n_string_path_ext_is_same_literal( ".CUR\0\0", path ) )
		{
			n_string_path_ext_mod_literal( ".jpg\0\0", path );
		} else
		if ( n_string_path_ext_is_same_literal( ".JPG\0\0", path ) )
		{
			n_string_path_ext_mod_literal( ".png\0\0", path );
		} else
		if ( n_string_path_ext_is_same_literal( ".PNG\0\0", path ) )
		{
			n_string_path_ext_mod_literal( ".lyr\0\0", path );
		} else
		if ( n_string_path_ext_is_same_literal( ".LYR\0\0", path ) )
		{
			n_string_path_ext_mod_literal( ".bmp\0\0", path );
		}
//NSLog( @"filename after : %s", path );

		paint.filename = n_mac_str2nsstring( path );
		n_string_path_free( path );

		[self NonnonPaintIconDisplay:_n_button_4_1];
		[self NonnonPaintTitle];

	}

}

- (void) n_layer_window_rightMouseUp:(NSEvent*) theEvent
{

	if ( FALSE == n_mac_window_is_keywindow( _n_layer_window ) ) { return; }

	if ( paint.layer_txt.readonly ) { return; }

	if ( n_mac_window_is_hovered( _n_layer_listbox ) )
	{
		if ( paint.layer_whole_grab_onoff )
		{
			if ( paint.grabber_mode != N_PAINT_GRABBER_NEUTRAL ) { return; }
		}

		[_n_layer_listbox NonnonTxtboxClickEvent:theEvent detailed:YES];

		[self NonnonPaintLayerSelect:_n_layer_listbox.n_focus];
//NSLog( @"%lld : %lld", _n_layer_listbox.n_focus, paint.layer_index );

		if ( paint.layer_index == 0 )
		{
			[_n_layer_popup_menu_up setEnabled:FALSE];
		} else {
			[_n_layer_popup_menu_up setEnabled:TRUE];
		}

		if ( paint.layer_index == ( paint.layer_count - 1 ) )
		{
			[_n_layer_popup_menu_down setEnabled:FALSE];
		} else {
			[_n_layer_popup_menu_down setEnabled:TRUE];
		}

		[NSMenu popUpContextMenu:_n_layer_popup_menu withEvent:theEvent forView:_n_layer_listbox];
	}

}

- (void) rightMouseUp:(NSEvent*) theEvent
{
//NSLog(@"rightMouseUp");

	[self n_toolwindow_rightMouseUp:theEvent];
	[self n_layer_window_rightMouseUp:theEvent];

}

- (void)colorUpdate:(NSColorPanel*)colorPanel{
 
	NSColor *nscolor = colorPanel.color;

	paint.color = n_bmp_color_mac( n_mac_nscolor2argb( nscolor ) );

	[self NonnonPaintColorSet];

}

- (IBAction)n_ppa_method:(NSButton *)sender {

	if ( sender.state == NSControlStateValueOff )
	{
		paint.grabber_per_pixel_alpha_onoff = n_posix_false;
	} else
	if ( sender.state == NSControlStateValueOn )
	{
		paint.grabber_per_pixel_alpha_onoff = n_posix_true;
	}

	[_n_paint_canvas display_optimized];

}

- (void) otherMouseUp:(NSEvent *)theEvent
{
//NSLog(@"otherMouseUp");

	[_n_paint_canvas  otherMouseUp:theEvent];
	[_n_layer_listbox otherMouseUp:theEvent];

}

- (void) otherMouseDragged:(NSEvent *)theEvent
{
//NSLog(@"otherMouseDragged");

	if ( n_mac_window_is_keywindow( _n_paint_canvas.window ) )
	{
		[_n_paint_canvas otherMouseDragged:theEvent];
	}

	if ( n_mac_window_is_keywindow( _n_layer_window ) )
	{
		[_n_layer_listbox otherMouseDragged:theEvent];
	}

}




// Resizer

- (void) n_resizer_go_resize:(n_bmp*) resizer_bmp
{

	n_type_gfx sx = (n_type_gfx) [_n_resizer_sx integerValue];
	n_type_gfx sy = (n_type_gfx) [_n_resizer_sy integerValue];

	int index = (int) [_n_resizer_combo indexOfSelectedItem];

	if ( index == 0 )
	{
		n_bmp_resizer( resizer_bmp, sx,sy, n_bmp_white_invisible, N_BMP_RESIZER_NORMAL );
	} else
	if ( index == 1 )
	{
		n_bmp_resizer( resizer_bmp, sx,sy, n_bmp_white_invisible, N_BMP_RESIZER_TILE );
	} else
	if ( index == 2 )
	{
		n_bmp_resizer( resizer_bmp, sx,sy, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );
	} else
	if ( index == 3 )
	{
		n_bmp_resampler
		(
			resizer_bmp,
			(n_type_real) sx / N_BMP_SX( resizer_bmp ),
			(n_type_real) sy / N_BMP_SY( resizer_bmp )
		);
	} else
	if ( index == 4 )
	{
		n_type_real ratio = (n_type_real) sx / N_BMP_SX( resizer_bmp );
		n_bmp_resampler( resizer_bmp, ratio, ratio );
	} else
	if ( index == 5 )
	{
		n_type_real ratio = (n_type_real) sy / N_BMP_SY( resizer_bmp );
		n_bmp_resampler( resizer_bmp, ratio, ratio );
	} else
	if ( index == 6 )
	{
		n_bmp_scaler_big_pixelart( resizer_bmp, 2 );
	} else
	if ( index == 7 )
	{
		n_bmp_scaler_big_pixelart( resizer_bmp, 3 );
	} else
	if ( index == 8 )
	{
		n_bmp_scaler_big( resizer_bmp, 2 );
		n_bmp_scaler_big( resizer_bmp, 2 );

		n_bmp_flush_antialias( resizer_bmp, 1.0 );
		n_bmp_flush_antialias( resizer_bmp, 1.0 );
		n_bmp_flush_antialias( resizer_bmp, 1.0 );
		n_bmp_flush_antialias( resizer_bmp, 1.0 );

		n_bmp_scaler_lil( resizer_bmp, 2 );

		n_bmp_flush_sharpen( resizer_bmp, 0.5 );
	}// else

	n_bmp_matrix_rotate( resizer_bmp, resizer_rotate, n_bmp_white_invisible, n_posix_true );


	return;
}

- (void) n_resizer_go_color:(n_bmp*) resizer_bmp
{
//n_mac_debug_count(); return;

	int index = (int) [_n_resizer_color_combobox indexOfSelectedItem];
	if ( index == 0 )
	{
		// [!] : "None"
	} else
	if ( index == 1 )
	{
		n_bmp_flush_grayscale( resizer_bmp );
	} else
	if ( index == 2 )
	{
		n_bmp_flush_monochrome( resizer_bmp );
	} else
	if ( index == 3 )
	{
		n_paint_bmp_flush_reverse( resizer_bmp );
	} else
	if ( index == 4 )
	{
		n_bmp_flush_reducer( resizer_bmp, 4 );
	} else
	if ( index == 5 )
	{
		n_bmp_flush_balance( resizer_bmp );
	} else
	if ( index == 6 )
	{
		n_paint_bmp_icon_squircle( resizer_bmp );
	} else
	if ( index == 7 )
	{
		n_paint_bmp_contour( resizer_bmp );
	}// else


	{ // Gamma

		// [!] : shortcut for performance

		n_type_real g = (n_type_real) resizer_gamma / 10.0;

		if ( g == 0.0 ) { n_bmp_flush( resizer_bmp, n_bmp_black ); } else
		if ( g == 2.0 ) { n_bmp_flush( resizer_bmp, n_bmp_white ); } else
		if ( g != 1.0 ) { n_bmp_flush_gamma( resizer_bmp, g );     }

	}

	//if ( 0 )
	{ // Color Wheel & Vividness

		int clrwh = (int) [_n_resizer_colorwheel_scrollbar n_scrollbar_position_get];
		int vivid = (int) [_n_resizer_vividness_scrollbar  n_scrollbar_position_get];

		int h = clrwh - 128;
		int s = vivid - 100;
		int l = 0;

//NSLog( @"%d %d : %d %d : %f", clrwh, vivid, h, s, _n_resizer_colorwheel_scrollbar.scr.unit_pos );

		n_mac_bmp_flush_tweaker_hsl( resizer_bmp, h, s, l );

	}

	//if ( 0 )
	{ // Sharpness

		int s = (int) [_n_resizer_sharpness_scrollbar n_scrollbar_position_get];

		s = s - 100;

		if ( s > 0 )
		{
			n_bmp_flush_sharpen  ( resizer_bmp, (n_type_real) s * 0.01 );
		} else
		if ( s < 0 )
		{
			s *= -1;
			n_bmp_flush_antialias( resizer_bmp, (n_type_real) s * 0.01 );
		}

	}

	//if ( 0 )
	{ // Contrast

		int ctrst = (int) [_n_resizer_contrast_scrollbar n_scrollbar_position_get];

		n_bmp_flush_contrast( resizer_bmp, ctrst );

	}

}

- (void) NonnonPaintResizerInit
{
//return;
//n_mac_debug_count(); return;

	if ( paint.resizer_onoff != FALSE ) { return; }

	if ( paint.layer_onoff )
	{
		paint.resizer_layer_data = n_bmp_layer_raw_copy( paint.layer_data, paint.layer_count );
//n_mac_image_debug_save( &paint.resizer_layer_data[ paint.layer_index ].bmp_data );
	} else {
		n_paint_bmp_carboncopy( &paint.bmp_data, &paint.resizer_bmp_data );
		n_paint_bmp_carboncopy( &paint.bmp_grab, &paint.resizer_bmp_grab );
//n_mac_image_debug_save( &paint.bmp_data );
//n_mac_image_debug_save( &paint.resizer_bmp_data );
	}

//NSLog( @"!" );

	// [Needed] : validateMenuItem is needed

	[self NonnonPaintUIContext:_n_resizer_window onoff:FALSE];


	return;
}

- (void) NonnonPaintResizerExit:(BOOL)replace
{
//return;
//n_mac_debug_count(); return;

	if ( paint.resizer_onoff == FALSE ) { return; }

	if ( replace )
	{
//n_mac_debug_count();

		if ( paint.layer_onoff )
		{
			n_bmp_layer_raw_free( paint.resizer_layer_data, paint.layer_count );
			paint.resizer_layer_data = NULL;
		} else {
			n_bmp_free( &paint.resizer_bmp_data );
			n_bmp_free( &paint.resizer_bmp_grab );
		}

	} else {

		if ( paint.layer_onoff )
		{
			n_bmp_layer_raw_free( paint.layer_data, paint.layer_count );
			paint.layer_data = paint.resizer_layer_data;
//n_mac_image_debug_save( &paint.layer_data[ paint.layer_index ].bmp_data );
		} else {
			n_paint_bmp_carboncopy( &paint.resizer_bmp_data, &paint.bmp_data );
			n_paint_bmp_carboncopy( &paint.resizer_bmp_grab, &paint.bmp_grab );
		}

	}

	if ( paint.layer_onoff )
	{
		n_type_int y = paint.layer_index;
		paint.pen_bmp_data = &paint.layer_data[ y ].bmp_data;
		paint.pen_bmp_grab = &paint.layer_data[ y ].bmp_grab;
	}


	// [Needed] : validateMenuItem is needed

	[self NonnonPaintUIContext:_n_resizer_window onoff:TRUE];


	return;
}

- (void) n_resizer_go
{
//return;
//n_mac_debug_count(); return;

	if ( paint.resizer_onoff == FALSE ) { return; }


	[[NSCursor operationNotAllowedCursor] set];

	if ( paint.layer_onoff )
	{

		if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
//NSLog( @"1" );
			n_bmp_layer_raw_free( paint.layer_data, paint.layer_count );
			paint.layer_data = n_bmp_layer_raw_copy( paint.resizer_layer_data, paint.layer_count );

			n_type_int y = paint.layer_index;

			if ( paint.layer_whole_grab_onoff )
			{
				n_type_int i = 0;
				n_posix_loop
				{//break;

					[self n_resizer_go_resize:&paint.layer_data[ i ].bmp_data];

					if ( n_posix_false == n_paint_layer_is_locked( i ) )
					{
						[self n_resizer_go_color :&paint.layer_data[ i ].bmp_data];
					}

					i++;
					if ( i >= paint.layer_count ) { break; }
				}
			} else {
				n_type_int i = 0;
				n_posix_loop
				{//break;

					[self n_resizer_go_resize:&paint.layer_data[ i ].bmp_data];

					i++;
					if ( i >= paint.layer_count ) { break; }
				}

				[self n_resizer_go_color :&paint.layer_data[ y ].bmp_data];
			}

			paint.pen_bmp_data = &paint.layer_data[ y ].bmp_data;

			[_n_paint_canvas n_paint_canvas_reset_cache];
		} else {
//NSLog( @"2" );

			n_type_int i = 0;
			n_posix_loop
			{//break;

				n_bmp_free( &paint.layer_data[ i ].bmp_grab );
				n_bmp_carboncopy( &paint.resizer_layer_data[ i ].bmp_grab, &paint.layer_data[ i ].bmp_grab );

				[self n_resizer_go_resize:&paint.layer_data[ i ].bmp_grab];
				[self n_resizer_go_color :&paint.layer_data[ i ].bmp_grab];

				i++;
				if ( i >= paint.layer_count ) { break; }
			}

			n_type_int y = paint.layer_index;
			paint.pen_bmp_grab = &paint.layer_data[ y ].bmp_grab;

			[_n_paint_canvas n_paint_canvas_reset_cache];

			n_type_gfx gx,gy,gsx,gsy,gfx,gfy;
			n_paint_grabber_system_get( &gx,&gy, &gsx,&gsy, &gfx,&gfy );

			gsx = N_BMP_SX( paint.pen_bmp_grab );
			gsy = N_BMP_SY( paint.pen_bmp_grab );

			n_paint_grabber_system_set( &gx,&gy, &gsx,&gsy, &gfx,&gfy );
		}

//n_mac_image_debug_save( &paint.layer_data[ paint.layer_index ].bmp_data );
	} else {

		n_bmp *bmp_target;

		if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			n_paint_bmp_carboncopy( &paint.resizer_bmp_data, &paint.bmp_data );
//n_mac_image_debug_save( &paint.resizer_bmp_data );
			bmp_target = &paint.bmp_data;
		} else {
			n_paint_bmp_carboncopy( &paint.resizer_bmp_grab, &paint.bmp_grab );
			bmp_target = &paint.bmp_grab;
		}

		[self n_resizer_go_resize:bmp_target];
		[self n_resizer_go_color :bmp_target];

//NSLog( @"%d", n_paint_global.paint->grabber_mode );
		if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
		{
			//
		} else {
			n_type_gfx gx,gy,gsx,gsy,gfx,gfy;
			n_paint_grabber_system_get( &gx,&gy, &gsx,&gsy, &gfx,&gfy );

			gsx = N_BMP_SX( bmp_target );
			gsy = N_BMP_SY( bmp_target );

			n_paint_grabber_system_set( &gx,&gy, &gsx,&gsy, &gfx,&gfy );
		}

	}

	[[NSCursor arrowCursor] set];


	paint.grabber_frame_lock = TRUE;


	[_n_paint_canvas display_optimized];

}

- (IBAction)n_resizer_resize_combobox_method:(id)sender {

	int index = (int) [_n_resizer_combo indexOfSelectedItem];

	if ( index == 4 )
	{
		[_n_resizer_sy setEnabled:FALSE];
	} else {
		[_n_resizer_sy setEnabled:TRUE];
	}

	if ( index == 5 )
	{
		[_n_resizer_sx setEnabled:FALSE];
	} else {
		[_n_resizer_sx setEnabled:TRUE];
	}

	[self n_resizer_go];

}

- (IBAction)n_resizer_sx_method:(id)sender {

//NSLog( @"%ld", [sender integerValue] );

	if ( N_PAINT_LIMIT_SIZE < [sender integerValue] )
	{
		[_n_resizer_sx setStringValue:[NSString stringWithFormat:@"%d", N_PAINT_LIMIT_SIZE]];
	}

	[self n_resizer_go];

}

- (IBAction)n_resizer_sy_method:(id)sender {

//NSLog( @"%ld", [sender integerValue] );

	if ( N_PAINT_LIMIT_SIZE < [sender integerValue] )
	{
		[_n_resizer_sy setStringValue:[NSString stringWithFormat:@"%d", N_PAINT_LIMIT_SIZE]];
	}

	[self n_resizer_go];

}

- (IBAction)n_resizer_color_combobox_method:(NSComboBox *)sender {
//NSLog( @"n_resizer_color_combobox_method" );

	[self n_resizer_go];

}

- (IBAction)n_resizer_button_go_method:(NSButton *)sender {

	resizer_is_ok = TRUE;
	[_n_resizer_window close];

}




// [!] : Layer

- (IBAction)n_layer_whole_preview_method:(NSButton *)sender {

	if ( sender.state == NSControlStateValueOff )
	{
		paint.layer_whole_preview_onoff = n_posix_false;
	} else
	if ( sender.state == NSControlStateValueOn )
	{
		paint.layer_whole_preview_onoff = n_posix_true;
	}

	[_n_paint_canvas display_optimized];

}

- (IBAction)n_layer_whole_grab_method:(NSButton *)sender {

	if ( sender.state == NSControlStateValueOff )
	{
		paint.layer_whole_grab_onoff = n_posix_false;
	} else
	if ( sender.state == NSControlStateValueOn )
	{
		paint.layer_whole_grab_onoff = n_posix_true;
	}

}

- (IBAction)n_layer_whole_save_method:(NSButton *)sender {

	if ( sender.state == NSControlStateValueOff )
	{
		paint.layer_whole_save_onoff = n_posix_false;
	} else
	if ( sender.state == NSControlStateValueOn )
	{
		paint.layer_whole_save_onoff = n_posix_true;
	}

}



// [!] : Layer Menu

- (IBAction)n_layer_menu_up:(id)sender {

	if ( n_paint_layer_swap_up( paint.layer_index ) )
	{
		_n_layer_listbox.n_focus = paint.layer_index;
	}

	[_n_layer_listbox NonnonTxtboxFocus2Caret];
	[_n_layer_listbox NonnonTxtboxCaretOutOfCanvasUpDown];

	[_n_layer_listbox display];

}

- (IBAction)n_layer_menu_down:(id)sender {

	if ( n_paint_layer_swap_down( paint.layer_index ) )
	{
		_n_layer_listbox.n_focus = paint.layer_index;
	}

	[_n_layer_listbox NonnonTxtboxFocus2Caret];
	[_n_layer_listbox NonnonTxtboxCaretOutOfCanvasUpDown];

	[_n_layer_listbox display];

}

- (IBAction)n_layer_menu_rename:(id)sender {

	[self NonnonTxtbox_delegate_listbox_rename:paint.layer_index];

	[self NonnonPaintLayerMenuContext];
/*
	if ( _n_layer_listbox.n_listbox_edit_onoff )
	{
		_n_layer_listbox.n_listbox_edit_onoff = FALSE;
	} else {
		_n_layer_listbox.n_listbox_edit_onoff = TRUE;
	}

	[_n_layer_listbox display];
*/
}

- (IBAction)n_layer_menu_all_off:(id)sender {

	n_paint_layer_visibility_onoff_all( n_posix_false );

}

- (IBAction)n_layer_menu_all_on:(id)sender {

	n_paint_layer_visibility_onoff_all( n_posix_true  );

}

- (IBAction)n_layer_menu_this_only:(id)sender {

	n_paint *p = &paint;

	n_paint_layer_visibility_onoff_all( n_posix_false );

	n_type_int i = p->layer_index;

	p->layer_data[ i ].visible = n_posix_true;

	n_posix_char *nam = n_txt_get( &p->layer_txt, i );
	n_paint_layer_text_mod( p, i, nam );

	[_n_layer_listbox display];

	[_n_paint_canvas display_optimized];

}

- (IBAction)n_layer_menu_thicken_lines:(id)sender {

	n_type_int y = paint.layer_index;

	if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		n_paint_bmp_thicken( &paint.layer_data[ y ].bmp_data, paint.color );
	} else {
		n_paint_bmp_thicken( &paint.layer_data[ y ].bmp_grab, paint.color );
	}

	[_n_paint_canvas display_optimized];

}

- (IBAction)n_layer_menu_thin_lines:(id)sender {

	n_type_int y = paint.layer_index;

	if ( paint.grabber_mode == N_PAINT_GRABBER_NEUTRAL )
	{
		n_paint_bmp_thin( &paint.layer_data[ y ].bmp_data, paint.color );
	} else {
		n_paint_bmp_thin( &paint.layer_data[ y ].bmp_grab, paint.color );
	}

	[_n_paint_canvas display_optimized];

}

- (IBAction)n_layer_menu_add_a_new_layer:(id)sender {

	n_paint *p = &paint;


	n_type_int i = p->layer_count;

	p->layer_data = n_memory_resize( p->layer_data, sizeof( n_paint_layer ) * ( i + 1 ) );
	n_paint_layer_zero( &p->layer_data[ i ] );

	n_type_gfx sx = N_BMP_SX( &p->layer_data[ 0 ].bmp_data );
	n_type_gfx sy = N_BMP_SY( &p->layer_data[ 0 ].bmp_data );

	n_bmp_new( &p->layer_data[ i ].bmp_data, sx,sy );

	n_bmp_flush( &p->layer_data[ i ].bmp_data, n_bmp_white_invisible );

	n_txt_add( &p->layer_txt,            i, n_posix_literal( "New Layer" ) );
	n_txt_add( &p->layer_decoration_txt, i, n_posix_literal( "B" ) );


	n_paint_layer_config_default_single( p, i );


	n_posix_char *nam = n_txt_get( &p->layer_txt, i );

	p->layer_data[ i ].visible = n_posix_true;
	n_paint_layer_text_mod( p, i, nam );


	p->layer_count++;


	// [!] : UI
//NSLog( @"!" );

	[self NonnonPaintLayerSelect:p->layer_count - 1];

	_n_layer_listbox.n_focus = p->layer_index;

	[_n_layer_listbox NonnonTxtboxFocus2Caret];
	[_n_layer_listbox NonnonTxtboxCaretOutOfCanvasUpDown];

	[_n_layer_listbox display];


	[self NonnonPaintLayerRefresh];


	[_n_paint_canvas display_optimized];

}



@end
