// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#define N_PAINT_CANVAS_COLOR n_bmp_rgb_mac( 128,128,128 )


#define N_PAINT_TOOL_TYPE_PEN     0
#define N_PAINT_TOOL_TYPE_FILL    1
#define N_PAINT_TOOL_TYPE_GRABBER 2


#define N_PAINT_GRABBER_NEUTRAL              0
#define N_PAINT_GRABBER_SELECTING            1
#define N_PAINT_GRABBER_DRAG_OK              2
#define N_PAINT_GRABBER_DRAGGING             3
#define N_PAINT_GRABBER_STRETCH_PROPORTIONAL 4
#define N_PAINT_GRABBER_STRETCH_TRANSFORM    5
//#define N_PAINT_GRABBER_RESELECT             6


#define N_PAINT_GRABBER_INPUT_U ( 1 << 0 )
#define N_PAINT_GRABBER_INPUT_D ( 1 << 1 )
#define N_PAINT_GRABBER_INPUT_L ( 1 << 2 )
#define N_PAINT_GRABBER_INPUT_R ( 1 << 3 )


#define N_PAINT_FILTER_SCALE_LIL 0
#define N_PAINT_FILTER_SCALE_BIG 1
#define N_PAINT_FILTER_MIRROR    2
#define N_PAINT_FILTER_ROTATE_L  3
#define N_PAINT_FILTER_ROTATE_R  4
#define N_PAINT_FILTER_ALPHA_CLR 5
#define N_PAINT_FILTER_ALPHA_REV 6

#define N_PAINT_REDRAW_TYPE_ALL  0
#define N_PAINT_REDRAW_TYPE_PEN  1


#define N_PAINT_GRABBER_FRAME_MODE_EDGE  ( 0 )
#define N_PAINT_GRABBER_FRAME_MODE_PIXEL ( 1 )



#define N_PAINT_LAYER_MAX ( 16 )
#define N_PAINT_LAYER_CCH ( 32 )

typedef struct {

	n_posix_char name[ N_PAINT_LAYER_CCH ];
	n_bmp        bmp_data;
	n_bmp        bmp_grab;
	BOOL         visible;
	int          percent;
	int          blur;

	// Cache

	n_type_real  blend;

	n_bmp        bmp_blur;

} n_paint_layer;

#define n_paint_layer_zero( p ) n_memory_zero( p, sizeof( n_paint_layer ) )




typedef struct {

	n_type_gfx x,y,sx,sy;

} n_rect;


