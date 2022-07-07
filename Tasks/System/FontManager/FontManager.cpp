#include "FontManager.h"
#include <stdio.h>
#include <circle/logger.h>
#include "../../../Library/json.hpp"

std::map<std::string, Font *> FontManager::loaded_fonts;
extern FontManager *fm;

FontManager::FontManager()
{
    this->id = "Font Manager";
    this->SetName("Font Manager");
    this->priority = TaskPriority::System;
}

void FontManager::InitFonts()
{
    SetOverride(this);

    // Does font cache exist?
    fs.SetCurrentDirectory(":Boot/Config");
    FILINFO fno;
    auto fr = f_stat((fs.GetCurrentDirectory() + "FontManager").c_str(), &fno);
    if (fr == FR_NO_FILE)
    {
        CreateConfigFile();
    }
    LoadConfigFile();
    ClearOverride();
}

void FontManager::Run()
{
    SetNameAndAddToList();
    while (1)
    {
        Yield();
    }
}

void FontManager::LoadConfigFile()
{
    CLogger::Get()->Write("FontManager", LogNotice, "Loading font cache");

    // Open file
    FIL fil;
    if (f_open(&fil, (fs.GetCurrentDirectory() + "FontManager").c_str(), FA_READ | FA_OPEN_EXISTING) != FR_OK)
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Error opening font cache file");
    }
    size_t sz = f_size(&fil);

    // Allocate space
    char *buffer = (char *)malloc(sz + 1);
    if (!buffer)
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Error allocating memory for font cache file");
    }

    // Read
    uint32_t l;
    if (f_read(&fil, buffer, sz, &l) != FR_OK)
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Error loading font cache file");
    }
    buffer[sz] = 0;

    // Parse JSON
    nlohmann::json j = nlohmann::json::parse(buffer);

    // Loop through and create all font objects
    for (auto &font : j["Fonts"])
    {

        // Do we have a font called this already?
        auto fff = loaded_fonts.find(font["name"]);
        if (fff == loaded_fonts.end())
        {
            auto f2 = new Font();
            loaded_fonts.insert(std::make_pair(font["name"], f2));
        }

        // Create font and style
        auto ff = new FontStyle();
        ff->filename = font["filename"];
        ff->font = NULL;

        // Add
        fff = loaded_fonts.find(font["name"]);
        fff->second->styles.insert(std::make_pair(font["style"], ff));
    }

    free(buffer);
}

void FontManager::CreateConfigFile()
{
    CLogger::Get()->Write("FontManager", LogNotice, "Rebuilding font cache");
    nlohmann::json j;
    j["Fonts"] = {};
    fs.SetCurrentDirectory(":Boot/Fonts");
    auto files = fs.ListAllFilesInCurrentDirectory(true);
    for (auto &file : files)
    {
        // Load and build the actual font
        auto f = (stbtt_fontinfo *)malloc(sizeof(stbtt_fontinfo));
        assert(f);

        // Open file
        FIL fil;
        if (f_open(&fil, file.c_str(), FA_READ | FA_OPEN_EXISTING) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error opening font file: %s", file.c_str());
        }
        size_t sz = f_size(&fil);

        // Allocate space
        char *ttf_buffer = (char *)malloc(sz);
        if (!ttf_buffer)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error allocating memory for TTF font");
        }

        // Read
        uint32_t l;
        if (f_read(&fil, ttf_buffer, sz, &l) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error loading font file: %s", file.c_str());
        }
        auto f_o = stbtt_GetFontOffsetForIndex((const unsigned char *)ttf_buffer, 0);
        if (stbtt_InitFont(f, (const unsigned char *)ttf_buffer, f_o) == 0)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Failure creating TTF font");
        }

        // Get name
        auto name = GetName(f, 1);
        auto style = GetName(f, 2);
        j["Fonts"].push_back({{
                                  "name",
                                  name,
                              },
                              {"style", style},
                              {"filename", file.c_str()}});

        // Close and free up memory
        f_close(&fil);
        free(f);
        free(ttf_buffer);
    }

    // Write config
    fs.SetCurrentDirectory(":Boot/Config");
    FIL fil;
    if (f_open(&fil, (fs.GetCurrentDirectory() + "FontManager").c_str(), FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Error opening font config file for write");
    }
    auto json_out = j.dump(4);
    uint32_t l;
    if (f_write(&fil, json_out.c_str(), json_out.length(), &l) != FR_OK)
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Error writing font config file");
    }
    f_close(&fil);
}

std::string FontManager::GetName(stbtt_fontinfo *f, int id)
{
    int length = 0;
    auto name = stbtt_GetFontNameString(f, &length, STBTT_PLATFORM_ID_MICROSOFT, STBTT_MS_EID_UNICODE_BMP,
                                        STBTT_MS_LANG_ENGLISH, id);

    std::string sname;
    for (int i = 1; i < length; i += 2)
        sname += name[i];
    return sname;
}

