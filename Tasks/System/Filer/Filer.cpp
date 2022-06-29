#include <circle/logger.h>
#include "Filer.h"
#include "../FileManager/FileManager.h"
#include "../IconBar/IconBar.h"
#include "../FontManager/FontManager.h"
#include "../../GUI/Window/LVGLWindow.h"
#include "../Library/StringLib.h"
#include "../../Editor/Editor.h"

int Filer::cx = 128;
int Filer::cy = 128;

Filer::Filer(std::string volume, std::string directory) : volume(volume), directory(directory)
{
    this->id = "Filer" + std::to_string(task_id++);
    this->name = volume + directory;
    this->priority = TaskPriority::Low;
    //	CLogger::Get()->Write("File Manager", LogNotice, "%s", name.c_str());
    this->d_x = cx;
    this->d_y = cy;
    cx += 100;
    cy += 100;
    this->d_w = 768;
    this->d_h = 386;
    this->type = TaskType::Filer;
    if (cx > 960)
    {
        cx = 128;
        cy = 128;
    }
};

Filer::~Filer()
{
}

void Filer::Run()
{
    fs.SetVolume(volume);
    fs.SetCurrentDirectory(directory);
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

    BuildContent();
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
                default:
                    CLogger::Get()->Write("Editor", LogNotice, "Key: %d %x", k.ascii, k.keycode);
                }
            }
            else
            {
                CLogger::Get()->Write("Editor", LogNotice, "%d", k.ascii);
            }
        }
    }
}

void Filer::BuildContent()
{
    switch (view)
    {
    case FilerView::Icons:
        BuildIcons();
        break;
    }
}

void Filer::Refresh()
{
    BuildContent();
}

