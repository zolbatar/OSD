#pragma once
#include "../OS/OS.h"
#include "System/WindowManager/window/Window.h"
#include <circle/logger.h>

class DARICWindow : public OSDTask
{
  public:
    DARICWindow(void *editor, std::string volume, std::string directory, std::string filename, std::string name,
                bool fullscreen, bool debug, bool inside_editor, int x, int y, int w, int h, int canvas_w,
                int canvas_h);
    void Run();
    void LoadSourceCode();
    void SetCode(std::string code_in)
    {
        code = code_in;
    }
    void RequestMaximise();
    void RequestMinimise();

  private:
    bool debug = false;
    bool inside_editor = false;
    void *editor = NULL;
    std::string volume;
    std::string directory;
    std::string filename;
    int canvas_w;
    int canvas_h;
    std::string code;

    void Maximise();
};
