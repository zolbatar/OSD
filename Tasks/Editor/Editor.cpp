#include <circle/logger.h>
#include "Editor.h"
#include "../../Library/StringLib.h"
#include "../../GUI/Window/LVGLWindow.h"
#include "../System/Menu/Menu.h"
#include "../System/IconBar/IconBar.h"
#include "../System/WindowManager/Style.h"

Editor::Editor(int x, int y, int w, int h)
{
    this->d_x = x;
    this->d_y = y;
    this->d_w = w;
    this->d_h = h;
    auto window_border_width = ThemeManager::GetConst(ConstAttribute::WindowBorderWidth);
    auto window_content_padding = ThemeManager::GetConst(ConstAttribute::WindowContentPaddingPadded);
    auto window_header_height = ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight);
    this->canvas_w = w - window_border_width * 2 - window_content_padding * 2;
    this->canvas_h = h - window_border_width * 2 - window_header_height - window_content_padding * 2;
    this->id = "@" + std::to_string(task_id++);
    this->name = "Editor";
    this->type = TaskType::Editor;
}

Editor::~Editor()
{
    lv_obj_del(buttons);
    lv_obj_del(debug);
}

void Editor::Run()
{
    SetNameAndAddToList();

    // Create Window
    DirectMessage mess;
    mess.type = Messages::WM_OpenWindow;
    mess.source = this;
    WM_OpenWindow m;
    mess.data = &m;
    strcpy(m.id, id.c_str());
    strcpy(m.title, name.c_str());
    m.x = d_x;
    m.y = d_y;
    m.width = d_w;
    m.height = d_h;
    m.canvas = false;
    m.fixed = true;
    CallGUIDirectEx(&mess);

    // App icon
    IconBar::RegisterApp(NULL, "Editor", WindowManager::GetIcon("Editor"), NULL, NULL);

    // Get window
    auto window = (Window *)this->GetWindow();
    auto ww = window->GetLVGLWindow();
    auto content = lv_mywin_get_content(ww);

    // Set up canvas
    obj = lv_obj_create(content);
    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(obj, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);
    lv_obj_add_event_cb(obj, ContextMenuEventHandler, LV_EVENT_LONG_PRESSED, this);

    // Scroll event
    lv_obj_add_event_cb(content, ScrollEventHandler, LV_EVENT_SCROLL, this);
    lv_obj_add_event_cb(ww, MoveEventHandler, (lv_event_code_t)OSD_EVENT_MOVED, this);
    lv_obj_add_event_cb(content, ResizeEventHandler, LV_EVENT_SIZE_CHANGED, this);

    // Buttons
    buttons = lv_obj_create(lv_scr_act());
    lv_obj_update_layout(ww);
    auto x = lv_obj_get_x(ww);
    auto y = lv_obj_get_y(ww);
    lv_obj_set_pos(buttons, x - 32 - ThemeManager::GetConst(ConstAttribute::WindowBorderWidth),
                   y + ThemeManager::GetConst(ConstAttribute::MenuHeaderHeight));
    const int num_buttons = 3;
    lv_obj_set_size(buttons, 34, 32 * num_buttons + 4);
    lv_obj_set_flex_flow(buttons, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(buttons, ThemeManager::GetStyle(StyleAttribute::Drawer), LV_STATE_DEFAULT);
    auto style = ThemeManager::GetStyle(StyleAttribute::WindowButton);
    {
        lv_obj_t *btn = lv_btn_create(buttons);
        lv_obj_set_size(btn, 28, 28);
        lv_obj_set_flex_grow(btn, 1);
        lv_obj_t *img = lv_img_create(btn);
        lv_img_set_src(img, LV_SYMBOL_BUILD);
        lv_obj_center(img);
        lv_obj_add_event_cb(btn, BuildHandler, LV_EVENT_CLICKED, this);
        lv_obj_add_style(btn, style, LV_STATE_DEFAULT);
    }
    {
        lv_obj_t *btn = lv_btn_create(buttons);
        lv_obj_set_size(btn, 28, 28);
        lv_obj_set_flex_grow(btn, 1);
        lv_obj_t *img = lv_img_create(btn);
        lv_img_set_src(img, LV_SYMBOL_WINDOWED);
        lv_obj_add_event_cb(btn, RunFullScreenHandler, LV_EVENT_CLICKED, this);
        lv_obj_center(img);
        lv_obj_add_style(btn, style, LV_STATE_DEFAULT);
    }
    {
        lv_obj_t *btn = lv_btn_create(buttons);
        lv_obj_set_size(btn, 28, 28);
        lv_obj_set_flex_grow(btn, 1);
        lv_obj_t *img = lv_img_create(btn);
        lv_img_set_src(img, LV_SYMBOL_FULLSCREEN);
        lv_obj_add_event_cb(btn, RunHandler, LV_EVENT_CLICKED, this);
        lv_obj_center(img);
        lv_obj_add_style(btn, style, LV_STATE_DEFAULT);
    }

    // Disassembly/debug drawer
    debug = lv_obj_create(lv_scr_act());
    lv_obj_update_layout(ww);
    x = lv_obj_get_x(ww);
    y = lv_obj_get_y(ww);
    auto w = lv_obj_get_width(ww);
    auto h = lv_obj_get_height(ww);
    lv_obj_set_pos(debug, x + w, y + ThemeManager::GetConst(ConstAttribute::MenuHeaderHeight));
    lv_obj_set_size(debug, 200, h - ThemeManager::GetConst(ConstAttribute::MenuHeaderHeight));
    lv_obj_add_style(debug, ThemeManager::GetStyle(StyleAttribute::DrawerText), LV_STATE_DEFAULT);
    lv_obj_t *tabview = lv_tabview_create(debug, LV_DIR_RIGHT, LV_PCT(25));
    //    lv_obj_set_style_pad_all(tabview, 0, LV_STATE_DEFAULT);
    //    lv_obj_set_style_border_width(tabview, 0, LV_STATE_DEFAULT);
    //    lv_obj_add_event_cb(lv_tabview_get_content(tabview), scroll_begin_event, LV_EVENT_SCROLL_BEGIN, NULL);
    //    lv_obj_set_style_bg_color(tabview, lv_palette_lighten(LV_PALETTE_RED, 2), 0);
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_border_width(tab_btns, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(tab_btns, ThemeManager::GetColour(ColourAttribute::WindowBorder), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 1), 0);
    lv_obj_set_style_text_color(tab_btns, lv_color_white(), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_LEFT, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "T");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "P");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "IR");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "MC");
    lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);
    lv_obj_t *label = lv_label_create(tab1);
    lv_label_set_text(label, "First tab");
    label = lv_label_create(tab2);
    lv_label_set_text(label, "Second tab");
    label = lv_label_create(tab3);
    lv_label_set_text(label, "Third tab");
    label = lv_label_create(tab4);
    lv_label_set_text(label, "Forth tab");
    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);

    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);

    // Do stuff
    while (1)
    {
        Yield();

        // Process keyboard queue
        while (!keyboard_queue.empty())
        {
            auto k = keyboard_queue.front();
            keyboard_queue.pop();

            // Process
            if (k.ascii == 0)
            {
                switch (k.keycode)
                {
                case KEY_F1: {
                    RunWindowed();
                    break;
                }
                case KEY_F2: {
                    FullscreenRun();
                    break;
                }
                case KEY_Insert:
                    if (mode == Mode::Overwrite)
                        mode = Mode::Insert;
                    else
                        mode = Mode::Overwrite;
                case KEY_Up:
                    y--;
                    break;
                case KEY_Down:
                    y++;
                    break;
                case KEY_Left:
                    x--;
                    break;
                case KEY_Right:
                    x++;
                    break;
                case KEY_PageUp:
                    y -= canvas_h / size;
                    break;
                case KEY_PageDown:
                    y += canvas_h / size;
                    break;
                default:
                    CLogger::Get()->Write("Editor", LogNotice, "Key: %d %x", k.ascii, k.keycode);
                }
            }
            else
            {
                if (k.ascii == 10)
                {
                    auto l1 = code[y].substr(0, x);
                    auto l2 = code[y].substr(x);
                    code[y] = l1;
                    code.insert(code.begin() + y + 1, l2);
                    y++;
                    x = 0;
                    // Break line at this point
                }
                else
                {
                    CLogger::Get()->Write("Editor", LogNotice, "%d", k.ascii);
                    // normal character
                    if (mode == Mode::Overwrite)
                    {
                        code[y][x] = k.ascii;
                        x++;
                    }
                    else
                    {
                        code[y].insert(x, std::string(1, k.ascii));
                        CalculateLongestLine();
                    }
                }
            }
            Render();
        }
    }
}

