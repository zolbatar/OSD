#include "Editor.h"
#include <Breakdown/Breakdown.h>
#include <IconBar/IconBar.h>
#include <Menu/Menu.h>
/*#include "../System/WindowManager/lvglwindow/LVGLWindow.h"
#include "../System/Menu/Menu.h"
#include "../System/IconBar/IconBar.h"
#include "../System/WindowManager/style/Style.h"
#include "../../../OS/Breakdown.h"*/

Editor::Editor(int x, int y, int w, int h)
{
    this->GUI.d_x = x;
    this->GUI.d_y = y;
    this->GUI.d_w = w;
    this->GUI.d_h = h;
    auto window_border_width = ThemeManager::GetConst(ConstAttribute::WindowBorderWidth);
    auto window_content_padding = ThemeManager::GetConst(ConstAttribute::WindowContentPaddingPadded);
    auto window_header_height = ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight);
    this->canvas_w = w - window_border_width * 2 - window_content_padding * 2;
    this->canvas_h = h - window_border_width * 2 - window_header_height - window_content_padding * 2;
    this->id = "@" + std::to_string(task_id++);
    this->SetName("Editor");
    this->type = TaskType::Editor;
}

Editor::~Editor()
{
}

void Editor::ScrollBeginEvent(lv_event_t *e)
{
    /*Disable the scroll animations. Triggered when a tab button is clicked */
    if (lv_event_get_code(e) == LV_EVENT_SCROLL_BEGIN)
    {
        lv_anim_t *a = (lv_anim_t *)lv_event_get_param(e);
        if (a)
            a->time = 0;
    }
}

