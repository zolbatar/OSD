#include "FontManager.h"
#ifndef CLION
#include <circle/logger.h>
#endif
#include <stdio.h>

size_t FontManager::font_index = 0;
std::map<std::string, size_t> FontManager::font_indexes;
std::vector<char*> FontManager::font_alloc;
std::vector<Font*> FontManager::fonts;
lv_font_t* loaded = NULL;

FontManager::FontManager()
{
	this->id = "Font Manager";
	this->name = "Font Manager";
}

void FontManager::InitFonts()
{
	task_override = this;
	loaded = LoadFile("Plex Sans", "/osd/fonts/IBMPlexSans-Regular.ttf");
//	loaded = LoadFile("Plex Sans", "/osd/fonts/IBMPlexMono-Regular.ttf");
//	loaded = LoadFile("Plex Sans", "/osd/fonts/IBMPlexMono-Regular.ttf");
	task_override = NULL;
}

void FontManager::Run()
{
	SetNameAndAddToList();

	// Do stuff
	while (1) {
		Sleep(100);
	}

	TerminateTask();
}

lv_font_t* FontManager::LoadFile(std::string name, std::string filename)
{
	auto index = font_index++;
	font_indexes.insert(std::make_pair(name, index));

	// Load and build the actual font
	auto f = (stbtt_fontinfo*)malloc(sizeof(stbtt_fontinfo));
	assert(f);

	// Open file and calculate size
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		CLogger::Get()->Write("FontManager", LogDebug, "Error opening font file");
		assert(0);
	}
	fseek(fp, 0L, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	// Allocate memory (we don't deallocate, so will leave a memory leak)
	char* ttf_buffer = (char*)malloc(sz);
	if (!ttf_buffer) {
		CLogger::Get()->Write("FontManager", LogDebug, "Error allocating memory for TTF font");
		assert(0);
	}

	// Read into buffer and close file
	auto r = fread(ttf_buffer, 1, sz, fp);
	assert(r==sz);
	fclose(fp);

	// Save into map
	font_alloc.push_back(ttf_buffer);

	// Now create font
	auto f_o = stbtt_GetFontOffsetForIndex((const unsigned char*)ttf_buffer, 0);
	if (stbtt_InitFont(f, (const unsigned char*)ttf_buffer, f_o)==0) {
		CLogger::Get()->Write("FontManager", LogDebug, "Failure creating TTF font");
		assert(0);
	}

	// Create font
	auto height_px = 20;

	auto font = new Font();
	font->font = f;
	font->scale = stbtt_ScaleForPixelHeight(f, height_px);
	fonts.push_back(font);

	// Baseline
	int ascent;
	stbtt_GetFontVMetrics(f, &ascent, 0, 0);

	auto ft = (lv_font_t*)malloc(sizeof(lv_font_t));
	memset(ft, 0, sizeof(lv_font_t));
	ft->line_height = (int)(static_cast<float>(ascent)*font->scale);
	ft->get_glyph_dsc = FontManager::GlyphDSCHandler;
	ft->get_glyph_bitmap = FontManager::GlyphBitmapHandler;
	ft->dsc = font;
	ft->base_line = 0;
//	CLogger::Get()->Write("FontManager", LogDebug, "%d %f %d", ascent, font->scale, ft->base_line);
	return ft;
}

bool FontManager::GlyphDSCHandler(const lv_font_t* font, lv_font_glyph_dsc_t* dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
	if (font==nullptr || font->dsc==nullptr) {
		return false;
	}

	auto* ctx = (Font*)font->dsc;
	if (dsc_out==nullptr) return false;
	float scale = ctx->scale;

	// Find glyph
	if (stbtt_FindGlyphIndex(ctx->font, (int)unicode_letter)==0) {
		return false;
	}

	// Size
	int x0, y0, x1, y1;
	stbtt_GetCodepointBitmapBox(ctx->font, (int)unicode_letter, scale, scale, &x0, &y0, &x1, &y1);

	int adv_w = 0;
	int left_side_bearing = 0;
	stbtt_GetCodepointHMetrics(ctx->font, (int)unicode_letter, &adv_w, &left_side_bearing);

	int kern = stbtt_GetCodepointKernAdvance(ctx->font, (int)unicode_letter, (int)unicode_letter_next);
	adv_w = static_cast<int>(roundf(static_cast<float>(adv_w)*scale));
	kern = static_cast<int>(roundf(static_cast<float>(kern)*scale));
	left_side_bearing = static_cast<int>(roundf((static_cast<float>(left_side_bearing)*scale)));

	dsc_out->resolved_font = font;
	dsc_out->adv_w = adv_w+kern;
	dsc_out->box_h = (uint16_t)(y1-y0);
	dsc_out->box_w = (uint16_t)(x1-x0);
	dsc_out->ofs_x = (int16_t)left_side_bearing;
	dsc_out->ofs_y = (int16_t)(y1*-1);
	dsc_out->bpp = 8;
	return true;
}

const uint8_t* FontManager::GlyphBitmapHandler(const lv_font_t* font, uint32_t unicode_letter)
{
	if (font==nullptr || font->dsc==nullptr) {
		return nullptr;
	}

	auto ctx = (Font*)font->dsc;
	float scale = ctx->scale;

	// Do we have it cached?
	auto f = ctx->cache.find(unicode_letter);
	if (f!=ctx->cache.end()) {
		return f->second;
	}

	// Create and then cache
	int width;
	int height;
	auto alloc = stbtt_GetCodepointBitmap(ctx->font, scale, scale, (int)unicode_letter, &width, &height, 0, 0);
	ctx->cache.insert(std::make_pair(unicode_letter, alloc));
	return alloc;
}