void Filer::BuildIcons()
{
    auto w = ((Window *)this->GetWindow())->GetLVGLWindow();
    auto content = lv_mywin_get_content(w);
    lv_obj_clean(content);

    auto style_grid = ThemeManager::GetStyle(StyleAttribute::Grid);

    filer_cont = lv_obj_create(content);
    lv_obj_set_size(filer_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_center(filer_cont);
    lv_obj_align(filer_cont, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_flex_align(filer_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_flex_flow(filer_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_add_style(filer_cont, style_grid, LV_STATE_DEFAULT);
    lv_obj_clear_flag(filer_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(filer_cont, WindowPressEventHandler, LV_EVENT_LONG_PRESSED, this);
    lv_obj_add_event_cb(content, WindowPressEventHandler, LV_EVENT_LONG_PRESSED, this);

    // Keys
    lv_obj_add_event_cb(lv_scr_act(), KeyPressEventHandler, LV_EVENT_KEY, this);
    //	lv_obj_add_event_cb(w, KeyPressEventHandler, LV_EVENT_KEY, this);
    //	lv_obj_add_event_cb(content, KeyPressEventHandler, LV_EVENT_KEY, this);
    //	lv_obj_add_event_cb(filer_cont, KeyPressEventHandler, LV_EVENT_KEY, this);

    auto l = volume.length() + directory.length();

    // Directories
    auto dirs = fs.ListAllDirectoriesInCurrentDirectory(false, false);
    for (auto &d : dirs)
    {
        AddIcon(d.substr(l), true);
    }

    // Files
    auto files = fs.ListAllFilesInCurrentDirectory(false);
    for (auto &d : files)
    {
        AddIcon(d.substr(l), false);
    }
}

void Filer::AddIcon(std::string name, bool is_directory)
{
    auto style_iconbar_button = ThemeManager::GetStyle(StyleAttribute::IconBarButton);
    auto style_iconbar_inner = ThemeManager::GetStyle(StyleAttribute::IconBarInner);

    FileIcon i;
    i.filename = name;
    i.is_directory = is_directory;
    i.directory = directory;
    i.volume = volume;
    lv_img_dsc_t *icon = NULL;

    // Do we have an extension? And therefore a type?
    auto fd = name.find_first_of(".");
    if (fd != std::string::npos)
    {
        auto extension = name.substr(fd + 1);
        toupper(extension);
        i.type = WindowManager::GetFileType(extension);
        if (i.type != NULL)
        {
            icon = WindowManager::GetIcon(i.type->icon);
        }
        //		CLogger::Get()->Write("File Manager", LogNotice, "%s", extension.c_str());
    }

    icons.push_back(std::move(i));
    auto ip = &icons.back();

    // Icon
    if (icon == NULL)
    {
        if (is_directory)
        {
            // We can override the default icon here
            FILINFO fno;
            auto name_to_check = (fs.GetCurrentDirectory() + name + "/Icon.png");
            auto fr = f_stat(name_to_check.c_str(), &fno);
            if (fr == FR_OK)
            {
                // Do we have it?
                icon = WindowManager::GetIcon(name_to_check);
                if (icon == NULL)
                {
                    fs.SetCurrentDirectory(directory + "/" + name);
                    WindowManager::LoadIcon(name_to_check, name_to_check);
                    icon = WindowManager::GetIcon(name_to_check);
                    fs.SetCurrentDirectory(directory);
                }
            }
            else
            {
                icon = WindowManager::GetIcon("Folder");
            }
        }
        else
        {
            // Later we need to look up the app types
            icon = WindowManager::GetIcon("Sloth");
        }
    }

    lv_obj_t *device_cont = lv_obj_create(filer_cont);
    lv_obj_set_size(device_cont, cell_size, cell_size);
    lv_obj_center(device_cont);
    lv_obj_set_flex_align(device_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(device_cont, style_iconbar_inner, LV_STATE_DEFAULT);
    lv_obj_clear_flag(device_cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btn = lv_btn_create(device_cont);
    lv_obj_center(btn);
    lv_obj_set_size(btn, 64, 64);
    lv_obj_add_event_cb(btn, IconClickEventHandler, LV_EVENT_SHORT_CLICKED, ip);
    lv_obj_add_event_cb(btn, IconPressEventHandler, LV_EVENT_LONG_PRESSED, ip);
    lv_obj_t *img = lv_img_create(btn);
    lv_obj_center(img);
    lv_img_set_src(img, icon);
    lv_obj_add_style(btn, style_iconbar_button, LV_STATE_DEFAULT);

    auto nam = lv_label_create(device_cont);
    lv_label_set_text(nam, name.c_str());

    // Wide?
    auto width = FontManager::GetWidth(ThemeManager::GetFont(FontAttribute::Body), name.c_str());
    if (width >= cell_size)
    {
        lv_obj_set_size(nam, cell_size, ThemeManager::GetConst(ConstAttribute::BodyFontSize));
        lv_label_set_long_mode(nam, LV_LABEL_LONG_DOT);
    }
    lv_obj_center(nam);

    // Add to list for later
    items.push_back(device_cont);
}

void Filer::IconClickEventHandler(lv_event_t *e)
{
    auto icon_clicked = (FileIcon *)e->user_data;
    if (icon_clicked->is_directory)
    {
        auto task = new Filer(icon_clicked->volume, icon_clicked->directory + "/" + icon_clicked->filename);
        task->Start();
    }
    else
    {
        auto app = new DARICWindow(icon_clicked->volume, icon_clicked->directory, icon_clicked->filename,
                                   icon_clicked->filename, false, false, cx, cy, 640, 512, 1920, 1080);
        app->LoadSourceCode();
        app->Start();
        cx += 100;
        cy += 100;
    }
    if (cx > 960)
    {
        cx = 128;
        cy = 128;
    }
}

void Filer::IconPressEventHandler(lv_event_t *e)
{
    auto t = (FileIcon *)e->user_data;

    lv_point_t p;
    lv_indev_t *indev = lv_indev_get_act();
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    if (indev_type == LV_INDEV_TYPE_POINTER)
    {
        lv_indev_get_point(indev, &p);

        // Create menu window
        MenuDefinition menu;

        {
            MenuItem mi;
            mi.type = MenuItemType::SubMenu;
            mi.v = "Rename";
            menu.items.push_back(std::move(mi));
        }
        {
            MenuItem mi;
            mi.type = MenuItemType::Item;
            mi.v = "Delete";
            mi.shortcut = "Del";
            menu.items.push_back(std::move(mi));
        }
        {
            MenuItem mi;
            mi.type = MenuItemType::Item;
            mi.v = "Info";
            menu.items.push_back(std::move(mi));
        }

        // Is it a DARIC file?
        if (t->type->extension == "DARIC")
        {
            {
                MenuItem mi;
                mi.type = MenuItemType::Separator;
                menu.items.push_back(std::move(mi));
            }
            {
                MenuItem mi;
                mi.type = MenuItemType::Item;
                mi.v = "Edit";
                mi.cb = &Filer::EditEventHandler;
                mi.user_data = e->user_data;
                menu.items.push_back(std::move(mi));
            }
            {
                MenuItem mi;
                mi.type = MenuItemType::Item;
                mi.v = "Run windowed";
                mi.cb = &Filer::RunEventHandler;
                mi.user_data = e->user_data;
                menu.items.push_back(std::move(mi));
            }
            {
                MenuItem mi;
                mi.type = MenuItemType::Item;
                mi.v = "Run full screen";
                mi.cb = &Filer::RunFullscreenEventHandler;
                mi.user_data = e->user_data;
                menu.items.push_back(std::move(mi));
            }
        }

        Menu::OpenMenu(p.x, p.y, NULL, "Filer", std::move(menu));
    }
}

void Filer::WindowPressEventHandler(lv_event_t *e)
{
    auto t = (Filer *)e->user_data;
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
            mi.v = "Grid view";
            mi.shortcut = "^G";
            menu.items.push_back(std::move(mi));
        }
        {
            MenuItem mi;
            mi.type = MenuItemType::Item;
            mi.v = "List view";
            mi.shortcut = "^L";
            menu.items.push_back(std::move(mi));
        }
        {
            MenuItem mi;
            mi.type = MenuItemType::Item;
            mi.v = "Detail view";
            mi.shortcut = "^D";
            menu.items.push_back(std::move(mi));
        }
        {
            MenuItem mi;
            mi.type = MenuItemType::Separator;
            menu.items.push_back(std::move(mi));
        }
        {
            MenuItem mi;
            mi.type = MenuItemType::Item;
            mi.v = "Select all";
            mi.shortcut = "^A";
            menu.items.push_back(std::move(mi));
        }
        {
            MenuItem mi;
            mi.type = MenuItemType::Item;
            mi.v = "Refresh";
            mi.shortcut = "^R";
            mi.cb = &Filer::RefreshEventHandler;
            mi.user_data = e->user_data;
            menu.items.push_back(std::move(mi));
        }
        Menu::OpenMenu(p.x, p.y, NULL, "Filer", std::move(menu));
    }
}

void Filer::KeyPressEventHandler(lv_event_t *e)
{
    auto t = (Filer *)e->user_data;
    uint32_t key = lv_event_get_key(e);
    switch (key)
    {
    case 7: // Ctrl-G
        break;
    case 12: // Ctrl-L
        break;
    case 4: // Ctrl-D
        break;
    case KEY_Right:
        break;
    }
    CLogger::Get()->Write("File Manager", LogNotice, "%d %s", key, t->id.c_str());
}

void Filer::RefreshEventHandler(lv_event_t *e)
{
    Menu::CloseMenu();
    auto t = (Filer *)e->user_data;
    t->Refresh();
}

void Filer::RunEventHandler(lv_event_t *e)
{
    Menu::CloseMenu();
    auto t = (FileIcon *)e->user_data;
    auto app =
        new DARICWindow(t->volume, t->directory, t->filename, "DARIC", false, false, 100, 100, 640, 512, 1920, 1080);
    app->LoadSourceCode();
    app->Start();
}

void Filer::RunFullscreenEventHandler(lv_event_t *e)
{
    Menu::CloseMenu();
    auto t = (FileIcon *)e->user_data;
    auto app =
        new DARICWindow(t->volume, t->directory, t->filename, "DARIC", true, false, 100, 100, 640, 512, 1920, 1080);
    app->LoadSourceCode();
    app->Start();
}

void Filer::EditEventHandler(lv_event_t *e)
{
    Menu::CloseMenu();
    auto t = (FileIcon *)e->user_data;
    auto editor = new Editor(100, 100, 1400, 600);
    editor->LoadSourceCode(t->volume, t->directory, t->filename);
    editor->Start();
}

void Filer::Maximise()
{
    maximise_requested = false;
    auto ww = (Window *)this->GetWindow();
    ww->Maximise(false);
}
