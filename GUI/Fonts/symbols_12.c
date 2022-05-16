/*******************************************************************************
 * Size: 12 px
 * Bpp: 4
 * Opts: 
 ******************************************************************************/

#ifdef CLION
#include <lvgl.h>
#else
#include <lvgl/lvgl/lvgl.h>
#endif

#ifndef SYMBOLS_12
#define SYMBOLS_12 1
#endif

#if SYMBOLS_12

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
		/* U+F00D "" */
		0x2, 0x0, 0x0, 0x0, 0x10, 0xa5, 0x0, 0x0,
		0xb4, 0x0, 0xb5, 0x0, 0xb5, 0x0, 0x0, 0xb5,
		0xb5, 0x0, 0x0, 0x1, 0xf8, 0x0, 0x0, 0x0,
		0xb5, 0xb5, 0x0, 0x0, 0xb5, 0x0, 0xb5, 0x0,
		0xa4, 0x0, 0x0, 0xb4, 0x1, 0x0, 0x0, 0x0,
		0x10,

		/* U+F066 "" */
		0x0, 0x7, 0x0, 0x7, 0x0, 0x0, 0x0, 0xc0,
		0x0, 0xb0, 0x0, 0x0, 0xc, 0x0, 0xb, 0x0,
		0xa, 0xcc, 0xb0, 0x0, 0xec, 0xc7, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa, 0xcc,
		0xb0, 0x0, 0xec, 0xc7, 0x0, 0xc, 0x0, 0xb,
		0x0, 0x0, 0x0, 0xc0, 0x0, 0xb0, 0x0, 0x0,
		0x7, 0x0, 0x7, 0x0, 0x0,

		/* U+F2D0 "" */
		0x5b, 0xcc, 0xcc, 0xcc, 0xcc, 0xb5, 0xc0, 0x0,
		0x0, 0x0, 0x0, 0xc, 0xc0, 0x0, 0x0, 0x0,
		0x0, 0xc, 0xc0, 0x0, 0x0, 0x0, 0x0, 0xc,
		0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcf, 0xc0, 0x0,
		0x0, 0x0, 0x0, 0xc, 0xc0, 0x0, 0x0, 0x0,
		0x0, 0xc, 0xc0, 0x0, 0x0, 0x0, 0x0, 0xc,
		0xc0, 0x0, 0x0, 0x0, 0x0, 0xc, 0xc0, 0x0,
		0x0, 0x0, 0x0, 0xc, 0x5b, 0xcc, 0xcc, 0xcc,
		0xcc, 0xb5,

		/* U+F2D1 "" */
		0xac, 0xcc, 0xcc, 0xcc, 0xcc, 0xca
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
		{.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
		{.bitmap_index = 0, .adv_w = 120, .box_w = 9, .box_h = 9, .ofs_x = -1, .ofs_y = 0},
		{.bitmap_index = 41, .adv_w = 168, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
		{.bitmap_index = 102, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
		{.bitmap_index = 168, .adv_w = 192, .box_w = 12, .box_h = 1, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
		0x0, 0x59, 0x2c3, 0x2c4
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
		{
		{
			.range_start = 61453, .range_length = 709, .glyph_id_start = 1,
			.unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
		}
		};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
		static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
				.glyph_bitmap = glyph_bitmap,
						.glyph_dsc = glyph_dsc,
						.cmaps = cmaps,
						.kern_dsc = NULL,
						.kern_scale = 0,
						.cmap_num = 1,
						.bpp = 4,
						.kern_classes = 0,
						.bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
.cache = &cache
#endif
		};


	/*-----------------
	 *  PUBLIC FONT
	 *----------------*/

	/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
	const lv_font_t symbols_12 = {
#else
			lv_font_t symbols_12 = {
#endif
					.get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
							.get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
							.line_height = 11,          /*The maximum line height required by the font*/
							.base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
.subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
.underline_position = -1,
.underline_thickness = 1,
#endif
.dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
			};



#endif /*#if SYMBOLS_12*/


