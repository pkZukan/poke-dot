#include "bfcpx.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/text_server.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
using BinaryFontUtils::has_bytes;

void BinaryCompositeFont::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "path"), &BinaryCompositeFont::LoadFromFile);
	ClassDB::bind_method(D_METHOD("LoadFromBuffer", "buffer", "base_dir"), &BinaryCompositeFont::LoadFromBuffer);
	ClassDB::bind_method(D_METHOD("load_bfcpx", "path"), &BinaryCompositeFont::load_bfcpx);
	ClassDB::bind_method(D_METHOD("get_warnings"), &BinaryCompositeFont::get_warnings);
}

bool BinaryCompositeFont::read_name(const Ref<StreamPeerBuffer> &sp, uint64_t offset, String &name)
{
	if (!has_bytes(sp, offset, 1)) return false;
	sp->seek(offset);
	PackedByteArray bytes;
	while (sp->get_available_bytes()) {
		const uint8_t c = sp->get_u8();
		if (!c) {
			name = bytes.get_string_from_utf8();
			return !name.is_empty();
		}
		bytes.push_back(c);
	}
	return false;
}

bool BinaryCompositeFont::read_ranges(const Ref<StreamPeerBuffer> &sp, uint64_t offset, uint32_t count, Vector<Vector2i> &ranges)
{
	if (!has_bytes(sp, offset, uint64_t(count) * 8)) return false;
	sp->seek(offset);
	for (uint32_t i = 0; i < count; ++i) {
		const uint32_t first = sp->get_u32();
		const uint32_t last = sp->get_u32();
		if (first > last || last > 0x10ffff) return false;
		ranges.push_back(Vector2i(first, last));
	}
	return true;
}

Error BinaryCompositeFont::parse_node(const Ref<StreamPeerBuffer> &sp, uint64_t offset, Vector<Entry> &entries, HashSet<uint64_t> &active, int depth)
{
	if (depth > 64 || active.has(offset) || !has_bytes(sp, offset, 16) || entries.size() > 1024) return ERR_FILE_CORRUPT;
	active.insert(offset);
	sp->seek(offset);
	const uint32_t type = sp->get_u32();
	if (type == 2) {
		const uint64_t first = offset + sp->get_u32();
		const uint64_t second = offset + sp->get_u32();
		Error error = parse_node(sp, first, entries, active, depth + 1);
		if (error != OK) return error;
		error = parse_node(sp, second, entries, active, depth + 1);
		if (error != OK) return error;
	} else if (type == 0) {
		Entry entry;
		const uint64_t name = offset + sp->get_u32();
		const uint32_t count = sp->get_u32();
		const uint64_t ranges = offset + sp->get_u32();
		if (!read_name(sp, name, entry.name) || !read_ranges(sp, ranges, count, entry.ranges)) return ERR_FILE_CORRUPT;
		entries.push_back(entry);
	} else if (type == 1) {
		if (!has_bytes(sp, offset, 32)) return ERR_FILE_CORRUPT;
		const float size = sp->get_float();
		sp->get_u32(); // outline set flags
		const uint32_t count = sp->get_u32();
		const uint64_t records = offset + sp->get_u32();
		if (!Math::is_finite(size) || size <= 0 || size > 512 || !count || count > 1024 ||
			!has_bytes(sp, records, uint64_t(count) * 40)) return ERR_FILE_CORRUPT;
		for (uint32_t i = 0; i < count; ++i) {
			sp->seek(records + uint64_t(i) * 40);
			Entry entry;
			entry.outline = true;
			entry.size = size;
			entry.adjustment = sp->get_float();
			entry.scale = sp->get_float();
			sp->get_float(); // reserved in the bundled assets
			const uint64_t name = offset + sp->get_u32();
			entry.flags = sp->get_u32();
			entry.advance_scale = sp->get_float();
			sp->get_u32(); // rasterization settings; not mapped to Godot hinting
			sp->get_u32();
			const uint32_t range_count = sp->get_u32();
			const uint64_t ranges = offset + sp->get_u32();
			if (!Math::is_finite(entry.adjustment) || !Math::is_finite(entry.scale) || !Math::is_finite(entry.advance_scale) ||
				entry.scale <= 0 || entry.scale > 16 || entry.advance_scale <= 0 || entry.advance_scale > 16 ||
				!read_name(sp, name, entry.name) || !read_ranges(sp, ranges, range_count, entry.ranges)) return ERR_FILE_CORRUPT;
			entries.push_back(entry);
		}
	} else return ERR_UNAVAILABLE;
	active.erase(offset);
	return OK;
}