void Editor::Run()
{
    SetNameAndAddToList();

    Breakdown::Init();

    // Create Window
    DirectMessage mess;
    mess.type = Messages::WM_OpenWindow;
    mess.source = this;
    WM_OpenWindow m;
    mess.data = &m;
    strcpy(m.id, id.c_str());
    strcpy(m.title, GetName());
    m.x = GUI.d_x;
    m.y = GUI.d_y;
    m.width = GUI.d_w;
    m.height = GUI.d_h;
    m.canvas = false;
    m.fixed = true;
    CallGUIDirectEx(&mess);

    // App icon
    IconBar::RegisterApp(NULL, "Editor", WindowManager::GetIcon("Application/CodeEditor"), NULL, NULL);

    // Get window
    auto window = (Window *)this->GUI.GetWindow();
    auto ww = window->GetLVGLWindow();
    auto content = lv_mywin_get_content(ww);

    // Grid layout
    lv_obj_t *grid = lv_obj_create(content);
    lv_obj_set_style_grid_column_dsc_array(grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid, row_dsc, 0);
    lv_obj_set_size(grid, LV_PCT(100), LV_PCT(100));
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_row(grid, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(grid, 0, LV_STATE_DEFAULT);
    lv_obj_add_style(grid, ThemeManager::GetStyle(StyleAttribute::WindowContent), LV_STATE_DEFAULT);

    // Set up canvas
    obj_parent = lv_obj_create(grid);
    lv_obj_set_grid_cell(obj_parent, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_add_style(obj_parent, ThemeManager::GetStyle(StyleAttribute::Scrollbar), LV_PART_SCROLLBAR);
    lv_obj_add_style(obj_parent, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);
    lv_obj_add_event_cb(obj_parent, ScrollEventHandler, LV_EVENT_SCROLL, this);

    // Canvas child
    obj = lv_obj_create(obj_parent);
    lv_obj_add_style(obj, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    // Buttons
    auto buttons = lv_obj_create(grid);
    lv_obj_set_grid_cell(buttons, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_size(buttons, LV_PCT(100), 46);
    lv_obj_set_flex_flow(buttons, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(buttons, ThemeManager::GetStyle(StyleAttribute::WindowContentPadded), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(buttons, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(buttons, ThemeManager::GetColour(ColourAttribute::WindowBorder), LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(buttons, LV_BORDER_SIDE_BOTTOM, LV_STATE_DEFAULT);
    auto style = ThemeManager::GetStyle(StyleAttribute::Button);
    {
        lv_obj_t *btn = lv_btn_create(buttons);
        lv_obj_set_size(btn, 36, 36);
        lv_obj_t *img = lv_img_create(btn);
        lv_img_set_src(img, LV_SYMBOL_BUILD);
        lv_obj_center(img);
        lv_obj_add_event_cb(btn, BuildHandler, LV_EVENT_CLICKED, this);
        lv_obj_add_style(btn, style, LV_STATE_DEFAULT);
    }
    {
        lv_obj_t *btn = lv_btn_create(buttons);
        lv_obj_set_size(btn, 36, 36);
        lv_obj_t *img = lv_img_create(btn);
        lv_img_set_src(img, LV_SYMBOL_WINDOWED);
        lv_obj_add_event_cb(btn, RunHandler, LV_EVENT_CLICKED, this);
        lv_obj_center(img);
        lv_obj_add_style(btn, style, LV_STATE_DEFAULT);
    }
    {
        lv_obj_t *btn = lv_btn_create(buttons);
        lv_obj_set_size(btn, 36, 36);
        lv_obj_t *img = lv_img_create(btn);
        lv_img_set_src(img, LV_SYMBOL_FULLSCREEN);
        lv_obj_add_event_cb(btn, RunFullScreenHandler, LV_EVENT_CLICKED, this);
        lv_obj_center(img);
        lv_obj_add_style(btn, style, LV_STATE_DEFAULT);
    }

    // Status window
    auto status = lv_obj_create(grid);
    lv_obj_set_grid_cell(status, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_size(status, LV_PCT(100), 150);
    lv_obj_add_style(status, ThemeManager::GetStyle(StyleAttribute::WindowContentPadded), LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(status, ThemeManager::GetColour(ColourAttribute::WindowBorder), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(status, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(status, LV_BORDER_SIDE_TOP, LV_STATE_DEFAULT);
    status_text = lv_textarea_create(status);
    lv_textarea_set_one_line(status_text, false);
    lv_obj_set_size(status_text, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(status_text, ThemeManager::GetStyle(StyleAttribute::Grid), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(status_text, ThemeManager::GetFont(FontAttribute::Mono), LV_STATE_DEFAULT);
    lv_textarea_set_text(status_text, "Compile took 0.0 seconds");

    // Disassembly/debug drawer
    auto debug = lv_obj_create(grid);
    lv_obj_set_grid_cell(debug, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_add_style(debug, ThemeManager::GetStyle(StyleAttribute::WindowContent), LV_STATE_DEFAULT);
    lv_obj_t *tabview = lv_tabview_create(debug, LV_DIR_TOP, 32);
    lv_obj_add_event_cb(lv_tabview_get_content(tabview), ScrollBeginEvent, LV_EVENT_SCROLL_BEGIN, NULL);
    lv_obj_add_style(tabview, ThemeManager::GetStyle(StyleAttribute::WindowContent), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(tabview, ThemeManager::GetColour(ColourAttribute::Buttonbackground), LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(tabview, ThemeManager::GetColour(ColourAttribute::WindowBorder), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(tabview, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(tabview, LV_BORDER_SIDE_LEFT, LV_STATE_DEFAULT);
    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, ThemeManager::GetColour(ColourAttribute::Buttonbackground), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(tab_btns, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(tab_btns, ThemeManager::GetFont(FontAttribute::Body), LV_STATE_DEFAULT);

    // Checked
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_font(tab_btns, ThemeManager::GetFont(FontAttribute::BodyBold),
                               LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(tab_btns, ThemeManager::GetColour(ColourAttribute::WindowBackground),
                              LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(tab_btns, lv_color_black(), LV_PART_ITEMS | LV_STATE_CHECKED);

    // Tabs
    auto tab_tokens = lv_tabview_add_tab(tabview, "Tokens");
    auto tab_parser = lv_tabview_add_tab(tabview, "Parser");
    auto tab_ir = lv_tabview_add_tab(tabview, "IR");
    auto tab_native = lv_tabview_add_tab(tabview, "Native");
    lv_obj_set_style_pad_all(tab_tokens, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab_parser, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab_ir, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(tab_native, 0, LV_STATE_DEFAULT);

    // Textareas
    auto grid_style = ThemeManager::GetStyle(StyleAttribute::Grid);
    ta1 = lv_textarea_create(tab_tokens);
    lv_textarea_set_one_line(ta1, false);
    lv_obj_set_size(ta1, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(ta1, grid_style, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ta1, ThemeManager::GetFont(FontAttribute::Mono), LV_STATE_DEFAULT);
    //        lv_textarea_set_cursor_hidden(ta1, true);
    ta2 = lv_textarea_create(tab_parser);
    lv_textarea_set_one_line(ta2, false);
    lv_obj_set_size(ta2, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(ta2, grid_style, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ta2, ThemeManager::GetFont(FontAttribute::Mono), LV_STATE_DEFAULT);
    //      lv_textarea_set_cursor_hidden(ta2, true);
    ta3 = lv_textarea_create(tab_ir);
    lv_textarea_set_one_line(ta3, false);
    lv_obj_set_size(ta3, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(ta3, grid_style, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ta3, ThemeManager::GetFont(FontAttribute::Mono), LV_STATE_DEFAULT);
    //        lv_textarea_set_cursor_hidden(ta3, true);
    ta4 = lv_textarea_create(tab_native);
    lv_textarea_set_one_line(ta4, false);
    lv_obj_set_size(ta4, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(ta4, grid_style, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ta4, ThemeManager::GetFont(FontAttribute::Mono), LV_STATE_DEFAULT);
    //        lv_textarea_set_cursor_hidden(ta4, true);

    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);

    Yield();
    lv_obj_add_event_cb(content, ResizeEventHandler, LV_EVENT_SIZE_CHANGED, this);

    // Do stuff
    SetupCanvas();
    while (1)
    {
        Yield();

        // Process keyboard queue
        bool update = false;
        while (!keyboard_queue.empty())
        {
            update = true;
            auto k = keyboard_queue.front();
            keyboard_queue.pop();

            // Process
            if (k.ascii == 0)
            {
                switch (k.keycode)
                {
                case KeyF1: {
                    RunWindowed();
                    break;
                }
                case KeyF2: {
                    FullscreenRun();
                    break;
                }
                case KeyInsert:
                    if (mode == Mode::Overwrite)
                        mode = Mode::Insert;
                    else
                        mode = Mode::Overwrite;
                case KeyUp:
                    this->y--;
                    break;
                case KeyDown:
                    this->y++;
                    break;
                case KeyLeft:
                    this->x--;
                    break;
                case KeyRight:
                    this->x++;
                    break;
                case KeyPageUp:
                    this->y -= canvas_h / size;
                    break;
                case KeyPageDown:
                    this->y += canvas_h / size;
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
        }
        if (update)
            Render();
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
    auto window = (Window *)this->GUI.GetWindow();
    auto canvas = window->GetCanvas();
    auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
    lv_obj_set_size(obj, longest_line * size + 4, code.size() * size + 4);

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
    canvas->SetFG(0xA0A0A0);
    canvas->SetBG(0xD0D0D0);
    canvas->DrawRectangleFilled(ex, ey, ex + size / 2, ey + size, 1);
    canvas->SetBG(0xFFFFFF);
    canvas->SetFG(0x0);

    UpdateDebugWindow();

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

void Editor::ResizeEventHandler(lv_event_t *e)
{
    auto editor = (Editor *)e->user_data;
    editor->SetupCanvas();
}

void Editor::SetupCanvas()
{
    auto window = (Window *)GUI.GetWindow();
    auto ww = window->GetLVGLWindow();
    if (obj == NULL || ww == NULL)
        return;
    lv_area_t sz;
    lv_obj_get_coords(obj_parent, &sz);
    //    CLogger::Get()->Write("File Manager", LogNotice, "%d %d %d %d", sz.x1, sz.x2, sz.y1, sz.y2);

    // Set new size
    GUI.d_w = sz.x2 - sz.x1 - 12;
    GUI.d_h = sz.y2 - sz.y1 - 12;

    // Resize/redraw canvas
    window->DeleteCanvas();
    canvas_w = GUI.d_w;
    canvas_h = GUI.d_h;
    window->CreateCanvas(canvas_w, canvas_h);

    // Reset canvas styling
    auto canvas = window->GetCanvas();
    canvas->SetBG(0xFFFFFF);
    canvas->SetFG(0x0);
    lv_obj_add_flag(canvas->GetFirstBuffer(), LV_OBJ_FLAG_FLOATING);
    lv_obj_align(canvas->GetFirstBuffer(), LV_ALIGN_TOP_LEFT, 0, 46);
    lv_obj_add_style(canvas->GetFirstBuffer(), ThemeManager::GetStyle(StyleAttribute::BorderedContent),
                     LV_STATE_DEFAULT);

    // Render
    Render();

    // CLogger::Get()->Write("File Manager", LogNotice, "Resize %d %d/%d %d", canvas_w, canvas_h);
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
    std::string msg;
    try
    {
        msg = Code.CompileSource(&fs, volume, directory, filename, c, true);
    }
    catch (DARICException &ex)
    {
        std::string cat;
        switch (ex.type)
        {
        case ExceptionType::COMPILER:
            cat = "[Compiler] ";
            break;
        case ExceptionType::TOKENISER:
            cat = "[Tokeniser] ";
            break;
        case ExceptionType::PARSER:
            cat = "[Parser] ";
            break;
        case ExceptionType::RUNTIME:
            cat = "[Runtime] ";
            break;
        }
        msg = cat + ex.error + " in file '" + ex.filename + "' at line " + std::to_string(ex.line_number) +
              ", column " + std::to_string(ex.char_position);
    }
    lv_textarea_set_text(status_text, msg.c_str());
    lv_textarea_set_cursor_pos(status_text, 0);
    UpdateDebugWindow();
}

void Editor::UpdateDebugWindow()
{
    auto breakdown = Breakdown::GetLineBreakdown(this->y + 1);
    if (breakdown == NULL)
        return;

    // Tokens
    std::string tokens;
    for (auto &s : breakdown->tokens)
    {
        tokens += s + "\n";
    }
    lv_textarea_set_text(ta1, tokens.c_str());
    lv_textarea_set_cursor_pos(ta1, 0);

    // Parser
    std::string parser;
    for (auto &s : breakdown->parser)
    {
        parser += s + "\n";
    }
    lv_textarea_set_text(ta2, parser.c_str());
    lv_textarea_set_cursor_pos(ta2, 0);

    // IR
    std::string ir;
    for (auto &s : breakdown->IR)
    {
        ir += s + "\n";
    }
    lv_textarea_set_text(ta3, ir.c_str());
    lv_textarea_set_cursor_pos(ta3, 0);

    // Global
    std::string native;
    if (breakdown->native_global.size() > 0)
        native += "Global:\n";
    size_t previous_addr = 0;
    for (auto &s : breakdown->native_global)
    {
        auto address_s = s.address;
        CLogger::Get()->Write("Native", LogNotice, "A: %p", address_s);
        if (address_s != previous_addr)
        {
            previous_addr = address_s;
            auto ss = Breakdown::GetNativeForAddress(address_s);
            native += *ss + "\n";
        }
    }
    if (breakdown->native.size() > 0)
        native += "DEF:\n";
    for (auto &s : breakdown->native)
    {
        auto address_s = s.address;
        if (address_s != previous_addr)
        {
            previous_addr = address_s;
            auto ss = Breakdown::GetNativeForAddress(address_s);
            native += *ss + "\n";
        }
    }
    lv_textarea_set_text(ta4, native.c_str());
    lv_textarea_set_cursor_pos(ta4, 0);
}

void Editor::Maximise()
{
    maximise_requested = false;
    auto ww = (Window *)this->GUI.GetWindow();
    ww->Maximise(false);
    lv_obj_update_layout(ww->GetLVGLWindow());
    SetupCanvas();
}
