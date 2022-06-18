#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
	lv_obj_t obj;
} lv_mywin_t;

extern const lv_obj_class_t lv_mywin_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t* lv_mywin_create(lv_obj_t* parent, lv_coord_t header_height);

lv_obj_t* lv_mywin_add_title(lv_obj_t* win, const char* txt);
lv_obj_t* lv_mywin_add_btn(lv_obj_t* win, const void* icon, lv_coord_t btn_w);

lv_obj_t* lv_mywin_get_header(lv_obj_t* win);
lv_obj_t* lv_mywin_get_content(lv_obj_t* win);

#ifdef __cplusplus
} /*extern "C"*/
#endif
