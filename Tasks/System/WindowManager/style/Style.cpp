#include "Style.h"
#include <FontManager/FontManager.h>
#include <WindowManager/WindowManager.h>

uint32_t OSD_EVENT_MOVED;

extern int ScreenResX;
extern int ScreenResY;
std::map<ColourAttribute, lv_color_t> ThemeManager::theme_colours;
std::map<StyleAttribute, lv_style_t *> ThemeManager::theme_styles;
std::map<FontAttribute, lv_font_t *> ThemeManager::theme_fonts;
std::map<ConstAttribute, uint32_t> ThemeManager::theme_consts;

lv_color_t ThemeManager::GetColour(ColourAttribute ca)
{
    return theme_colours.find(ca)->second;
}

lv_style_t *ThemeManager::GetStyle(StyleAttribute ca)
{
    return theme_styles.find(ca)->second;
}

lv_font_t *ThemeManager::GetFont(FontAttribute ca)
{
    return theme_fonts.find(ca)->second;
}

uint32_t ThemeManager::GetConst(ConstAttribute ca)
{
    return theme_consts.find(ca)->second;
}

void ThemeManager::AddConst(ConstAttribute ca, uint32_t v)
{
    theme_consts.insert(std::make_pair(ca, v));
}

void ThemeManager::AddColour(ColourAttribute ca, lv_color_t v)
{
    theme_colours.insert(std::make_pair(ca, v));
}

void ThemeManager::AddStyle(StyleAttribute sa, lv_style_t *v)
{
    theme_styles.insert(std::make_pair(sa, v));
}

void ThemeManager::AddFont(FontAttribute fa, lv_font_t *v)
{
    theme_fonts.insert(std::make_pair(fa, v));
}

lv_style_t *WindowManager::CreateStyle()
{
    return (lv_style_t *)malloc(sizeof(lv_style_t));
}