typedef struct {

	// [!] : Base

	BOOL       init;

	NSString  *filename;
	n_curico   curico;
	n_bmp      bmp_data;
	n_bmp      bmp_grab;
	n_type_gfx zoom;
	NSPoint    scroll;
	CGFloat    inner_sx;
	CGFloat    inner_sy;
	NSRect     rect_redraw;
	int        redraw_type;

	BOOL       margin_onoff;
	CGFloat    margin;

	NSWindow  *nswindow[ 25 ];
	BOOL        restore[ 25 ];

	NSCursor  *cursor_arrow;
	NSCursor  *cursor_no;
	NSCursor  *cursor_hand;
	NSCursor  *cursor_pen_off;
	NSCursor  *cursor_pen_on;
	NSCursor  *cursor_eraser;
	NSCursor  *cursor_blur;
	NSCursor  *cursor_resize_dragging;
	NSCursor  *cursor_resize_nw;
	NSCursor  *cursor_resize_ne;
	BOOL       cursor_grab_n_drag_onoff;

	NSOperationQueue *queue;
	int               cores;

	NSButton *whole_preview_checkbox;
	NSButton *whole_grab_checkbox;
	NSButton *whole_save_checkbox;

	BOOL      is_about_window;

	BOOL      xmouse_stop;


	// [!] : Instance

	u32        color;
	int        tooltype;
	int        pensize;
	int        mix;
	int        boost;
	int        air;

	BOOL       grid_onoff;
	BOOL       pixel_grid_onoff;
	BOOL       alpha_emphasizer_onoff;


	// [!] : Canvas

	BOOL       readonly;

	NSPoint    pt_prv;

	CGFloat    canvas_offset_x;
	CGFloat    canvas_offset_y;
	CGFloat    canvas_offset_sx;
	CGFloat    canvas_offset_sy;

	CGFloat    scroller_size;
	n_bmp_fade scroller_fade;
	BOOL       scroller_thumb_is_captured;

	BOOL       scroller_x_onoff;
	BOOL       scroller_y_onoff;
	NSRect     scroller_x_rect_shaft;
	NSRect     scroller_y_rect_shaft;
	NSRect     scroller_x_rect_thumb;
	NSRect     scroller_y_rect_thumb;
	BOOL       scroller_x_thumb_is_hovered;
	BOOL       scroller_x_shaft_is_hovered;
	BOOL       scroller_y_thumb_is_hovered;
	BOOL       scroller_y_shaft_is_hovered;
	CGFloat    scroller_x_offset;
	CGFloat    scroller_y_offset;


	// [!] : Grabber
	
	BOOL          grabber_per_pixel_alpha_onoff;
	int           grabber_blend;
	double        grabber_blend_ratio;

	BOOL          grabber_delay_onoff;
	u32           grabber_delay_timer;

	int           grabber_mode;
	n_rect        grabber_rect;
	n_type_gfx    grabber_fx;
	n_type_gfx    grabber_fy;

	n_type_gfx    grabber_drag_ox;
	n_type_gfx    grabber_drag_oy;
	n_type_gfx    grabber_drag_fx;
	n_type_gfx    grabber_drag_fy;

	n_bmp        *grabber_stretch_bmp;
	n_type_real   grabber_stretch_ratio;
	n_type_gfx    grabber_stretch_cx;
	n_type_gfx    grabber_stretch_cy;
	n_type_gfx    grabber_stretch_px;
	n_type_gfx    grabber_stretch_py;
	n_type_gfx    grabber_stretch_sx;
	n_type_gfx    grabber_stretch_sy;
	n_type_gfx    grabber_stretch_ox_l;
	n_type_gfx    grabber_stretch_ox_r;
	n_type_gfx    grabber_stretch_oy_u;
	n_type_gfx    grabber_stretch_oy_d;
	n_type_gfx    grabber_stretch_start_x;
	n_type_gfx    grabber_stretch_start_y;

	n_posix_bool  grabber_is_resel;

	n_rect        grabber_rect_resizer;

	int           grabber_is_key_input;
	BOOL          grabber_is_key_input_fade;
	u32           grabber_is_key_input_tick;

	n_type_gfx    grabber_cache_redraw_x;
	n_type_gfx    grabber_cache_redraw_y;
	n_type_gfx    grabber_cache_redraw_sx;
	n_type_gfx    grabber_cache_redraw_sy;

	NSRect        grabber_tooltip_rect_drawn;
	NSRect        grabber_resync_rect;

	n_type_gfx    grabber_delayed_x;
	n_type_gfx    grabber_delayed_y;

	int           grabber_frame_mode;
	BOOL          grabber_frame_lock;
	BOOL          grabber_frame_hide;
	NSRect        grabber_frame_rect[ 4 ];
	n_type_int    grabber_frame_step;
	NSPoint      *grabber_frame_path;
	n_type_int    grabber_frame_path_count;


	// [!] : Pen

	n_bmp        *pen_bmp_data;
	n_bmp        *pen_bmp_grab;
	BOOL          pen_start;
	n_type_gfx    pen_start_x;
	n_type_gfx    pen_start_y;
	n_type_gfx    pen_radius;
	u32           pen_color;
	double        pen_blend;
	int           pen_boost;
	BOOL          pen_quick_eraser_onoff;
	BOOL          pen_quick_blur_onoff;
	BOOL          pen_quick_eraser_whole_layer_onoff;


	// [!] : Resizer

	BOOL           resizer_onoff;
	n_bmp          resizer_bmp_data;
	n_bmp          resizer_bmp_grab;
	n_paint_layer *resizer_layer_data;


	// [!] : Layer

	BOOL           layer_onoff;
	int            layer_count;
	n_type_int     layer_index;
	n_paint_layer *layer_data;
	n_ini          layer_ini;
	BOOL           layer_load_as_layer;

	n_txt          layer_txt;
	n_txt          layer_decoration_txt;

	BOOL           layer_whole_preview_onoff;
	BOOL           layer_whole_grab_onoff;
	BOOL           layer_whole_save_onoff;

	BOOL           layer_load_onoff;
	n_type_real    layer_load_percent;

	n_bmp          layer_cache_bmp_data;
	n_bmp          layer_cache_bmp_pre;
	n_bmp          layer_cache_bmp_pst;

	int            layer_save_count;


	// [!] : Clear Canvas

	u32            clear_color;


	// [!] : Color Replacer

	u32            replacer_fr_color;
	u32            replacer_to_color;

} n_paint;


#define n_paint_zero( p ) n_memory_zero( p, sizeof( n_paint ) )


