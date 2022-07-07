#include <memory.h>
#include <string.h>
#include <fstream>
#include "DARICWindow.h"
#include "../Compiler/Exception/DARICException.h"
#include "System/WindowManager/lvglwindow/LVGLWindow.h"

DARICWindow::DARICWindow(std::string volume, std::string directory, std::string filename, std::string name,
                         bool fullscreen, bool inside_editor, int x, int y, int w, int h, int canvas_w, int canvas_h)
{
    this->volume = volume;
    this->filename = filename;
    this->directory = directory;
    this->GUI.fullscreen = fullscreen;
    this->inside_editor = inside_editor;
    this->GUI.d_x = x;
    this->GUI.d_y = y;
    this->GUI.d_w = w;
    this->GUI.d_h = h;
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
    this->SetName(name.c_str());
    this->id = "@" + std::to_string(task_id++);
    type = TaskType::DARIC;
}

void DARICWindow::LoadSourceCode()
{
    SetOverride(this);
    this->code = Code.LoadSource(&fs, volume, directory, filename);
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
    strcpy(m.title, GetName());
    m.x = GUI.d_x;
    m.y = GUI.d_y;
    m.width = GUI.d_w;
    m.height = GUI.d_h;
    m.canvas = true;
    m.canvas_w = canvas_w;
    m.canvas_h = canvas_h;
    m.fixed = true;
    CallGUIDirectEx(&mess);

    auto ww = (Window *)this->GUI.GetWindow();
    auto win = ww->GetLVGLWindow();
    auto content = lv_mywin_get_content(win);
    lv_obj_set_style_bg_color(content, lv_color_black(), LV_STATE_DEFAULT);

    // Compile (and run)
    try
    {
        Code.CompileSource(&fs, volume, directory, filename, code, inside_editor);
        if (GUI.fullscreen)
        {
            ww->Maximise(true);
        }
        // CLogger::Get()->Write("DARICWindow", LogDebug, "Run: %s", GetWindowName().c_str());
        Code.RunCode(inside_editor);
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
    auto ww = (Window *)this->GUI.GetWindow();
    ww->Maximise(true);
}