void WindowManager::SetupLVGLStyles()
{
    // Custom events
    OSD_EVENT_MOVED = lv_event_register_id();

    CLogger::Get()->Write("Window Manager", LogDebug, "Setting up styles");

    // Constants
    ThemeManager::AddConst(ConstAttribute::WindowFontSize, 20);
    ThemeManager::AddConst(ConstAttribute::BodyFontSize, 20);
    ThemeManager::AddConst(ConstAttribute::MonoFontSize, 20);
    ThemeManager::AddConst(ConstAttribute::FurnitureFontSize, 16);
    ThemeManager::AddConst(ConstAttribute::HeaderFontSize, 24);
    ThemeManager::AddConst(ConstAttribute::ButtonFontSize, 24);
    ThemeManager::AddConst(ConstAttribute::IconBarHeight, 110);
    ThemeManager::AddConst(ConstAttribute::WindowBorderWidth, 1);
    ThemeManager::AddConst(ConstAttribute::WindowHeaderHeight, 32);
    ThemeManager::AddConst(ConstAttribute::MenuHeaderHeight, 28);
    ThemeManager::AddConst(ConstAttribute::WindowFurnitureWidth, 24);
    ThemeManager::AddConst(ConstAttribute::WindowContentPadding, 0);
    ThemeManager::AddConst(ConstAttribute::WindowContentPaddingPadded, 4);
    ThemeManager::AddConst(ConstAttribute::CornerRadius, 5);
    ThemeManager::AddConst(ConstAttribute::ControlPadding, 1);
    ThemeManager::AddConst(ConstAttribute::ContainerPadding, 8);
    ThemeManager::AddConst(ConstAttribute::ScrollbarSize, 8);

    // Colours
    ThemeManager::AddColour(ColourAttribute::WindowBackground, lv_color_hex(0xF0F0F0));
    ThemeManager::AddColour(ColourAttribute::WindowForeground, lv_color_black());
    ThemeManager::AddColour(ColourAttribute::MenuBackground, lv_color_white());
    ThemeManager::AddColour(ColourAttribute::MenuForeground, lv_color_black());
    ThemeManager::AddColour(ColourAttribute::IconBarBackground, lv_color_hex(0xddd6e6));
    ThemeManager::AddColour(ColourAttribute::IconBarForeground, lv_color_black());
    ThemeManager::AddColour(ColourAttribute::Focus, lv_color_hex(0x73a6e6));
    ThemeManager::AddColour(ColourAttribute::WindowHeaderForeground, lv_color_black());
    ThemeManager::AddColour(ColourAttribute::WindowHeaderBackground, lv_color_hex(0xB0B0B0));
    ThemeManager::AddColour(ColourAttribute::WindowHeaderBackgroundActive, lv_color_hex(0xe0e0b0));
    ThemeManager::AddColour(ColourAttribute::MenuHeaderBackgroundActive, lv_color_hex(0x73a6e6));
    ThemeManager::AddColour(ColourAttribute::MenuSeparator, lv_color_hex(0x808080));
    ThemeManager::AddColour(ColourAttribute::ContainerBackgroundColour,
                            ThemeManager::GetColour(ColourAttribute::WindowBackground));
    ThemeManager::AddColour(ColourAttribute::ControlBackgroundColour, lv_color_hex(0xE8E8E8));
    ThemeManager::AddColour(ColourAttribute::ControlBorderColour, lv_color_hex(0x404040));
    ThemeManager::AddColour(ColourAttribute::ControlHighlightColour, lv_color_hex(0xA0A0A0));
    ThemeManager::AddColour(ColourAttribute::WindowFurnitureBackground, lv_color_hex(0xF0F0F0));
    ThemeManager::AddColour(ColourAttribute::WindowFurnitureForeground, lv_color_black());
    ThemeManager::AddColour(ColourAttribute::Buttonbackground, lv_color_hex(0xD0D0D0));
    ThemeManager::AddColour(ColourAttribute::ButtonForeground, lv_color_black());
    ThemeManager::AddColour(ColourAttribute::WindowFurnitureBorder, lv_color_hex(0x808080));
    ThemeManager::AddColour(ColourAttribute::WindowBorder, lv_color_hex(0x606060));
    ThemeManager::AddColour(ColourAttribute::ScrollbarBackground, lv_color_hex(0x404040));

    ThemeManager::AddFont(FontAttribute::Window,
                          FontManager::GetFontByNameStyleAndSize(
                              "Roboto", "Regular", ThemeManager::GetConst(ConstAttribute::WindowFontSize)));
    ThemeManager::AddFont(FontAttribute::WindowBold,
                          FontManager::GetFontByNameStyleAndSize(
                              "Roboto", "Bold", ThemeManager::GetConst(ConstAttribute::WindowFontSize)));
    ThemeManager::AddFont(FontAttribute::Body,
                          FontManager::GetFontByNameStyleAndSize("Roboto", "Regular",
                                                                 ThemeManager::GetConst(ConstAttribute::BodyFontSize)));
    ThemeManager::AddFont(
        FontAttribute::BodyBold,
        FontManager::GetFontByNameStyleAndSize("Roboto", "Bold", ThemeManager::GetConst(ConstAttribute::BodyFontSize)));
    ThemeManager::AddFont(FontAttribute::Header,
                          FontManager::GetFontByNameStyleAndSize(
                              "Roboto", "Bold", ThemeManager::GetConst(ConstAttribute::HeaderFontSize)));
    ThemeManager::AddFont(FontAttribute::Mono,
                          FontManager::GetFontByNameStyleAndSize("Roboto Mono", "Regular",
                                                                 ThemeManager::GetConst(ConstAttribute::MonoFontSize)));
    ThemeManager::AddFont(FontAttribute::Symbol, FontManager::GetFontByNameStyleAndSize(
                                                     "Font Awesome 6 Pro Light", "Light",
                                                     ThemeManager::GetConst(ConstAttribute::FurnitureFontSize)));
    ThemeManager::AddFont(FontAttribute::SymbolLarge, FontManager::GetFontByNameStyleAndSize(
                                                          "Font Awesome 6 Pro Light", "Light",
                                                          ThemeManager::GetConst(ConstAttribute::ButtonFontSize)));

    // Focussed
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::Focus));
        lv_style_set_border_width(style, 0);
        ThemeManager::AddStyle(StyleAttribute::Focussed, style);
    }

    // Transparent
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_bg_opa(style, LV_OPA_TRANSP);
        lv_style_set_border_width(style, 0);
        lv_style_set_pad_all(style, 0);
        ThemeManager::AddStyle(StyleAttribute::TransparentWindow, style);
    }

    // Bold body font
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::BodyBold));
        ThemeManager::AddStyle(StyleAttribute::FontBodyBold, style);
    }

    // Symbol font
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Symbol));
        ThemeManager::AddStyle(StyleAttribute::FontSymbol, style);
    }

    // Menu
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::MenuBackground));
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::WindowBorder));
        lv_style_set_border_width(style, ThemeManager::GetConst(ConstAttribute::WindowBorderWidth));
        lv_style_set_radius(style, ThemeManager::GetConst(ConstAttribute::CornerRadius));
        lv_style_set_pad_all(style, 0);
        lv_style_set_clip_corner(style, true);
        lv_style_set_border_post(style, true);
        ThemeManager::AddStyle(StyleAttribute::Menu, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, 0);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::MenuBackground));
        lv_style_set_text_color(style, ThemeManager::GetColour(ColourAttribute::MenuForeground));
        lv_style_set_border_width(style, 0);
        lv_style_set_pad_all(style, 0);
        ThemeManager::AddStyle(StyleAttribute::MenuContainer, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_pad_all(style, 4);
        lv_style_set_pad_left(style, 8);
        lv_style_set_border_width(style, 0);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Window));
        ThemeManager::AddStyle(StyleAttribute::MenuItem, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_pad_all(style, 0);
        lv_style_set_border_width(style, 0);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::WindowBold));
        ThemeManager::AddStyle(StyleAttribute::MenuItemBold, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_pad_all(style, 0);
        lv_style_set_border_width(style, 1);
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::MenuSeparator));
        ThemeManager::AddStyle(StyleAttribute::MenuSeparator, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::MenuHeaderBackgroundActive));
        ThemeManager::AddStyle(StyleAttribute::MenuHeader, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, 0);
        lv_style_set_border_width(style, 0);
        lv_style_set_bg_color(style, lv_color_black());
        //		lv_style_set_bg_opa(style, LV_OPA_TRANSP);
        lv_style_set_bg_opa(style, LV_OPA_30);
        ThemeManager::AddStyle(StyleAttribute::MenuBackground, style);
    }

    // Icon Bar
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::WindowBorder));
        lv_style_set_border_side(style, LV_BORDER_SIDE_TOP);
        lv_style_set_border_width(style, ThemeManager::GetConst(ConstAttribute::WindowBorderWidth));
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::IconBarBackground));
        lv_style_set_radius(style, 0);
        lv_style_set_pad_all(style, 0);
        lv_style_set_pad_left(style, 8);
        lv_style_set_pad_right(style, 8);
        ThemeManager::AddStyle(StyleAttribute::IconBar, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_text_color(style, ThemeManager::GetColour(ColourAttribute::IconBarForeground));
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::IconBarBackground));
        lv_style_set_border_width(style, 0);
        lv_style_set_radius(style, 0);
        lv_style_set_pad_all(style, 0);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Body));
        ThemeManager::AddStyle(StyleAttribute::IconBarInner, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, 0);
        lv_style_set_shadow_opa(style, LV_OPA_TRANSP);
        lv_style_set_shadow_width(style, 0);
        lv_style_set_border_width(style, 0);
        lv_style_set_bg_opa(style, LV_OPA_TRANSP);
        lv_style_set_pad_all(style, 0);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Header));
        ThemeManager::AddStyle(StyleAttribute::IconBarButton, style);
    }

    // Window
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::WindowBorder));
        lv_style_set_border_width(style, ThemeManager::GetConst(ConstAttribute::WindowBorderWidth));
        lv_style_set_radius(style, ThemeManager::GetConst(ConstAttribute::CornerRadius));
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Window));
        lv_style_set_clip_corner(style, true);
        lv_style_set_border_post(style, true);

        ThemeManager::AddStyle(StyleAttribute::Window, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_border_width(style, 0);
        lv_style_set_radius(style, 0);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::WindowBackground));
        lv_style_set_bg_opa(style, LV_OPA_COVER);
        lv_style_set_pad_all(style, ThemeManager::GetConst(ConstAttribute::WindowContentPadding));
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Body));
        lv_style_set_text_color(style, ThemeManager::GetColour(ColourAttribute::WindowForeground));
        ThemeManager::AddStyle(StyleAttribute::WindowContent, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_border_width(style, 0);
        lv_style_set_radius(style, 0);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::WindowBackground));
        lv_style_set_bg_opa(style, LV_OPA_COVER);
        lv_style_set_pad_all(style, ThemeManager::GetConst(ConstAttribute::WindowContentPaddingPadded));
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Body));
        lv_style_set_text_color(style, ThemeManager::GetColour(ColourAttribute::WindowForeground));
        ThemeManager::AddStyle(StyleAttribute::WindowContentPadded, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, 0);
        lv_style_set_text_color(style, ThemeManager::GetColour(ColourAttribute::WindowHeaderForeground));
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::WindowBorder));
        lv_style_set_border_side(style, LV_BORDER_SIDE_BOTTOM);
        lv_style_set_border_width(style, ThemeManager::GetConst(ConstAttribute::WindowBorderWidth));
        lv_style_set_pad_all(style, 4);
        ThemeManager::AddStyle(StyleAttribute::WindowHeader, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::WindowHeaderBackgroundActive));
        ThemeManager::AddStyle(StyleAttribute::WindowActive, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::WindowHeaderBackground));
        ThemeManager::AddStyle(StyleAttribute::WindowInactive, style);
    }

    // Window full screen
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_border_width(style, 0);
        lv_style_set_radius(style, ThemeManager::GetConst(ConstAttribute::CornerRadius));
        ThemeManager::AddStyle(StyleAttribute::WindowFullScreen, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_height(style, 0);
        lv_style_set_radius(style, 0);
        lv_style_set_border_width(style, 0);
        ThemeManager::AddStyle(StyleAttribute::WindowHeaderFullScreen, style);
    }

    // Window buttons
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, ThemeManager::GetConst(ConstAttribute::CornerRadius));
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::WindowFurnitureBorder));
        lv_style_set_border_width(style, ThemeManager::GetConst(ConstAttribute::WindowBorderWidth));
        lv_style_set_border_opa(style, LV_OPA_COVER);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::Buttonbackground));
        lv_style_set_bg_opa(style, LV_OPA_COVER);
        lv_style_set_text_color(style, ThemeManager::GetColour(ColourAttribute::ButtonForeground));
        lv_style_set_pad_all(style, 2);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::SymbolLarge));
        ThemeManager::AddStyle(StyleAttribute::Button, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, ThemeManager::GetConst(ConstAttribute::CornerRadius));
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::WindowFurnitureBorder));
        lv_style_set_border_width(style, ThemeManager::GetConst(ConstAttribute::WindowBorderWidth));
        lv_style_set_border_opa(style, LV_OPA_COVER);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::WindowFurnitureBackground));
        lv_style_set_bg_opa(style, LV_OPA_50);
        lv_style_set_text_color(style, ThemeManager::GetColour(ColourAttribute::WindowFurnitureForeground));
        lv_style_set_pad_all(style, 2);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Symbol));
        ThemeManager::AddStyle(StyleAttribute::WindowButton, style);
    }

    // Scrollbar
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, ThemeManager::GetConst(ConstAttribute::CornerRadius));
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::ControlBorderColour));
        lv_style_set_border_opa(style, LV_OPA_30);
        lv_style_set_border_width(style, 1);
        lv_style_set_width(style, ThemeManager::GetConst(ConstAttribute::ScrollbarSize));
        lv_style_set_height(style, ThemeManager::GetConst(ConstAttribute::ScrollbarSize));
        lv_style_set_bg_opa(style, LV_OPA_30);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::ScrollbarBackground));
        ThemeManager::AddStyle(StyleAttribute::Scrollbar, style);
    }

    // Bar
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, 0);
        lv_style_set_border_color(style, ThemeManager::GetColour(ColourAttribute::ControlBorderColour));
        lv_style_set_border_width(style, 1);
        lv_style_set_pad_all(style, ThemeManager::GetConst(ConstAttribute::ControlPadding));
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::ControlBackgroundColour));
        ThemeManager::AddStyle(StyleAttribute::Bar, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, 0);
        lv_style_set_border_width(style, 0);
        lv_style_set_bg_color(style, lv_color_mix(ThemeManager::GetColour(ColourAttribute::ControlHighlightColour),
                                                  lv_color_hex(0x00FF000), 192));
        ThemeManager::AddStyle(StyleAttribute::BarGreen, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_radius(style, 0);
        lv_style_set_border_width(style, 0);
        lv_style_set_bg_color(style, lv_color_mix(ThemeManager::GetColour(ColourAttribute::ControlHighlightColour),
                                                  lv_color_hex(0xFF00000), 192));
        ThemeManager::AddStyle(StyleAttribute::BarRed, style);
    }

    // Grid
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_bg_color(style, ThemeManager::GetColour(ColourAttribute::ContainerBackgroundColour));
        lv_style_set_border_width(style, 0);
        lv_style_set_pad_all(style, ThemeManager::GetConst(ConstAttribute::ContainerPadding));
        lv_style_set_radius(style, ThemeManager::GetConst(ConstAttribute::CornerRadius));
        ThemeManager::AddStyle(StyleAttribute::Grid, style);
    }
    {
        lv_style_t *style = CreateStyle();
        lv_style_init(style);
        lv_style_set_text_font(style, ThemeManager::GetFont(FontAttribute::Header));
        ThemeManager::AddStyle(StyleAttribute::GridTitle, style);
    }

    // Wallpaper
    fs.SetCurrentDirectory(":Boot/Wallpaper");
    auto img = LoadPNG(fs.GetCurrentDirectory() + "Daric Wallpaper.png", 1920, 1080);
    static lv_style_t style_background;
    lv_style_init(&style_background);
    lv_style_set_bg_img_src(&style_background, img);
    lv_obj_add_style(lv_scr_act(), &style_background, LV_STATE_DEFAULT);

    // Disable scrolling
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

    // Icons
    fs.SetCurrentDirectory(":Boot/Config");
    CLogger::Get()->Write("Window Manager", LogNotice, "Loading icon definitions");
    FIL fil;
    if (f_open(&fil, (fs.GetCurrentDirectory() + "Icons").c_str(), FA_READ | FA_OPEN_EXISTING) != FR_OK)
    {
        CLogger::Get()->Write("Window Manager", LogPanic, "Error opening icon definition file");
    }
    size_t sz = f_size(&fil);
    char *buffer = (char *)malloc(sz + 1);
    if (!buffer)
    {
        CLogger::Get()->Write("Window Manager", LogPanic, "Error allocating memory for icon definition file");
    }
    uint32_t l;
    if (f_read(&fil, buffer, sz, &l) != FR_OK)
    {
        CLogger::Get()->Write("Window Manager", LogPanic, "Error loading icon definition file");
    }
    buffer[sz] = 0;

    // Parse JSON
    nlohmann::json j = nlohmann::json::parse(buffer);

    // Loop through and create all font objects
    fs.SetCurrentDirectory(":Boot/Icons");
    for (auto &icon : j["Icons"])
    {
        std::string name = icon["name"];
        std::string filename = icon["filename"];
        LoadIcon(fs.GetCurrentDirectory() + filename, name);
    }

    // Cursors
    fs.SetCurrentDirectory(":Boot/Cursors");
    mouse_cursor = LoadPNG(fs.GetCurrentDirectory() + "Arrow.png", 32, 32);
}

