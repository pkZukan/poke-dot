#include "bffnt.h"
#include "bntx.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/text_server.hpp>
#include <godot_cpp/templates/hash_set.hpp>

using namespace godot;

void BinaryFont::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "path"), &BinaryFont::LoadFromFile);
	ClassDB::bind_method(D_METHOD("LoadFromBuffer", "buffer"), &BinaryFont::LoadFromBuffer);
	ClassDB::bind_method(D_METHOD("load_bffnt", "path"), &BinaryFont::load_bffnt);
	ClassDB::bind_method(D_METHOD("from_bffnt", "path"), &BinaryFont::from_bffnt);
}

bool BinaryFontUtils::has_bytes(const Ref<StreamPeerBuffer> &stream, uint64_t offset, uint64_t length)
{
	const uint64_t size = stream->get_size();
	return offset <= size && length <= size - offset;
}

Ref<StreamPeerBuffer> BinaryFontUtils::open_stream(const PackedByteArray &data)
{
	if (data.size() < 20 || data.size() > INT32_MAX) return Ref<StreamPeerBuffer>();
	const bool be = data[4] == 0xfe && data[5] == 0xff;
	if (!be && !(data[4] == 0xff && data[5] == 0xfe)) return Ref<StreamPeerBuffer>();
	Ref<StreamPeerBuffer> stream;
	stream.instantiate();
	stream->set_data_array(data);
	stream->set_big_endian(be);
	return stream;
}

bool BinaryFontUtils::in_ranges(uint32_t code, const Vector<Vector2i> &ranges)
{
	if (ranges.is_empty()) return true;
	for (const Vector2i &range : ranges) {
		if (int64_t(code) >= range.x && int64_t(code) <= range.y) return true;
	}
	return false;
}

Error BinaryFont::LoadFromFile(const String &path)
{
	return load_bffnt_filtered(path, Vector<Vector2i>());
}

Error BinaryFont::LoadFromBuffer(const PackedByteArray &buffer)
{
	return parse(buffer, Vector<Vector2i>());
}

Error BinaryFont::load_bffnt_filtered(const String &path, const Vector<Vector2i> &ranges)
{
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	if (file.is_null()) return ERR_FILE_CANT_OPEN;
	return parse(file->get_buffer(file->get_length()), ranges);
}

