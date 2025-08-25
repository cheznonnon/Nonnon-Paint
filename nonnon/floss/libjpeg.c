// Nonnon FLOSS
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_FLOSS_LIBJPEG
#define _H_NONNON_FLOSS_LIBJPEG




#ifdef __APPLE__

#pragma clang diagnostic ignored "-Wcomma"

#endif // #ifdef __APPLE__




#define JPEG_INTERNALS

//#include "libjpeg/jconfig.h"
//#include "libjpeg/jmorecfg.h"
#include "libjpeg/jpeglib.h"

//#include "libjpeg/jerror.h"
//#include "libjpeg/jinclude.h"
//#include "libjpeg/jversion.h"
#include "libjpeg/jerror.c"

#include "libjpeg/jpegint.h"	// include guard is needed
#include "libjpeg/jcomapi.c"

#include "libjpeg/jcparam.c"

#include "libjpeg/jcapimin.c"
#include "libjpeg/jdapimin.c"

#include "libjpeg/jdatadst.c"
#include "libjpeg/jdatasrc.c"

#include "libjpeg/jcapistd.c"
#include "libjpeg/jdapistd.c"

#include "libjpeg/jcmarker.c"	// my_marker_ptr => jcmarker_my_marker_ptr
				// emit_byte     => jcmarker_emit_byte
#include "libjpeg/jdmarker.c"	// my_marker_ptr => jdmarker_my_marker_ptr, JPEG_MARKER => removed

#include "libjpeg/jutils.c"

#include "libjpeg/jmemansi.c"
//#include "libjpeg/jmemsys.h"	// include guard is needed
#include "libjpeg/jmemmgr.c"

#include "libjpeg/jcinit.c"

#include "libjpeg/jcmaster.c"	// my_master_ptr  => jcmaster_my_master_ptr
				// initial_setup  => jcmaster_initial_setup
				// per_scan_setup => jcmaster_per_scan_setup
#include "libjpeg/jdmaster.c"	// my_master_ptr => jdmaster_my_master_ptr, RGB_PIXELSIZE is needed

#include "libjpeg/jdinput.c"	// initial_setup    => jdinput_initial_setup
				// per_scan_setup   => jdinput_per_scan_setup
				// start_input_pass => jdinput_start_input_pass

#include "libjpeg/jccolor.c"	// RGB_* macro are needed
				// my_cconvert_ptr   => jccolor_my_cconvert_ptr
				// rgb_gray_convert  => jccolor_rgb_gray_convert
				// grayscale_convert => jccolor_grayscale_convert
				// rgb_convert       => jccolor_rgb_convert
				// null_convert      => jccolor_null_convert
				// TABLE_SIZE        => jccolor_TABLE_SIZE
				// FIX               => jccolor_FIX
#include "libjpeg/jdcolor.c"	// my_cconvert_ptr     => jdcolor_my_cconvert_ptr
				// rgb_gray_convert    => jdcolor_rgb_gray_convert
				// grayscale_convert   => jdcolor_grayscale_convert
				// rgb_convert         => jdcolor_rgb_convert
				// null_convert        => jdcolor_null_convert
				// TABLE_SIZE          => jdcolor_TABLE_SIZE
				// FIX                 => jdcolor_FIX
				// build_ycc_rgb_table => jdcolor_build_ycc_rgb_table

#include "libjpeg/jcsample.c"
#include "libjpeg/jdsample.c"	// my_upsampler    => jdsample_my_upsampler
				// my_upsample_ptr => jdsample_my_upsample_ptr

#include "libjpeg/jcprepct.c"
#include "libjpeg/jdpostct.c"

#include "libjpeg/jccoefct.c"	// my_coef_controller => jccoefct_my_coef_controller
				// my_coef_ptr        => jccoefct_my_coef_ptr
				// start_iMCU_row     => jccoefct_start_iMCU_row
				// start_pass_coef    => jccoefct_start_pass_coef
				// start_iMCU_row     => jccoefct_start_iMCU_row
				// compress_output    => jccoefct_compress_output
#include "libjpeg/jdcoefct.c"	// my_coef_controller => jdcoefct_my_coef_controller
				// my_coef_ptr        => jdcoefct_my_coef_ptr
				// start_iMCU_row     => jdcoefct_start_iMCU_row
				// start_input_pass   => jdcoefct_start_input_pass

#include "libjpeg/jcmainct.c"	// my_main_controller       => jcmainct_my_main_controller
				// my_main_ptr              => jcmainct_my_main_ptr
				// process_data_simple_main => jcmainct_process_data_simple_main
				// main                     => jcmainct_main
				// start_pass_main          => jcmainct_start_pass_main
#include "libjpeg/jdmainct.c"	// my_main_controller       => jdmainct_my_main_controller
				// my_main_ptr              => jdmainct_my_main_ptr
				// process_data_simple_main => jdmainct_process_data_simple_main
				// main                     => jdmainct_main
				// start_pass_main          => jdmainct_start_pass_main

