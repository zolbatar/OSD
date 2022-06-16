#include "FontManager.h"

#ifndef CLION
#include <circle/logger.h>
#endif

#include <stdio.h>
#include <sstream>
#include <codecvt>
#include "InstalledFonts.h"

std::map<std::string, Font*> FontManager::loaded_fonts;

FontManager::FontManager()
{
	this->id = "Font Manager";
	this->name = "Font Manager";
	this->priority = TaskPriority::Low;
}

void FontManager::InitFonts()
{
	SetOverride(this);
	fs.SetCurrentDirectory(":BOOT.$.System.Fonts");
	auto files = fs.ListAllFilesInCurrentDirectory(true);
	while (1);
#ifndef CLION
	CLogger::Get()->Write("Font Manager", LogNotice, "Loading fonts");
#endif
	std::stringstream ss(installed_fonts);
	std::string val;
	while (std::getline(ss, val, '\n')) {
#ifndef CLION
		LoadFile("/osd/System/Fonts/"+val);
#else
		LoadFile("/Users/daryl/Dev/osd/fonts/"+val);
#endif
	}
#ifndef CLION
	CLogger::Get()->Write("Font Manager", LogNotice, "Loaded fonts");
#endif
	ClearOverride();
}

void FontManager::Run()
{
	SetNameAndAddToList();
	while (1) {
		Yield();
	}
	TerminateTask();
}

std::string FontManager::GetName(stbtt_fontinfo* f, int id)
{
	int length = 0;
	auto name = stbtt_GetFontNameString(f, &length, STBTT_PLATFORM_ID_MICROSOFT, STBTT_MS_EID_UNICODE_BMP, STBTT_MS_LANG_ENGLISH, id);

	std::string sname;
	for (int i = 1; i<length; i += 2)
		sname += name[i];
	return sname;
}

void FontManager::LoadFile(std::string filename)
{
	// Load and build the actual font
	auto f = (stbtt_fontinfo*)malloc(sizeof(stbtt_fontinfo));
	assert(f);

	// Open file and calculate size
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) {
#ifdef CLION
		printf("Error opening font file: %s\n", filename.c_str());
#else
		CLogger::Get()->Write("FontManager", LogDebug, "Error opening font file: %s", filename.c_str());
#endif
		assert(0);
	}
	fseek(fp, 0L, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	// Allocate memory (we don't deallocate, so will leave a memory leak)
	char* ttf_buffer = (char*)malloc(sz);
	if (!ttf_buffer) {
#ifdef CLION
		printf("Error allocating memory for TTF font\n");
#else
		CLogger::Get()->Write("FontManager", LogDebug, "Error allocating memory for TTF font");
#endif
		assert(0);
	}

	// Read into buffer and close file
	auto r = fread(ttf_buffer, 1, sz, fp);
	assert(r==sz);
	fclose(fp);

	// Now create font
	auto f_o = stbtt_GetFontOffsetForIndex((const unsigned char*)ttf_buffer, 0);
	if (stbtt_InitFont(f, (const unsigned char*)ttf_buffer, f_o)==0) {
#ifdef CLION
		printf("Failure creating TTF font\n");
#else
		CLogger::Get()->Write("FontManager", LogDebug, "Failure creating TTF font");
#endif
		assert(0);
	}

	// Get name
	auto name = GetName(f, 1);
	auto style = GetName(f, 2);

	// Do we have a font called this already?
	auto fff = loaded_fonts.find(name);
	if (fff==loaded_fonts.end()) {
		auto f2 = new Font();
		loaded_fonts.insert(std::make_pair(name, f2));
	}

	// Create font and style
	auto ff = new FontStyle();
	ff->font = f;

	// Add
	fff = loaded_fonts.find(name);
	fff->second->styles.insert(std::make_pair(style, ff));

#ifndef CLION
	//CLogger::Get()->Write("FontManager", LogNotice, "Loaded '%s', '%s', '%s'", filename.c_str(), name.c_str(), style.c_str()	);
#endif
}

FontSize* FontManager::InternalLookup(std::string name, std::string style_name, int size)
{
	auto f = loaded_fonts.find(name);
	if (f==loaded_fonts.end()) {
#ifdef CLION
		printf("Can't find TTF font: %s\n", name.c_str());
#else
		CLogger::Get()->Write("FontManager", LogDebug, "Can't find TTF font: %s", name.c_str());
#endif
		assert(0);
	}

	// Find style
	auto style = f->second->styles.find(style_name);
	if (style==f->second->styles.end()) {
#ifdef CLION
		printf("Can't find style for TTF font\n");
#else
		CLogger::Get()->Write("FontManager", LogDebug, "Can't find TTF font");
#endif
		assert(0);
	}

	// Have we cached it?
	auto f2 = style->second->sizes.find(size);
	if (f2!=style->second->sizes.end()) {
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
	auto line_height = ascent+abs(descent)+linegap;
	ff->lv = (lv_font_t*)malloc(sizeof(lv_font_t));
	memset(ff->lv, 0, sizeof(lv_font_t));
	ff->font = style->second->font;
	ff->lv->line_height = (static_cast<float>(line_height)*ff->scale);
	ff->lv->get_glyph_dsc = FontManager::GlyphDSCHandler;
	ff->lv->get_glyph_bitmap = FontManager::GlyphBitmapHandler;
	ff->lv->dsc = ff;
	ff->lv->base_line = (static_cast<float>(line_height-ascent)*ff->scale);

	// Save for later
	style->second->sizes.insert(std::make_pair(size, ff));
	return ff;
}

lv_font_t* FontManager::GetFontByNameStyleAndSize(std::string name, std::string style_name, int size)
{
	auto f = InternalLookup(name, style_name, size);
	return f->lv;
}

bool FontManager::GlyphDSCHandler(const lv_font_t* font, lv_font_glyph_dsc_t* dsc_out, uint32_t unicode_letter,
		uint32_t unicode_letter_next)
{
	if (font==nullptr || font->dsc==nullptr) {
		return false;
	}

	auto* ctx = (FontSize*)font->dsc;
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
	adv_w = static_cast<int>(truncf(static_cast<float>(adv_w)*scale));
	kern = static_cast<int>(truncf(static_cast<float>(kern)*scale));
	left_side_bearing = static_cast<int>(truncf((static_cast<float>(left_side_bearing)*scale)));

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

	auto ctx = (FontSize*)font->dsc;
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

int FontManager::GetWidth(const lv_font_t* font, const char* string)
{
	lv_point_t label_size;
	lv_txt_get_size(&label_size, string, font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
	return label_size.x;
}