Error BinaryFont::parse(const PackedByteArray &buffer, const Vector<Vector2i> &ranges)
{
	using BinaryFontUtils::has_bytes;
	Ref<StreamPeerBuffer> sp = BinaryFontUtils::open_stream(buffer);
	if (sp.is_null()) return ERR_FILE_CORRUPT;
	if (sp->get_string(4) != "FFNT") return ERR_FILE_UNRECOGNIZED;
	sp->seek(6);
	const uint16_t header_size = sp->get_u16();
	const uint32_t version = sp->get_u32();
	const uint32_t file_size = sp->get_u32();
	const uint16_t block_count = sp->get_u16();
	// Switch Toolbox BXFNT: NX starts at 4.1 and stores sheets in a BNTX array.
	if (version != 0x04010000) return ERR_UNAVAILABLE;
	if (header_size < 20 || file_size != buffer.size()) return ERR_FILE_CORRUPT;

	struct Block { String tag; uint32_t size; };
	HashMap<uint32_t, Block> blocks;
	uint64_t pos = header_size;
	uint32_t finf = 0;
	for (uint32_t i = 0; i < block_count; ++i) {
		if (!has_bytes(sp, pos, 8)) return ERR_FILE_CORRUPT;
		sp->seek(pos);
		const String tag = sp->get_string(4);
		const uint32_t size = sp->get_u32();
		if (size < 8 || !has_bytes(sp, pos, size)) return ERR_FILE_CORRUPT;
		blocks.insert(pos + 8, {tag, size - 8});
		if (tag == "FINF") {
			if (finf) return ERR_FILE_CORRUPT;
			finf = pos + 8;
		}
		pos += size;
	}
	if (pos != file_size || !finf || blocks[finf].size < 24) return ERR_FILE_CORRUPT;
	sp->seek(finf);
	const uint8_t font_type = sp->get_u8();
	const int height = sp->get_u8();
	sp->get_u8(); // nominal width
	const int ascent = sp->get_u8();
	const int line_feed = sp->get_16();
	sp->get_u16(); // alternate glyph index
	const GlyphWidth default_width{sp->get_8(), sp->get_u8(), sp->get_8()};
	const uint8_t encoding = sp->get_u8();
	const uint32_t tglp = sp->get_u32();
	uint32_t cwdh = sp->get_u32();
	uint32_t cmap = sp->get_u32();
	if (font_type != 1 || encoding != 1) return ERR_UNAVAILABLE;
	if (!height || ascent > height || line_feed < ascent || !blocks.has(tglp) ||
		blocks[tglp].tag != "TGLP" || blocks[tglp].size < 24) return ERR_FILE_CORRUPT;

	sp->seek(tglp);
	const int cell_width = sp->get_u8();
	const int cell_height = sp->get_u8();
	const int sheet_count = sp->get_u8();
	sp->get_u8(); // maximum character width
	const uint32_t sheet_size = sp->get_u32();
	const int baseline = sp->get_16();
	sp->get_u16(); // texture format is described by the embedded BNTX
	const int columns = sp->get_u16();
	const int rows = sp->get_u16();
	const int sheet_width = sp->get_u16();
	const int sheet_height = sp->get_u16();
	const uint32_t data_offset = sp->get_u32();
	const uint64_t data_size = uint64_t(sheet_size) * sheet_count;
	if (!columns || !rows || !cell_width || !cell_height || !sheet_count || !sheet_size ||
		int64_t(columns) * (cell_width + 1) > sheet_width || int64_t(rows) * (cell_height + 1) > sheet_height ||
		data_offset < tglp + 24 || uint64_t(data_offset) + data_size > uint64_t(tglp) + blocks[tglp].size ||
		!has_bytes(sp, data_offset, data_size)) return ERR_FILE_CORRUPT;

	HashMap<uint16_t, GlyphWidth> widths;
	HashMap<uint32_t, uint16_t> characters;
	HashSet<uint32_t> visited;
	while (cwdh) {
		if (visited.has(cwdh) || !blocks.has(cwdh) || blocks[cwdh].tag != "CWDH" || blocks[cwdh].size < 8) return ERR_FILE_CORRUPT;
		visited.insert(cwdh);
		sp->seek(cwdh);
		const uint32_t first = sp->get_u16();
		const uint32_t last = sp->get_u16();
		const uint32_t next = sp->get_u32();
		if (last < first || 8 + (last - first + 1) * 3 > blocks[cwdh].size) return ERR_FILE_CORRUPT;
		for (uint32_t index = first; index <= last; ++index) {
			widths.insert(index, {sp->get_8(), sp->get_u8(), sp->get_8()});
		}
		cwdh = next;
	}
	while (cmap) {
		if (visited.has(cmap) || !blocks.has(cmap) || blocks[cmap].tag != "CMAP" || blocks[cmap].size < 18) return ERR_FILE_CORRUPT;
		visited.insert(cmap);
		sp->seek(cmap);
		const uint32_t first = sp->get_u32();
		const uint32_t last = sp->get_u32();
		const uint16_t method = sp->get_u16();
		sp->get_u16();
		const uint32_t next = sp->get_u32();
		if (first > last || last > 0x10ffff) return ERR_FILE_CORRUPT;
		if (method == 0) {
			const uint32_t start = sp->get_u16();
			if (start + last - first > 0xffff) return ERR_FILE_CORRUPT;
			for (uint32_t code = first; code <= last; ++code) characters.insert(code, start + code - first);
		} else if (method == 1) {
			if (16 + uint64_t(last - first + 1) * 2 > blocks[cmap].size) return ERR_FILE_CORRUPT;
			for (uint32_t code = first; code <= last; ++code) characters.insert(code, sp->get_u16());
		} else if (method == 2) {
			const uint32_t count = sp->get_u16();
			if (20 + uint64_t(count) * 8 > blocks[cmap].size) return ERR_FILE_CORRUPT;
			sp->get_u16();
			for (uint32_t i = 0; i < count; ++i) {
				const uint32_t code = sp->get_u32();
				const uint16_t index = sp->get_u16();
				sp->get_u16();
				if (code < first || code > last) return ERR_FILE_CORRUPT;
				characters.insert(code, index);
			}
		} else return ERR_UNAVAILABLE;
		cmap = next;
	}
	HashMap<Vector2i, int16_t> kerning;
	for (const KeyValue<uint32_t, Block> &block : blocks) {
		if (block.value.tag != "KRNG") continue;
		const uint32_t base = block.key;
		const uint32_t length = block.value.size;
		if (length < 4) return ERR_FILE_CORRUPT;
		sp->seek(base);
		const uint32_t count = sp->get_u16();
		if (4 + count * 8 > length) return ERR_FILE_CORRUPT;
		for (uint32_t i = 0; i < count; ++i) {
			sp->seek(base + 4 + i * 8);
			const uint32_t first = sp->get_u32();
			const uint32_t offset = sp->get_u32();
			if (first > 0x10ffff || offset > length || length - offset < 4) return ERR_FILE_CORRUPT;
			sp->seek(base + offset);
			const uint32_t pairs = sp->get_u16();
			sp->get_u16();
			if (4 + pairs * 8 > length - offset) return ERR_FILE_CORRUPT;
			for (uint32_t j = 0; j < pairs; ++j) {
				const uint32_t second = sp->get_u32();
				const int16_t adjustment = sp->get_16();
				sp->get_u16();
				if (second > 0x10ffff) return ERR_FILE_CORRUPT;
				if (BinaryFontUtils::in_ranges(first, ranges) && BinaryFontUtils::in_ranges(second, ranges)) {
					kerning.insert(Vector2i(first, second), adjustment);
				}
			}
		}
	}

	// Reuse the BNTX decoder, including its array layer and channel selectors.
	PackedByteArray texture_data = buffer.slice(data_offset, data_offset + data_size);
	Ref<StreamPeerBuffer> texture_stream;
	// BNTX has its BOM at 12, not at 4.
	texture_stream.instantiate();
	texture_stream->set_data_array(texture_data);
	if (texture_data.size() < 68 || texture_stream->get_string(8) != "BNTX" ||
		texture_data[12] != 0xff || texture_data[13] != 0xfe) return ERR_FILE_CORRUPT;
	texture_stream->seek(28);
	if (texture_stream->get_u32() != data_size) return ERR_FILE_CORRUPT;
	NXHeader nx(texture_stream);
	if (nx.Magic != "NX  " || nx.Count != 1 || !has_bytes(texture_stream, nx.InfoPtrAddr, 8)) return ERR_FILE_CORRUPT;
	texture_stream->seek(nx.InfoPtrAddr);
	const uint64_t info_offset = texture_stream->get_u64();
	if (!has_bytes(texture_stream, info_offset, 160)) return ERR_FILE_CORRUPT;
	texture_stream->seek(info_offset);
	BRTInfo info(texture_stream);
	if (info.ArrayLength != sheet_count || info.Width != sheet_width || info.Height != sheet_height) return ERR_FILE_CORRUPT;
	Vector<Ref<Image>> sheets;
	for (int layer = 0; layer < sheet_count; ++layer) {
		Ref<BinaryTexture> texture;
		texture.instantiate();
		Error error = texture->LoadFromEntry(texture_stream, info_offset, layer);
		if (error != OK) return error;
		if (texture->is_compressed()) {
			error = texture->decompress();
			if (error != OK) return error;
		}
		texture->convert(Image::FORMAT_RGBA8);
		PackedByteArray pixels = texture->get_data();
		const Vector4i channels = texture->get_channel_sources();
		for (int c = 0; c < 4; ++c) if (channels[c] < 0 || channels[c] > 5) return ERR_UNAVAILABLE;
		uint8_t *dst = pixels.ptrw();
		for (int64_t i = 0; i < pixels.size(); i += 4) {
			const uint8_t source[] = {0, 255, dst[i], dst[i + 1], dst[i + 2], dst[i + 3]};
			for (int c = 0; c < 4; ++c) dst[i + c] = source[channels[c]];
		}
		Ref<Image> image = Image::create_from_data(sheet_width, sheet_height, false, Image::FORMAT_RGBA8, pixels);
		// NX font sheets are stored upside down (Switch Toolbox GetBitmapFont).
		image->flip_y();
		sheets.push_back(image);
	}

	const int per_sheet = columns * rows;
	for (const KeyValue<uint32_t, uint16_t> &entry : characters) {
		if (entry.value == 0xffff) continue;
		const GlyphWidth *width = widths.getptr(entry.value);
		if (entry.value >= per_sheet * sheet_count || (width ? width->width : default_width.width) > cell_width) return ERR_FILE_CORRUPT;
	}
	clear_cache();
	set_data(PackedByteArray());
	set_fallbacks(TypedArray<Font>());
	set_allow_system_fallback(false);
	set_fixed_size(height);
	set_fixed_size_scale_mode(TextServer::FIXED_SIZE_SCALE_ENABLED);
	set_subpixel_positioning(TextServer::SUBPIXEL_POSITIONING_DISABLED);
	set_cache_ascent(0, height, ascent);
	set_cache_descent(0, height, line_feed - ascent);
	const Vector2i size(height, 0);
	for (int i = 0; i < sheets.size(); ++i) set_texture_image(0, size, i, sheets[i]);
	for (const KeyValue<uint32_t, uint16_t> &entry : characters) {
		const uint32_t code = entry.key;
		const uint16_t index = entry.value;
		if (index == 0xffff || (code >= 0xd800 && code <= 0xdfff) || !BinaryFontUtils::in_ranges(code, ranges)) continue;
		const GlyphWidth *found = widths.getptr(index);
		const GlyphWidth &width = found ? *found : default_width;
		const int cell = index % per_sheet;
		const Vector2 origin((cell % columns) * (cell_width + 1) + 1, (cell / columns) * (cell_height + 1) + 1);
		set_glyph_texture_idx(0, size, code, index / per_sheet);
		set_glyph_uv_rect(0, size, code, Rect2(origin, Vector2(width.width, cell_height)));
		set_glyph_size(0, size, code, Vector2(width.width, cell_height));
		set_glyph_offset(0, size, code, Vector2(width.left, -baseline));
		set_glyph_advance(0, height, code, Vector2(width.advance, 0));
	}
	for (const KeyValue<Vector2i, int16_t> &pair : kerning) {
		set_kerning(0, height, pair.key, Vector2(pair.value, 0));
	}
	return OK;
}

Ref<BinaryFont> BinaryFont::from_bffnt(const String &path)
{
	Ref<BinaryFont> font;
	font.instantiate();
	return font->LoadFromFile(path) == OK ? font : Ref<BinaryFont>();
}

Variant ResourceFormatLoaderBFFNT::_load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const
{
	Ref<BinaryFont> font;
	font.instantiate();
	Error error = font->LoadFromFile(path);
	if (error != OK) return error;
	return font;
}

PackedStringArray ResourceFormatLoaderBFFNT::_get_recognized_extensions() const
{
	PackedStringArray extensions;
	extensions.push_back("bffnt");
	return extensions;
}

bool ResourceFormatLoaderBFFNT::_handles_type(const StringName &type) const
{
	return type == StringName("BinaryFont") || type == StringName("FontFile") || type == StringName("Font");
}

String ResourceFormatLoaderBFFNT::_get_resource_type(const String &path) const
{
	return path.get_extension().to_lower() == "bffnt" ? "BinaryFont" : "";
}
