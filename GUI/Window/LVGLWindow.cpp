#include "LVGLWindow.h"
#include "../../Tasks/WindowManager/Style.h"

static void lv_mywin_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_mywin_class = {
		.base_class = &lv_obj_class,
		.constructor_cb = lv_mywin_constructor,
		.width_def = LV_PCT(100),
		.height_def = LV_PCT(100),
		.instance_size = sizeof(lv_mywin_t)
};
static lv_coord_t create_header_height;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* lv_mywin_create(lv_obj_t* parent, lv_coord_t header_height, WindowAttributes* wa)
{
	LV_LOG_INFO("begin");
	create_header_height = header_height;

	lv_obj_t* obj = lv_obj_class_create_obj(&lv_mywin_class, parent);
	lv_obj_class_init_obj(obj);
	obj->user_data = wa;

	if (wa->resizable) {
		auto btn_resize = lv_btn_create(obj);
		auto furniture_width = ThemeManager::GetConst(ConstAttribute::WindowFurnitureWidth);
		lv_obj_set_size(btn_resize, furniture_width, furniture_width);
		lv_obj_add_flag(btn_resize, LV_OBJ_FLAG_FLOATING);
		lv_obj_add_event_cb(btn_resize, ResizePressed, LV_EVENT_PRESSING, NULL);
		lv_obj_align(btn_resize, LV_ALIGN_BOTTOM_RIGHT, -4, -4);
		lv_obj_t* img = lv_img_create(btn_resize);
		lv_img_set_src(img, LV_SYMBOL_RESIZE);
		lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
		auto style = ThemeManager::GetStyle(StyleAttribute::WindowButton);
		lv_obj_add_style(btn_resize, style, LV_STATE_DEFAULT);
	}

	return obj;
}

lv_obj_t* lv_mywin_add_title(lv_obj_t* win, const char* txt)
{
	lv_obj_t* header = lv_mywin_get_header(win);
	lv_obj_t* title = lv_label_create(header);
	lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
	lv_label_set_text(title, txt);
	lv_obj_set_flex_grow(title, 1);
	return title;
}

lv_obj_t* lv_mywin_add_btn(lv_obj_t* win, const void* icon, lv_coord_t btn_w)
{
	lv_obj_t* header = lv_mywin_get_header(win);
	lv_obj_t* btn = lv_btn_create(header);
	lv_obj_set_size(btn, btn_w, LV_PCT(100));

	lv_obj_t* img = lv_img_create(btn);
	lv_img_set_src(img, icon);
	lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

	return btn;
}

lv_obj_t* lv_mywin_get_header(lv_obj_t* win)
{
	return lv_obj_get_child(win, 0);
}

lv_obj_t* lv_mywin_get_content(lv_obj_t* win)
{
	return lv_obj_get_child(win, 1);
}

lv_obj_t* lv_mywin_get_resizebutton(lv_obj_t* win)
{
	return lv_obj_get_child(win, 2);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ResizePressed(_lv_event_t* e)
{
	lv_obj_t* obj = lv_event_get_target(e);

	lv_indev_t* indev = lv_indev_get_act();
	if (indev==NULL) {
		return;
	}

	lv_point_t vect;
	lv_indev_get_vect(indev, &vect);

	auto win = obj->parent;
	auto wa = (WindowAttributes*)win->user_data;
	auto content = lv_mywin_get_content(win);

	lv_coord_t x = lv_obj_get_width(win)+vect.x;
	lv_coord_t y = lv_obj_get_height(win)+vect.y;

	// Resize shouldn't push beyond content size
/*	if (wa->fixed_size_content) {
		auto canvas = lv_obj_get_child(content, 0);
		auto x2 = lv_obj_get_width(canvas)+vect.x;
		auto y2 = lv_obj_get_height(canvas)+vect.y;
		auto sw = lv_obj_get_width(canvas);
		auto sh = lv_obj_get_height(canvas);
		if (x2>sw)
			x = lv_obj_get_width(win);
		if (y2>sh)
			y = lv_obj_get_height(win);
	}*/
	if (x<160)
		x = 160;
	if (y<80)
		y = 80;

	lv_obj_set_size(win, x, y);
}

static void lv_mywin_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
	// Attributes
	auto wa = (WindowAttributes*)obj->user_data;
	auto style_window = ThemeManager::GetStyle(StyleAttribute::Window);
	auto style_header = ThemeManager::GetStyle(StyleAttribute::WindowHeader);
	auto style_content = ThemeManager::GetStyle(StyleAttribute::WindowContent);
	auto style_scrollbar = ThemeManager::GetStyle(StyleAttribute::Scrollbar);

	LV_UNUSED(class_p);
	lv_obj_t* parent = lv_obj_get_parent(obj);
	lv_obj_set_size(obj, lv_obj_get_width(parent), lv_obj_get_height(parent));
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(obj, style_window, LV_STATE_DEFAULT);
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t* header = lv_obj_create(obj);
	lv_obj_set_size(header, LV_PCT(100), create_header_height);
	lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(header, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(header, style_header, LV_STATE_DEFAULT);

	lv_obj_t* body = lv_obj_create(obj);
	lv_obj_set_flex_grow(body, 1);
	lv_obj_set_width(body, LV_PCT(100));
	lv_obj_set_height(body, LV_PCT(100));
	//lv_obj_set_width(body, LV_SIZE_CONTENT);
	//lv_obj_set_height(body, LV_SIZE_CONTENT);
	lv_obj_add_style(body, style_content, LV_STATE_DEFAULT);
	lv_obj_set_scrollbar_mode(body, LV_SCROLLBAR_MODE_AUTO);
//	lv_obj_set_scrollbar_mode(body, LV_SCROLLBAR_MODE_ON);
	lv_obj_add_style(body, style_scrollbar, LV_PART_SCROLLBAR);
}