void Editor::LoadSourceCode(std::string volume, std::string directory, std::string filename)
{
    SetOverride(this);
    this->volume = volume;
    this->directory = directory;
    this->filename = filename;
    fs.SetVolume(volume);
    fs.SetCurrentDirectory(directory);
    filename = fs.GetCurrentDirectory() + filename;

    FIL fil;
    if (f_open(&fil, (filename).c_str(), FA_READ | FA_OPEN_EXISTING) != FR_OK)
    {
        CLogger::Get()->Write("Editor", LogPanic, "Error opening source file '%s'", filename.c_str());
    }
    size_t sz = f_size(&fil);
    char *buffer = (char *)malloc(sz + 1);
    if (!buffer)
    {
        CLogger::Get()->Write("Editor", LogPanic, "Error allocating memory for source file '%s'", filename.c_str());
    }
    uint32_t l;
    if (f_read(&fil, buffer, sz, &l) != FR_OK)
    {
        CLogger::Get()->Write("Editor", LogPanic, "Error loading source file '%s'", filename.c_str());
    }
    f_close(&fil);
    buffer[sz] = 0;
    std::string s(buffer);
    free(buffer);

    // Split into lines
    auto ll = splitString(s, '\n');
    code.reserve(ll.size());
    std::copy(std::begin(ll), std::end(ll), std::back_inserter(code));
    CalculateLongestLine();

    ClearOverride();
}

