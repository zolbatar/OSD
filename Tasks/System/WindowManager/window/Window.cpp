#include "Window.h"

std::map<std::string, Window *> Window::windows;

Window::Window(OSDTask *task, bool pure_canvas, bool fixed, std::string title, int x, int y, int w, int h, int canvas_w,
               int canvas_h)
    : task(task), title(title), x1(x), y1(y), width(w), height(h)
{
    x2 = x1 + width;
    y2 = y1 + height;

    // Set attributes
    WindowAttributes *wa = new WindowAttributes();
    wa->resizable = true;
    wa->fixed_size_content = pure_canvas;

    // Create
    win = lv_mywin_create(lv_scr_act(), ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight), wa);
    lv_obj_set_pos(win, x1, y1);
    lv_obj_set_width(win, width);
    lv_obj_set_height(win, height);

    // Header
    header = lv_mywin_get_header(win);
    lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowInactive), LV_STATE_DEFAULT);
    lv_obj_add_event_cb(header, ClickEventHandler, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(header, DragEventHandler, LV_EVENT_PRESSING, this);

    // Title
    lv_mywin_add_title(win, title.c_str());

    auto furniture_width = ThemeManager::GetConst(ConstAttribute::WindowFurnitureWidth);
    auto style = ThemeManager::GetStyle(StyleAttribute::WindowButton);

    auto btn_min = lv_mywin_add_btn(win, LV_SYMBOL_MINIMISE, furniture_width);
    lv_obj_add_event_cb(btn_min, MinimiseClicked, LV_EVENT_CLICKED, this);
    lv_obj_add_style(btn_min, style, LV_STATE_DEFAULT);

    auto btn_max = lv_mywin_add_btn(win, LV_SYMBOL_MAXIMISE, furniture_width);
    lv_obj_add_event_cb(btn_max, MaximiseClicked, LV_EVENT_CLICKED, this);
    lv_obj_add_style(btn_max, style, LV_STATE_DEFAULT);

    auto btn_close = lv_mywin_add_btn(win, LV_SYMBOL_MY_CLOSE, furniture_width);
    lv_obj_add_event_cb(btn_close, CloseClicked, LV_EVENT_CLICKED, this);
    lv_obj_add_style(btn_close, style, LV_STATE_DEFAULT);

    if (pure_canvas)
    {
        CreateCanvas(canvas_w, canvas_h);
    }
}

void Window::CreateCanvas(int canvas_w, int canvas_h)
{
    auto content = lv_mywin_get_content(win);
    canvas = new Canvas(task, content, canvas_w, canvas_h);
}

void Window::DeleteCanvas()
{
    if (canvas != NULL)
        delete canvas;
    canvas = NULL;
}

Window::~Window()
{
    if (canvas != NULL)
        delete canvas;
    lv_obj_del(win);
}

void Window::SetActive()
{
    // Set all others as inactive
    for (auto &w : windows)
        w.second->SetInactive();

    lv_obj_remove_style(header, ThemeManager::GetStyle(StyleAttribute::WindowInactive), 0);
    lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowActive), 0);
    lv_obj_move_foreground(this->GetLVGLWindow());
}

void Window::SetInactive()
{
    lv_obj_remove_style(header, ThemeManager::GetStyle(StyleAttribute::WindowActive), 0);
    lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowInactive), 0);
}

void Window::CloseClicked(_lv_event_t *e)
{
    auto w = (Window *)e->user_data;
    w->task->RequestTerminate();
}

void Window::MinimiseClicked(_lv_event_t *e)
{
    auto win = (Window *)e->user_data;
    auto task = win->GetTask();
    task->RequestMinimise();
}

void Window::MaximiseClicked(_lv_event_t *e)
{
    auto win = (Window *)e->user_data;
    auto task = win->GetTask();
    task->RequestMaximise();
}

void Window::ClickEventHandler(lv_event_t *e)
{
    auto win = (Window *)e->user_data;
    OSDTask::SetActive(win->task);
}

void Window::DragEventHandler(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);

    lv_indev_t *indev = lv_indev_get_act();
    if (indev == NULL)
    {
        return;
    }

    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);

    lv_obj_t *win = lv_obj_get_parent(obj);

    // Set pos
    lv_coord_t x = lv_obj_get_x(win) + vect.x;
    lv_coord_t y = lv_obj_get_y(win) + vect.y;

    // Get size of window, will it fit?
    auto w = lv_obj_get_width(win);
    auto h = lv_obj_get_height(win);
    auto sw = lv_obj_get_width(lv_scr_act());
    auto sh = lv_obj_get_height(lv_scr_act());
    if (x < 0)
        x = 0;
    if (x + w > sw)
        x = sw - w;
    if (y < 0)
        y = 0;
    if (y + h > sh)
        y = sh - h;

    lv_obj_set_pos(win, x, y);

    // Event
    lv_event_send(win, (lv_event_code_t)OSD_EVENT_MOVED, NULL);
}

void Window::Maximise(bool full_maximise)
{
    if (!maximised)
    {
        maximised = true;

        // Save position to restore
        x1 = lv_obj_get_x(win);
        y1 = lv_obj_get_y(win);
        width = lv_obj_get_width(win);
        height = lv_obj_get_height(win);
        x2 = x1 + width;
        y2 = y1 + height;

        // Maximise
        lv_obj_set_pos(win, 0, 0);
        auto w = lv_obj_get_width(lv_scr_act());
        lv_coord_t h;
        if (!full_maximise)
            h = lv_obj_get_height(lv_scr_act()) - ThemeManager::GetConst(ConstAttribute::IconBarHeight);
        else
            h = lv_obj_get_height(lv_scr_act());
        lv_obj_set_width(win, w);
        lv_obj_set_height(win, h);

        if (full_maximise)
        {
            // Set styles to hide all the window stuff
            lv_obj_remove_style(win, ThemeManager::GetStyle(StyleAttribute::Window), LV_STATE_DEFAULT);
            lv_obj_add_style(win, ThemeManager::GetStyle(StyleAttribute::WindowFullScreen), LV_STATE_DEFAULT);
            auto header = lv_mywin_get_header(win);
            lv_obj_remove_style(header, ThemeManager::GetStyle(StyleAttribute::WindowHeader), LV_STATE_DEFAULT);
            lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowHeaderFullScreen), LV_STATE_DEFAULT);
            lv_obj_set_size(header, LV_PCT(100), 0);

            // Hide resize button
            auto resize = lv_mywin_get_resizebutton(win);
            lv_obj_add_flag(resize, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else
    {
        maximised = false;

        // Restore
        lv_obj_set_pos(win, x1, y1);
        lv_obj_set_width(win, width);
        lv_obj_set_height(win, height);
    }
}