void WindowManager::LoadIcon(std::string filename, std::string name)
{
    Icon i;
    i.filename = filename;
    i.image = NULL;
    icons.insert(std::make_pair(name, std::move(i)));
}

lv_img_dsc_t *WindowManager::LoadPNG(std::string filename, int w, int h)
{
    FIL fil;
    if (f_open(&fil, (filename).c_str(), FA_READ | FA_OPEN_EXISTING) != FR_OK)
    {
        CLogger::Get()->Write("Window Manager", LogPanic, "Error opening image file '%s'", filename.c_str());
    }
    size_t sz = f_size(&fil);
    char *buffer = (char *)malloc(sz);
    if (!buffer)
    {
        CLogger::Get()->Write("Window Manager", LogPanic, "Error allocating memory for image file '%s'",
                              filename.c_str());
    }
    uint32_t l;
    if (f_read(&fil, buffer, sz, &l) != FR_OK)
    {
        CLogger::Get()->Write("Window Manager", LogPanic, "Error loading image file '%s'", filename.c_str());
    }
    f_close(&fil);

    // Convert to PNG
    auto img = (lv_img_dsc_t *)calloc(1, sizeof(lv_img_dsc_t));
    img->data = (const uint8_t *)buffer;
    img->header.w = w;
    img->header.h = h;
    img->header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    img->data_size = l;
    return img;
}