void Editor::CalculateLongestLine()
{
    longest_line = 0;
    for (auto &c : code)
    {
        longest_line = std::max(longest_line, c.length());
    }
}

void Editor::Render()
{
    auto window = (Window *)this->GetWindow();
    auto canvas = window->GetCanvas();
    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
    lv_obj_set_size(obj, longest_line * size, code.size() * size);

    // Clear
    canvas->Clear();

    // Adjust cursor etc.
    if (y < 0)
        y = 0;
    if (y >= static_cast<int>(code.size()))
        y = static_cast<int>(code.size()) - 1;
    if (x < 0)
        x = 0;
    if (x >= static_cast<int>(code[y].size()))
        x = static_cast<int>(code[y].size()) - 1;
    bool adjust = false;
    do
    {
        adjust = false;
        int bottom_y = screen_y + (canvas_h / size) - 1;
        if (y < screen_y)
        {
            //					CLogger::Get()->Write("Editor", LogNotice, "Y<: %d %d", y, screen_y);
            screen_y--;
            // screen_y -= (canvas_h/size);
            adjust = true;
        }
        if (y > bottom_y)
        {
            //					CLogger::Get()->Write("Editor", LogNotice, "Y>: %d %d", y, bottom_y);
            screen_y++;
            // screen_y += (canvas_h/size);
            adjust = true;
        }
    } while (adjust);

    // Cursor
    int64_t ex = (x - screen_x) * (size / 2);
    int64_t ey = (y - screen_y) * size;
    canvas->SetBG(0xD0D0D0);
    canvas->DrawRectangleFilled(ex, ey, ex + size / 2, ey + size, 1);
    canvas->SetBG(0xFFFFFF);

    // Text
    int actual_y = 0;
    for (size_t i = screen_y; i < code.size(); i++)
    {

        // Get line
        auto line = code[i];

        int actual_x = 0;
        for (size_t j = screen_x; j < line.length(); j++)
        {
            char c = line[j];

            // Draw
            if (c == 8)
            {
                canvas->DrawMonoText(actual_x, actual_y, std::string(4, c));
            }
            else
            {
                canvas->DrawMonoText(actual_x, actual_y, std::string(1, c));
            }

            // Next character
            actual_x += size / 2;
            if (actual_x > canvas_w)
                break;
        }

        // Next line
        actual_y += size;
        if (actual_y > canvas_h)
            break;
    }
}

void Editor::ScrollEventHandler(lv_event_t *e)
{
    lv_obj_t *scroll = lv_event_get_target(e);
    auto scroll_y = lv_obj_get_scroll_top(scroll);
    auto scroll_x = lv_obj_get_scroll_left(scroll);
    if (scroll_x < 0)
        scroll_x = 0;
    if (scroll_y < 0)
        scroll_y = 0;

    // Work out X and Y
    auto editor = (Editor *)e->user_data;
    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
    editor->x = scroll_x / size / 2;
    editor->y = scroll_y / size;
    editor->screen_x = scroll_x / size / 2;
    editor->screen_y = scroll_y / size;

    // Render
    editor->Render();
}

