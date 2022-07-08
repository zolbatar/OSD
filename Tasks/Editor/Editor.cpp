#include "Editor.h"
#include <Breakdown/Breakdown.h>
#include <IconBar/IconBar.h>
#include <Menu/Menu.h>

Editor::Editor(int x, int y, int w, int h)
{
    this->GUI.d_x = x;
    this->GUI.d_y = y;
    this->GUI.d_w = w;
    this->GUI.d_h = h;
    this->id = "@" + std::to_string(task_id++);
    this->SetName("Editor");
    this->type = TaskType::Editor;
}

Editor::~Editor()
{
    if (edit != NULL)
        delete edit;
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
    edit = new TextEdit(this, grid);
    edit->SetText(loaded_code);
    lv_obj_set_grid_cell(edit->GetObjectParent(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

    /*    obj_parent = lv_obj_create(grid);
        lv_obj_set_grid_cell(obj_parent, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
        lv_obj_add_style(obj_parent, ThemeManager::GetStyle(StyleAttribute::Scrollbar), LV_PART_SCROLLBAR);
        lv_obj_add_style(obj_parent, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);
        lv_obj_add_event_cb(obj_parent, ScrollEventHandler, LV_EVENT_SCROLL, this);

        // Canvas child
        obj = lv_obj_create(obj_parent);
        lv_obj_add_style(obj, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);*/

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

    Yield();
    lv_obj_add_event_cb(content, ResizeEventHandler, LV_EVENT_SIZE_CHANGED, this);

    // Do stuff
    edit->Resized();
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
                case KeyF1: {
                    RunWindowed();
                    break;
                }
                case KeyF2: {
                    FullscreenRun();
                    break;
                }
                default:
                    // Pass to edit
                    edit->HandleKey(k);
                }
            }
            else
            {
                edit->HandleKey(k);
            }
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
    loaded_code = buffer;
    free(buffer);

    ClearOverride();
}

void Editor::ResizeEventHandler(lv_event_t *e)
{
    auto editor = (Editor *)e->user_data;
    editor->edit->Resized();
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
    app->SetCode(edit->GetText());
    app->Start();
}

void Editor::FullscreenRun()
{
    auto app = new DARICWindow(volume, directory, filename, "DARIC", true, true, 100, 100, 640, 512, 1920, 1080);
    app->SetCode(edit->GetText());
    app->Start();
}

void Editor::Debug()
{
    auto c = edit->GetText();
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
    auto breakdown = Breakdown::GetLineBreakdown(edit->y + 1);
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
    edit->Resized();
}
