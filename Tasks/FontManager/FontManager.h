#pragma once
#include "../../OS/OS.h"
#include "../../GUI/Controls/Control.h"
#include "../../GUI/Controls/Canvas.h"
#include "../../GUI/Window.h"
#include "../../GUI/lvgl.h"
#include "../../GUI/Style.h"
#include <map>

// Lightweight(ish) truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

struct Font {
	stbtt_fontinfo* font;
	float scale;
	std::map<uint32_t, uint8_t*> cache;
};

class FontManager : public OSDTask {
public:
	FontManager();
	void InitFonts();
	void Run();
	lv_font_t* LoadFile(std::string name, std::string filename);
private:
	static size_t font_index;
	static std::map<std::string, size_t> font_indexes;
	static std::vector<char*> font_alloc;
	static std::vector<Font*> fonts;

	static bool GlyphDSCHandler(const lv_font_t* font, lv_font_glyph_dsc_t* dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next);
	static const uint8_t* GlyphBitmapHandler(const lv_font_t* font, uint32_t unicode_letter);
};