FontSize *FontManager::InternalLookup(std::string name, std::string style_name, int size)
{
    SetOverride(fm);
    auto f = loaded_fonts.find(name);
    if (f == loaded_fonts.end())
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Can't find TTF font: %s", name.c_str());
    }

    // Find style
    auto style = f->second->styles.find(style_name);
    if (style == f->second->styles.end())
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Can't find TTF font style: %s", style_name.c_str());
    }

    // Is it loaded?
    if (style->second->font == NULL)
    {

        style->second->font = (stbtt_fontinfo *)malloc(sizeof(stbtt_fontinfo));

        // Open file
        FIL fil;
        if (f_open(&fil, style->second->filename.c_str(), FA_READ | FA_OPEN_EXISTING) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error opening font file: %s",
                                  style->second->filename.c_str());
        }
        size_t sz = f_size(&fil);

        // Allocate space
        char *ttf_buffer = (char *)malloc(sz);
        if (!ttf_buffer)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error allocating memory for TTF font");
        }

        // Read
        uint32_t l;
        if (f_read(&fil, ttf_buffer, sz, &l) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error loading font file: %s",
                                  style->second->filename.c_str());
        }
        auto f_o = stbtt_GetFontOffsetForIndex((const unsigned char *)ttf_buffer, 0);
        if (stbtt_InitFont(style->second->font, (const unsigned char *)ttf_buffer, f_o) == 0)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Failure creating TTF font");
        }
    }

    // Have we cached it?
    auto f2 = style->second->sizes.find(size);
    if (f2 != style->second->sizes.end())
    {
        ClearOverride();
        return f2->second;
    }

    auto ff = new FontSize();
    ff->scale = stbtt_ScaleForPixelHeight(style->second->font, size);

    // Baseline
    int ascent;
    int descent;
    int linegap;
    stbtt_GetFontVMetrics(style->second->font, &ascent, &descent, &linegap);
    //	CLogger::Get()->Write("FontManager", LogDebug, "%d %d %d", ascent, descent, linegap);
    //	while (1);

    // Create lv_font_t structure
    auto line_height = ascent + abs(descent) + linegap;
    ff->lv = (lv_font_t *)malloc(sizeof(lv_font_t));
    memset(ff->lv, 0, sizeof(lv_font_t));
    ff->font = style->second->font;
    ff->lv->line_height = (static_cast<float>(line_height) * ff->scale);
    ff->lv->get_glyph_dsc = FontManager::GlyphDSCHandler;
    ff->lv->get_glyph_bitmap = FontManager::GlyphBitmapHandler;
    ff->lv->dsc = ff;
    ff->lv->base_line = (static_cast<float>(line_height - ascent) * ff->scale);

    // Save for later
    style->second->sizes.insert(std::make_pair(size, ff));
    ClearOverride();
    return ff;
}

lv_font_t *FontManager::GetFontByNameStyleAndSize(std::string name, std::string style_name, int size)
{
    // return (lv_font_t *)&lv_font_unscii_8;
    // return (lv_font_t *)&lv_font_unscii_16;
    // return (lv_font_t *)&lv_font_montserrat_12_subpx;
    auto f = InternalLookup(name, style_name, size);
    return f->lv;
}

bool FontManager::GlyphDSCHandler(const lv_font_t *font, lv_font_glyph_dsc_t *dsc_out, uint32_t unicode_letter,
                                  uint32_t unicode_letter_next)
{
    if (font == nullptr || font->dsc == nullptr)
    {
        return false;
    }

    auto *ctx = (FontSize *)font->dsc;
    if (dsc_out == nullptr)
        return false;
    float scale = ctx->scale;

    // Find glyph
    if (stbtt_FindGlyphIndex(ctx->font, (int)unicode_letter) == 0)
    {
        return false;
    }

    // Size
    int x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(ctx->font, (int)unicode_letter, scale, scale, &x0, &y0, &x1, &y1);

    int adv_w = 0;
    int left_side_bearing = 0;
    stbtt_GetCodepointHMetrics(ctx->font, (int)unicode_letter, &adv_w, &left_side_bearing);

    int kern = stbtt_GetCodepointKernAdvance(ctx->font, (int)unicode_letter, (int)unicode_letter_next);
    adv_w = static_cast<int>(truncf(static_cast<float>(adv_w) * scale));
    kern = static_cast<int>(truncf(static_cast<float>(kern) * scale));
    left_side_bearing = static_cast<int>(truncf((static_cast<float>(left_side_bearing) * scale)));

    dsc_out->resolved_font = font;
    dsc_out->adv_w = adv_w + kern;
    dsc_out->box_h = (uint16_t)(y1 - y0);
    dsc_out->box_w = (uint16_t)(x1 - x0);
    dsc_out->ofs_x = (int16_t)left_side_bearing;
    dsc_out->ofs_y = (int16_t)(y1 * -1);
    dsc_out->bpp = 8;
    return true;
}

const uint8_t *FontManager::GlyphBitmapHandler(const lv_font_t *font, uint32_t unicode_letter)
{
    if (font == nullptr || font->dsc == nullptr)
    {
        return nullptr;
    }

    auto ctx = (FontSize *)font->dsc;
    float scale = ctx->scale;

    // Do we have it cached?
    auto f = ctx->cache.find(unicode_letter);
    if (f != ctx->cache.end())
    {
        return f->second;
    }

    // Create and then cache
    int width;
    int height;
    auto alloc = stbtt_GetCodepointBitmap(ctx->font, scale, scale, (int)unicode_letter, &width, &height, 0, 0);
    ctx->cache.insert(std::make_pair(unicode_letter, alloc));
    return alloc;
}

int FontManager::GetWidth(const lv_font_t *font, const char *string)
{
    lv_point_t label_size;
    lv_txt_get_size(&label_size, string, font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
    return label_size.x;
}