#include "libjpeg/jaricom.c"
#include "libjpeg/jcarith.c"	// emit_byte            => jcarith_emit_byte
				// arith_entropy_ptr    => jcarith_arith_entropy_ptr
				// start_pass           => jcarith_start_pass
				// encode_mcu_DC_first  => jcarith_encode_mcu_DC_first
				// encode_mcu_AC_first  => jcarith_encode_mcu_AC_first
				// encode_mcu_DC_refine => jcarith_encode_mcu_DC_refine
				// encode_mcu_AC_refine => jcarith_encode_mcu_AC_refine
#include "libjpeg/jdarith.c"	// arith_entropy_ptr    => jdarith_arith_entropy_ptr
				// start_pass           => jdarith_start_pass
				// process_restart      => jdarith_process_restart
				// decode_mcu_DC_first  => jdarith_decode_mcu_DC_first
				// decode_mcu_AC_first  => jdarith_decode_mcu_AC_first
				// decode_mcu_DC_refine => jdarith_decode_mcu_DC_refine
				// decode_mcu_AC_refine => jdarith_decode_mcu_AC_refine
//#include "libjpeg/jdct.h"	// include guard is needed
				// FIX => jdct_FIX
#include "libjpeg/jcdctmgr.c"	// CONST_BITS => jcdctmgr_CONST_BITS
#include "libjpeg/jddctmgr.c"	// CONST_BITS => jddctmgr_CONST_BITS
#include "libjpeg/jfdctflt.c"
#include "libjpeg/jfdctfst.c"	// CONST_BITS => jfdctfst_CONST_BITS
				// FIX*       => jfdctfst_FIX
				// MULTIPLY   => jfdctfst_MULTIPLY
#include "libjpeg/jfdctint.c"	// MULTIPLY   => jfdctint_MULTIPLY
				// FIX*       => jidctint_FIX*

#include "libjpeg/jchuff.c"	// huff_entropy_ptr     => jchuff_huff_entropy_ptr
				// encode_mcu_DC_first  => jchuff_encode_mcu_DC_first
				// encode_mcu_AC_first  => jchuff_encode_mcu_AC_first
				// encode_mcu_DC_refine => jchuff_encode_mcu_DC_refine
				// encode_mcu_AC_refine => jchuff_encode_mcu_AC_refine
				// encode_mcu_huff      => jchuff_encode_mcu_huff
				// encode_mcu_gather    => jchuff_encode_mcu_gather
				// INLINE is needed
#include "libjpeg/jdhuff.c"	// huff_entropy_ptr     => jdhuff_huff_entropy_ptr
				// process_restart      => jdhuff_process_restart
				// decode_mcu_DC_first  => jdhuff_decode_mcu_DC_first
				// decode_mcu_AC_first  => jdhuff_decode_mcu_AC_first
				// decode_mcu_DC_refine => jdhuff_decode_mcu_DC_refine
				// decode_mcu_AC_refine => jdhuff_decode_mcu_AC_refine
				// decode_mcu           => jdhuff_decode_mcu
				// decode_mcu_sub       => jdhuff_decode_mcu_sub

#include "libjpeg/jctrans.c"	// start_pass_coef => jctrans_start_pass_coef
				// start_iMCU_row  => jctrans_start_iMCU_row
				// compress_output => jctrans_compress_output
#include "libjpeg/jdtrans.c"

#include "libjpeg/jdmerge.c"	// my_upsampler        => jdmerge_my_upsampler
				// my_upsample_ptr     => jdmerge_my_upsample_ptr
				// build_ycc_rgb_table => jdmerge_build_ycc_rgb_table
				// FIX                 => jdmerge_FIX

#include "libjpeg/jidctflt.c"	// DEQUANTIZE => jidctflt_DEQUANTIZE
#include "libjpeg/jidctfst.c"	// FIX        => jidctfst_FIX
				// CONST_BITS => jidctfst_CONST_BITS
				// DEQUANTIZE => jidctfst_DEQUANTIZE
				// MULTIPLY   => jidctfst_MULTIPLY
#include "libjpeg/jidctint.c"	// MULTIPLY   => jidctint_MULTIPLY
				// FIX*       => jidctint_FIX*

#include "libjpeg/jquant1.c"	// FSERROR          => jquant1_FSERROR
				// LOCFSERROR       => jquant1_LOCFSERROR
				// FSERRPTR         => jquant1_FSERRPTR
				// my_cquantizer    => jquant1_my_cquantizer
				// my_cquantize_ptr => jquant1_my_cquantize_ptr
#include "libjpeg/jquant2.c"	// FSERROR          => jquant2_FSERROR
				// LOCFSERROR       => jquant2_LOCFSERROR
				// FSERRPTR         => jquant2_FSERRPTR
				// my_cquantizer    => jquant2_my_cquantizer
				// my_cquantize_ptr => jquant2_my_cquantize_ptr




#endif // _H_NONNON_FLOSS_LIBJPEG


