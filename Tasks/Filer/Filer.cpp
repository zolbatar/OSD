#include "Filer.h"
#include "../FileManager/FileManager.h"
#include "../../GUI/Window/LVGLWindow.h"

int Filer::cx = 628;
int Filer::cy = 512;
Icon* Filer::icon_clicked;
unsigned Filer::last_click;

Filer::Filer(std::string volume, std::string directory)
		:volume(volume), directory(directory)
{
	this->id = "Filer"+std::to_string(task_id++);
	this->name = volume+"."+directory;
	this->d_x = cx;
	this->d_y = cy;
	cx += 100;
	cy += 100;
	this->d_w = 512;
	this->d_h = 224;
};

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

	// Now build content
	auto w = ((Window*)
			this->GetWindow())->GetLVGLWindow();
	auto content = lv_mywin_get_content(w);
	filer_cont = lv_obj_create(content);
	lv_obj_set_size(filer_cont, LV_PCT(100), LV_SIZE_CONTENT);
	lv_obj_center(filer_cont);
	lv_obj_align(filer_cont, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_flex_align(filer_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_flex_flow(filer_cont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_add_style(filer_cont, &style_grid, LV_STATE_DEFAULT);
	lv_obj_clear_flag(filer_cont, LV_OBJ_FLAG_SCROLLABLE);

	auto l = volume.length()+directory.length()-1;

	// Directories
	auto dirs = fs.ListAllDirectoriesInCurrentDirectory(false, false);
	for (auto& d: dirs) {
		AddIcon(d.substr(l), true);
	}

	// Files
	auto files = fs.ListAllFilesInCurrentDirectory(false);
	for (auto& d: files) {
		AddIcon(d.substr(l), false);
	}

	while (1) {
		Yield();
	}
}

void Filer::AddIcon(std::string name, bool is_directory)
{
	Icon i;
	i.name = name;
	i.is_directory = is_directory;
	i.current_directory = directory;
	i.volume = volume;
	icons.push_back(std::move(i));
	auto ip = &icons.back();

	// Icon
	lv_img_dsc_t* icon = NULL;
	if (is_directory) {
		// We can override the default icon here

		FILINFO fno;
		auto name_to_check = (fs.GetCurrentDirectory()+name+"/"+name+".png");
//		CLogger::Get()->Write("Filer", LogDebug, "%s", name_to_check.c_str());
		auto fr = f_stat(name_to_check.c_str(), &fno);
		if (fr==FR_OK) {
			// Do we have it?
			icon = WindowManager::GetIcon(name_to_check);
			if (icon==NULL) {
				fs.SetCurrentDirectory(directory+"."+name);
				icon = WindowManager::LoadIcon(name_to_check, name_to_check);
				fs.SetCurrentDirectory(directory);
			}
		}
		else {
			icon = WindowManager::GetIcon("Folder");
		}
	}
	else {
		// Later we need to look up the app types
		icon = WindowManager::GetIcon("Sloth");
	}

	lv_obj_t* device_cont = lv_obj_create(filer_cont);
	lv_obj_set_size(device_cont, cell_size, cell_size);
	lv_obj_center(device_cont);
	lv_obj_set_flex_align(device_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(device_cont, &style_iconbar_inner, LV_STATE_DEFAULT);
	lv_obj_clear_flag(device_cont, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t* btn = lv_btn_create(device_cont);
	lv_obj_center(btn);
	lv_obj_set_size(btn, 64, 64);
	lv_obj_add_event_cb(btn, IconClickEventHandler, LV_EVENT_SHORT_CLICKED, ip);
	lv_obj_t* img = lv_img_create(btn);
	lv_obj_center(img);
	lv_img_set_src(img, icon);
	lv_obj_add_style(btn, &style_iconbar_button, LV_STATE_DEFAULT);

	auto nam = lv_label_create(device_cont);
	lv_obj_center(nam);
	lv_label_set_text(nam, name.c_str());
}

void Filer::IconClickEventHandler(lv_event_t* e)
{
	auto this_icon_clicked = (Icon*)e->user_data;
	if (icon_clicked!=NULL && icon_clicked->name==this_icon_clicked->name) {
		auto ticks = CTimer::Get()->GetClockTicks();
		auto diff = ticks-last_click;
		if (diff<DOUBLE_CLICK_SPEED) {
			auto task = new Filer(icon_clicked->volume, icon_clicked->current_directory+"."+icon_clicked->name);
			task->Start();
			icon_clicked = NULL;
			return;
		}
	}
	last_click = CTimer::Get()->GetClockTicks();
	icon_clicked = this_icon_clicked;
}