void Editor::MoveEventHandler(lv_event_t *e)
{
    auto editor = (Editor *)e->user_data;
    auto window = (Window *)editor->GetWindow();
    auto ww = window->GetLVGLWindow();
    lv_area_t sz;
    lv_obj_get_coords(ww, &sz);

    // Move button drawer
    lv_obj_set_pos(editor->GetButtons(), sz.x1 - 32 - ThemeManager::GetConst(ConstAttribute::WindowBorderWidth),
                   sz.y1 + ThemeManager::GetConst(ConstAttribute::MenuHeaderHeight));
}

void Editor::ResizeEventHandler(lv_event_t *e)
{
    auto editor = (Editor *)e->user_data;
    auto window = (Window *)editor->GetWindow();
    auto ww = window->GetLVGLWindow();
    lv_area_t sz;
    lv_obj_get_coords(ww, &sz);

    // Set new size
    editor->d_w = sz.x2 - sz.x1;
    editor->d_h = sz.y2 - sz.y1;

    // Resize/redraw canvas
    auto window_border_width = ThemeManager::GetConst(ConstAttribute::WindowBorderWidth);
    auto window_content_padding = ThemeManager::GetConst(ConstAttribute::WindowContentPaddingPadded);
    auto window_header_height = ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight);
    window->DeleteCanvas();
    editor->canvas_w = editor->d_w - window_border_width * 2 - window_content_padding * 2;
    editor->canvas_h = editor->d_h - window_border_width * 2 - window_header_height - window_content_padding * 2;
    window->CreateCanvas(editor->canvas_w, editor->canvas_h);

    // Reset canvas styling
    auto canvas = window->GetCanvas();
    canvas->SetBG(0xFFFFFF);
    canvas->SetFG(0x0);
    lv_obj_add_flag(canvas->GetFirstBuffer(), LV_OBJ_FLAG_FLOATING);
    lv_obj_align(canvas->GetFirstBuffer(), LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_style(canvas->GetFirstBuffer(), ThemeManager::GetStyle(StyleAttribute::BorderedContent),
                     LV_STATE_DEFAULT);

    // Render
    editor->Render();

    //	CLogger::Get()->Write("File Manager", LogNotice, "Resize %d %d/%d %d", editor->canvas_w, editor->canvas_h);
}

void Editor::ContextMenuEventHandler(lv_event_t *e)
{
    /*    auto editor = (Editor *)e->user_data;
        e->stop_bubbling = 1;
        lv_point_t p;
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if (indev_type == LV_INDEV_TYPE_POINTER)
        {
            lv_indev_get_point(indev, &p);

            // Create menu window
            MenuDefinition menu;

            // View style
            {
                MenuItem mi;
                mi.type = MenuItemType::Item;
                mi.v = "Run windowed";
                mi.shortcut = "F1";
                mi.cb = &Editor::RunHandler;
                mi.user_data = e->user_data;
                menu.items.push_back(std::move(mi));
            }
            {
                MenuItem mi;
                mi.type = MenuItemType::Item;
                mi.v = "Run full screen";
                mi.shortcut = "F2";
                mi.cb = &Editor::RunFullScreenHandler;
                mi.user_data = e->user_data;
                menu.items.push_back(std::move(mi));
            }
            {
                MenuItem mi;
                mi.type = MenuItemType::Item;
                mi.v = "Debug output";
                mi.shortcut = "F3";
                menu.items.push_back(std::move(mi));
            }
            Menu::OpenMenu(p.x, p.y, NULL, "Editor", std::move(menu));
        }*/
}

void Editor::RunHandler(lv_event_t *e)
{
    auto editor = (Editor *)e->user_data;
    editor->RunWindowed();
}

void Editor::RunFullScreenHandler(lv_event_t *e)
{
    auto editor = (Editor *)e->user_data;
    editor->FullscreenRun();
}

void Editor::BuildHandler(lv_event_t *e)
{
    auto editor = (Editor *)e->user_data;
    editor->Debug();
}

void Editor::RunWindowed()
{
    auto app = new DARICWindow(volume, directory, filename, "DARIC", false, true, 100, 100, 640, 512, 1920, 1080);
    std::string c;
    for (auto &l : code)
    {
        c += l + "\n";
    }
    app->SetCode(c);
    app->Start();
}

void Editor::FullscreenRun()
{
    auto app = new DARICWindow(volume, directory, filename, "DARIC", true, true, 100, 100, 640, 512, 1920, 1080);
    std::string c;
    for (auto &l : code)
    {
        c += l + "\n";
    }
    app->SetCode(c);
    app->Start();
}

void Editor::Debug()
{
    std::string c;
    for (auto &l : code)
    {
        c += l + "\n";
    }
    CompileSource(volume, directory, filename, c, true);
}
