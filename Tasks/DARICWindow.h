#pragma once
#include "../OS/OS.h"
#include "../GUI/Window/Window.h"
#include <circle/logger.h>

class DARICWindow : public OSDTask
{
  public:
    DARICWindow(std::string volume, std::string directory, std::string filename, std::string name, bool fullscreen,
                bool inside_editor, int x, int y, int w, int h, int canvas_w, int canvas_h);
    void Run();
    void LoadSourceCode();
    void SetCode(std::string code_in)
    {
        code = code_in;
    }

  private:
    std::string volume;
    std::string directory;
    std::string filename;
    int canvas_w;
    int canvas_h;
    std::string code;

    void Maximise();
};