Error BinaryCompositeFont::load_outline(const String &path, const Entry &entry, Ref<FontFile> &font)
{
	String filename = path;
	if (!FileAccess::file_exists(filename)) {
		const String extension = path.get_extension().to_lower();
		if (extension == "otf" || extension == "ttf") filename = path.get_basename() + ".bf" + extension;
	}
	Ref<FileAccess> file = FileAccess::open(filename, FileAccess::READ);
	if (file.is_null()) return ERR_FILE_CANT_OPEN;
	PackedByteArray data = file->get_buffer(file->get_length());
	if (data.size() < 12) return ERR_FILE_CORRUPT;
	const uint32_t magic = data.decode_u32(0);
	uint32_t key = 0;
	// Switch Toolbox BFTTF.cs / BFTTFutil: big-endian words XORed after an 8-byte header.
	if (magic == 0x1a879bd9) key = 2785117442U;
	else if (magic == 0x1e1af836) key = 1231165446U;
	else if (magic == 0xc1de68f3) key = 2364726489U;
	if (key) {
		if (data.size() % 4) return ERR_FILE_CORRUPT;
		Ref<StreamPeerBuffer> sp;
		sp.instantiate();
		sp->set_data_array(data);
		sp->set_big_endian(true);
		sp->seek(4);
		const uint32_t length = sp->get_u32() ^ key;
		if (length != data.size() - 8) return ERR_FILE_CORRUPT;
		data = data.slice(8);
		uint8_t *bytes = data.ptrw();
		for (int64_t i = 0; i < data.size(); ++i) bytes[i] ^= uint8_t(key >> (24 - (i % 4) * 8));
	}
	if (data.slice(0, 4).get_string_from_ascii() != "OTTO" && data.decode_u32(0) != 0x00000100) return ERR_FILE_UNRECOGNIZED;
	Ref<FontFile> source;
	source.instantiate();
	source->set_data(data);
	source->set_allow_system_fallback(false);
	if (source->get_supported_chars().is_empty()) return ERR_FILE_CORRUPT;
	if (entry.ranges.is_empty() && entry.scale == 1 && entry.advance_scale == 1) {
		font = source;
		return OK;
	}

	// A bitmap FontFile provides exact character coverage for a restricted member.
	// Keeping the original outline data would let TextServer select excluded glyphs.
	const int size = MAX(1, int(Math::round(entry.size)));
	const int raster_size = MAX(1, int(Math::round(entry.size * entry.scale)));
	const Vector2i raster(raster_size, 0);
	font.instantiate();
	font->set_fixed_size(size);
	font->set_fixed_size_scale_mode(TextServer::FIXED_SIZE_SCALE_ENABLED);
	font->set_allow_system_fallback(false);
	source->set_subpixel_positioning(TextServer::SUBPIXEL_POSITIONING_DISABLED);
	const String characters = source->get_supported_chars();
	for (int i = 0; i < characters.length(); ++i) {
		const char32_t code = characters[i];
		if (!BinaryFontUtils::in_ranges(code, entry.ranges)) continue;
		const int index = source->get_glyph_index(raster_size, code, 0);
		source->render_glyph(0, raster, index);
		font->set_glyph_texture_idx(0, Vector2i(size, 0), code, source->get_glyph_texture_idx(0, raster, index));
		font->set_glyph_uv_rect(0, Vector2i(size, 0), code, source->get_glyph_uv_rect(0, raster, index));
		font->set_glyph_size(0, Vector2i(size, 0), code, source->get_glyph_size(0, raster, index));
		font->set_glyph_offset(0, Vector2i(size, 0), code, source->get_glyph_offset(0, raster, index));
		font->set_glyph_advance(0, size, code, source->get_glyph_advance(0, raster_size, index) * (entry.advance_scale / entry.scale));
	}
	for (int i = 0; i < source->get_texture_count(0, raster); ++i) {
		font->set_texture_image(0, Vector2i(size, 0), i, source->get_texture_image(0, raster, i));
	}
	font->set_cache_ascent(0, size, source->get_ascent(raster_size));
	font->set_cache_descent(0, size, source->get_descent(raster_size));
	return OK;
}

