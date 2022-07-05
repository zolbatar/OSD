#include <memory.h>
#include <string.h>
#include <fstream>
#include "DARICWindow.h"
#include "../Exception/DARICException.h"
#include "../GUI/Window/LVGLWindow.h"

DARICWindow::DARICWindow(std::string volume, std::string directory, std::string filename, std::string name,
                         bool fullscreen, bool inside_editor, int x, int y, int w, int h, int canvas_w, int canvas_h)
{
    this->volume = volume;
    this->filename = filename;
    this->directory = directory;
    this->fullscreen = fullscreen;
    this->inside_editor = inside_editor;
    this->d_x = x;
    this->d_y = y;
    this->d_w = w;
    this->d_h = h;
    auto window_border_width = ThemeManager::GetConst(ConstAttribute::WindowBorderWidth);
    auto window_content_padding = ThemeManager::GetConst(ConstAttribute::WindowContentPadding);
    auto window_header_height = ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight);
    if (canvas_w != 0)
        this->canvas_w = canvas_w;
    else
        this->canvas_w = w - window_border_width * 2 - window_content_padding * 2;
    if (canvas_h != 0)
        this->canvas_h = canvas_h;
    else
        this->canvas_h = h - window_border_width * 2 - window_header_height - window_content_padding * 2;
    this->name = name;
    this->id = "@" + std::to_string(task_id++);
    type = TaskType::DARIC;
}

void DARICWindow::LoadSourceCode()
{
    SetOverride(this);
    this->code = this->LoadSource(volume, directory, filename);
    ClearOverride();
}

void DARICWindow::Run()
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
    m.canvas = true;
    m.canvas_w = canvas_w;
    m.canvas_h = canvas_h;
    m.fixed = true;
    CallGUIDirectEx(&mess);

    auto ww = (Window *)this->GetWindow();
    auto win = ww->GetLVGLWindow();
    auto content = lv_mywin_get_content(win);
    lv_obj_set_style_bg_color(content, lv_color_black(), LV_STATE_DEFAULT);

    // Compile (and run)
    try
    {
        if (CompileSource(volume, directory, filename, code, inside_editor))
        {
            if (fullscreen)
            {
                ww->Maximise(true);
            }
            // CLogger::Get()->Write("DARICWindow", LogDebug, "Run: %s", GetWindowName().c_str());
            RunCode(inside_editor);
        }
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
        CLogger::Get()->Write("CompileSource", LogPanic, "%s%s in file '%s' at line %d, column %d", cat.c_str(),
                              ex.error.c_str(), ex.filename.c_str(), ex.line_number, ex.char_position);
    }
    TerminateTask();
}

void DARICWindow::Maximise()
{
    maximise_requested = false;
    auto ww = (Window *)this->GetWindow();
    ww->Maximise(true);
}
