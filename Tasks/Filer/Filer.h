#pragma once
#include <list>
#include "../../OS/OS.h"
#include "../WindowManager/WindowManager.h"
#include "../Menu/Menu.h"

enum class FilerView
{
    Icons,
    List,
    Detail
};

struct FileIcon
{
    std::string volume;
    std::string directory;
    std::string filename;
    bool is_directory;
    FileType *type;
};

class Filer : public OSDTask
{
  public:
    Filer(std::string volume, std::string directory);
    ~Filer();
    void Run();
    void Refresh();

  private:
    FilerView view = FilerView::Icons;
    const int cell_size = 128;
    std::string volume;
    std::string directory;
    static int cx;
    static int cy;
    lv_obj_t *filer_cont = NULL;
    std::list<FileIcon> icons;
    std::vector<lv_obj_t *> items;

    void Maximise();
    void BuildContent();
    void BuildIcons();
    void AddIcon(std::string name, bool is_directory);
    static void WindowPressEventHandler(lv_event_t *e);
    static void IconClickEventHandler(lv_event_t *e);
    static void IconPressEventHandler(lv_event_t *e);
    static void KeyPressEventHandler(lv_event_t *e);
    static void RefreshEventHandler(lv_event_t *e);
    static void RunEventHandler(lv_event_t *e);
    static void RunFullscreenEventHandler(lv_event_t *e);
    static void EditEventHandler(lv_event_t *e);
};