Error BinaryCompositeFont::LoadFromBuffer(const PackedByteArray &buffer, const String &base_dir)
{
	warnings.clear();
	Ref<StreamPeerBuffer> sp = BinaryFontUtils::open_stream(buffer);
	if (sp.is_null()) return ERR_FILE_CORRUPT;
	if (sp->get_string(4) != "FCPX") return ERR_FILE_UNRECOGNIZED;
	sp->seek(6);
	const uint16_t header_size = sp->get_u16();
	const uint32_t version = sp->get_u32();
	const uint32_t file_size = sp->get_u32();
	if (version != 0x09030000) return ERR_UNAVAILABLE;
	if (file_size != buffer.size() || header_size < 20 || sp->get_u16() != 1 || !has_bytes(sp, header_size, 4)) return ERR_FILE_CORRUPT;
	sp->seek(header_size);
	const uint64_t root = uint64_t(header_size) + sp->get_u32();
	Vector<Entry> entries;
	HashSet<uint64_t> active;
	Error error = parse_node(sp, root, entries, active, 0);
	if (error != OK) return error;
	TypedArray<Font> fonts;
	PackedStringArray loaded_warnings;
	for (const Entry &entry : entries) {
		Ref<FontFile> font;
		const String path = base_dir.path_join(entry.name);
		if (entry.outline) {
			error = load_outline(path, entry, font);
			const String warning = "BFCPX outline rasterization flags use Godot defaults; Nintendo hinting and weight settings are not reproduced.";
			if (!loaded_warnings.has(warning)) loaded_warnings.push_back(warning);
		} else {
			Ref<BinaryFont> bitmap;
			bitmap.instantiate();
			error = bitmap->load_bffnt_filtered(path, entry.ranges);
			font = bitmap;
		}
		if (error != OK) {
			warnings.push_back(vformat("Could not load BFCPX member '%s' (error %d).", path, error));
			return error;
		}
		fonts.push_back(font);
	}
	if (fonts.is_empty()) return ERR_FILE_CORRUPT;
	clear_cache();
	set_data(PackedByteArray());
	set_allow_system_fallback(false);
	set_fallbacks(fonts);
	// Bitmap composites retain their native, potentially non-square cell size.
	// Outline fonts use a square em; symbol fallbacks must not replace the
	// nominal dimensions of the bitmap font used for the layout's text.
	Vector2 nominal_size(1, 1);
	for (int i = 0; i < fonts.size(); ++i) {
		Ref<Font> member = fonts[i];
		if (member->has_meta("nominal_font_size")) {
			nominal_size = member->get_meta("nominal_font_size");
			break;
		}
	}
	set_meta("nominal_font_size", nominal_size);
	warnings = loaded_warnings;
	return OK;
}

Error BinaryCompositeFont::LoadFromFile(const String &path)
{
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	if (file.is_null()) return ERR_FILE_CANT_OPEN;
	return LoadFromBuffer(file->get_buffer(file->get_length()), path.get_base_dir());
}

Ref<BinaryCompositeFont> BinaryCompositeFont::load_bfcpx(const String &path)
{
	Ref<BinaryCompositeFont> font;
	font.instantiate();
	return font->LoadFromFile(path) == OK ? font : Ref<BinaryCompositeFont>();
}

Variant ResourceFormatLoaderBFCPX::_load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const
{
	Ref<BinaryCompositeFont> font;
	font.instantiate();
	Error error = font->LoadFromFile(path);
	if (error != OK) return error;
	return font;
}

PackedStringArray ResourceFormatLoaderBFCPX::_get_recognized_extensions() const
{
	PackedStringArray extensions;
	extensions.push_back("bfcpx");
	return extensions;
}

bool ResourceFormatLoaderBFCPX::_handles_type(const StringName &type) const
{
	return type == StringName("BinaryCompositeFont") || type == StringName("FontFile") || type == StringName("Font");
}

String ResourceFormatLoaderBFCPX::_get_resource_type(const String &path) const
{
	return path.get_extension().to_lower() == "bfcpx" ? "BinaryCompositeFont" : "";
}
