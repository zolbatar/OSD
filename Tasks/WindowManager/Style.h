#pragma once
#include "../../GUI/lvgl.h"
#include <map>

enum class ColourAttribute {
	ContainerBackgroundColour,
	ControlBackgroundColour,
	ControlBorderColour,
	ControlHighlightColour,
	DesktopBackground,
	DesktopForeground,
	Focus,
	ScrollbarBackground,
	WindowBackground,
	WindowForeground,
	WindowHeaderForeground,
	WindowHeaderBackground,
	WindowHeaderBackgroundActive,
	MenuHeaderBackgroundActive,
	MenuBackground,
	MenuSeparator,
	MenuForeground,
	WindowBorder,
	WindowFurnitureBackground,
	WindowFurnitureForeground,
	WindowFurnitureBorder,
};

enum class StyleAttribute {
	Bar,
	BarGreen,
	BarRed,
	Focussed,
	FontBodyBold,
	FontSymbol,
	IconBar,
	IconBarInner,
	IconBarButton,
	Grid,
	GridTitle,
	Menu,
	MenuBackground,
	MenuContainer,
	MenuHeader,
	MenuItem,
	MenuItemBold,
	MenuSeparator,
	Scrollbar,
	TransparentWindow,
	Window,
	WindowButton,
	WindowContent,
	WindowContentPadded,
	WindowHeader,
	WindowActive,
	WindowInactive,
	WindowFullScreen,
	WindowHeaderFullScreen,
	BorderedContent,
};

enum class FontAttribute {
	Window,
	WindowBold,
	Body,
	BodyBold,
	Mono,
	Header,
	Symbol
};

enum class ConstAttribute {
	BodyFontSize,
	CornerRadius,
	ContainerPadding,
	ControlPadding,
	FurnitureFontSize,
	HeaderFontSize,
	MenuHeaderHeight,
	MonoFontSize,
	ScrollbarSize,
	WindowBorderWidth,
	WindowFontSize,
	WindowHeaderHeight,
	WindowFurnitureWidth,
	WindowContentPadding,
	WindowContentPaddingPadded,
};

#define LV_SYMBOL_MY_CLOSE        "\xEF\x8B\x93" // 0xF00D close
#define LV_SYMBOL_MAXIMISE        "\xEF\x8B\x90" // 0xF2D0 window maximize
#define LV_SYMBOL_MINIMISE        "\xEF\x8B\x91" // 0xF2D1 window minimise
#define LV_SYMBOL_RESIZE          "\xEF\x8C\x9D" // 0xf31D
#define LV_SYMBOL_MENURIGHT       "\xEF\x81\x94" // 0xf054
#define LV_SYMBOL_LAYOUT          "\xEE\x8A\x8F" // 0xf054
#define LV_SYMBOL_FILE          "\xEF\x85\x9B" // 0xf15b

class ThemeManager {
public:
	static lv_color_t GetColour(ColourAttribute ca);
	static lv_style_t* GetStyle(StyleAttribute ca);
	static lv_font_t* GetFont(FontAttribute ca);
	static uint32_t GetConst(ConstAttribute ca);
	static void AddColour(ColourAttribute ca, lv_color_t v);
	static void AddStyle(StyleAttribute sa, lv_style_t* v);
	static void AddFont(FontAttribute fa, lv_font_t* v);
	static void AddConst(ConstAttribute ca, uint32_t v);
private:
	static std::map<ColourAttribute, lv_color_t> theme_colours;
	static std::map<StyleAttribute, lv_style_t*> theme_styles;
	static std::map<FontAttribute, lv_font_t*> theme_fonts;
	static std::map<ConstAttribute, uint32_t> theme_consts;
};