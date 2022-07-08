#pragma once
#include <OS.h>
#include <WindowManager/WindowManager.h>

// Lightweight(ish) truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

struct FontSize
{
    stbtt_fontinfo *font;
    std::map<uint32_t, uint8_t *> cache;
    float scale;
    lv_font_t *lv;
};

struct FontStyle
{
    std::string filename;
    stbtt_fontinfo *font = NULL;
    std::map<int, FontSize *> sizes;
};

struct Font
{
    std::map<std::string, FontStyle *> styles;
};

class FontManager : public OSDTask
{
  public:
    FontManager();
    void InitFonts();
    void Run();
    std::string GetName(stbtt_fontinfo *, int id);
    static lv_font_t *GetFontByNameStyleAndSize(std::string name, std::string style_name, int size);
    static int GetWidth(const lv_font_t *font, const char *string);
    void LoadConfigFile();
    void CreateConfigFile();

  private:
    static std::map<std::string, Font *> loaded_fonts;

    static FontSize *InternalLookup(std::string name, std::string style_name, int size);
    static bool GlyphDSCHandler(const lv_font_t *font, lv_font_glyph_dsc_t *dsc_out, uint32_t unicode_letter,
                                uint32_t unicode_letter_next);
    static const uint8_t *GlyphBitmapHandler(const lv_font_t *font, uint32_t unicode_letter);
};