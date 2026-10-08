#include "bflyt.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <cmath>
#include <cstring>

using namespace godot;

void BinaryLayout::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("LoadFromBuffer", "buffer"), &BinaryLayout::LoadFromBuffer);
	ClassDB::bind_method(D_METHOD("LoadFromFile", "path"), &BinaryLayout::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_layout"), &BinaryLayout::get_layout);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "layout", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_layout");
}

// ---------------- Loading / chunk parsing ----------------

Error BinaryLayout::LoadFromBuffer(const PackedByteArray &buffer)
{
	layout.clear();
	BflytContext ctx;
	BflytUtils::Reader reader{buffer, uint64_t(buffer.size())};
	if (!parse(reader, ctx)) return ERR_FILE_CORRUPT;
	layout = ctx.layout;
	return OK;
}

Error BinaryLayout::LoadFromFile(const String &path)
{
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	if (file.is_null())
	{
		layout.clear();
	}
	ERR_FAIL_COND_V_MSG(file.is_null(), ERR_FILE_CANT_OPEN, "Could not open " + path);
	return LoadFromBuffer(file->get_buffer(file->get_length()));
}

bool BinaryLayout::parse(BflytUtils::Reader &r, BflytContext &ctx)
{
	ERR_FAIL_COND_V_MSG(!r.has(0, 20) || r.string(0, 4) != "FLYT", false, "Missing FLYT header");
	if (r.data[4] == 0xfe && r.data[5] == 0xff) r.be = true;
	else
	{
		ERR_FAIL_COND_V_MSG(r.data[4] != 0xff || r.data[5] != 0xfe, false, "Invalid byte-order mark");
	}
	ctx.version = r.number(8, 4);
	ERR_FAIL_COND_V_MSG(ctx.version < 0x05000000 || ctx.version >= 0x0a000000, false, "Unsupported BFLYT version (expected 5.x through 9.x)");
	uint64_t p = r.number(6, 2);
	ERR_FAIL_COND_V_MSG(p < 20 || p > r.end || r.number(12, 4) != r.end, false, "Invalid header/file size");
	const uint32_t count = r.number(16, 2);
	const uint64_t file_end = r.end;
	ctx.layout["version"] = ctx.version;
	ctx.layout["big_endian"] = r.be;
	for (uint32_t i = 0; i < count; ++i)
	{
		r.end = file_end;
		ERR_FAIL_COND_V_MSG(!r.has(p, 8), false, "Truncated section header");
		BflytSection section;
		section.tag = r.string(p, 4);
		section.offset = p;
		section.size = r.number(p + 4, 4);
		ERR_FAIL_COND_V_MSG(section.size < 8 || section.size % 4 || !r.has(p, section.size), false, "Invalid section size: " + section.tag);
		r.end = p + section.size;
		if (!parse_section(r, section, ctx)) return false;
		ERR_FAIL_COND_V_MSG(!r.valid, false, "Invalid field or offset in " + section.tag);
		p += section.size;
	}
	ERR_FAIL_COND_V_MSG(p != file_end || !ctx.parents.empty(), false, "Section count or pane hierarchy mismatch");
	if (!validate_references(ctx)) return false;
	ctx.layout["panes"] = ctx.panes;
	ctx.layout["materials"] = ctx.materials;
	ctx.layout["textures"] = ctx.textures;
	ctx.layout["fonts"] = ctx.fonts;
	ctx.layout["sections"] = ctx.sections;
	ctx.layout["unrendered_pane_types"] = ctx.warnings;
	return true;
}

bool BinaryLayout::parse_section(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx)
{
	Dictionary raw;
	raw["tag"] = section.tag;
	raw["offset"] = int64_t(section.offset);
	raw["data"] = r.data.slice(section.offset, section.offset + section.size);
	ctx.sections.push_back(raw);

	const String &tag = section.tag;
	if (tag == "lyt1") return parse_lyt1(r, section, ctx);
	if (tag == "txl1") return parse_strings(r, section, ctx.textures);
	if (tag == "fnl1") return parse_strings(r, section, ctx.fonts);
	if (tag == "mat1") return parse_mat1(r, section, ctx);
	if (tag == "pas1") return parse_pas1(ctx);
	if (tag == "pae1") return parse_pae1(ctx);
	if (tag == "pan1" || tag == "pic1" || tag == "txt1" || tag == "wnd1" || tag == "bnd1" ||
		tag == "prt1" || tag == "ali1" || tag == "scr1" || tag == "cpt1")
		return parse_pane(r, section, ctx);
	// Preserve unsupported sections for inspection and future parsers.
	return true;
}

bool BinaryLayout::parse_lyt1(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx)
{
	const uint64_t p = section.offset;
	ERR_FAIL_COND_V_MSG(!r.has(p + 8, 20), false, "Truncated lyt1");
	ctx.layout["draw_from_center"] = bool(r.number(p + 8, 1));
	ctx.layout["size"] = r.vec2(p + 12);
	ctx.layout["parts_size"] = r.vec2(p + 20);
	ctx.layout["name"] = r.cstring(p + 28);
	return true;
}

bool BinaryLayout::parse_strings(BflytUtils::Reader &r, const BflytSection &section, PackedStringArray &strings)
{
	const uint64_t p = section.offset;
	const uint32_t count = r.number(p + 8, 2);
	ERR_FAIL_COND_V_MSG(!r.has(p + 12, uint64_t(count) * 4), false, "Truncated offset table: " + section.tag);
	for (uint32_t i = 0; i < count && r.valid; ++i)
	{
		uint64_t offset = r.number(p + 12 + i * 4, 4);
		ERR_FAIL_COND_V_MSG(offset < uint64_t(count) * 4, false, "String overlaps offset table");
		strings.push_back(r.cstring(p + 12 + offset));
	}
	return true;
}

bool BinaryLayout::parse_mat1(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx)
{
	const uint64_t p = section.offset;
	const uint32_t count = r.number(p + 8, 2);
	ERR_FAIL_COND_V_MSG(!r.has(p + 12, uint64_t(count) * 4), false, "Truncated offset table: " + section.tag);
	for (uint32_t i = 0; i < count && r.valid; ++i)
	{
		uint64_t offset = r.number(p + 12 + i * 4, 4);
		uint64_t next = i + 1 < count ? r.number(p + 16 + i * 4, 4) : section.size;
		ERR_FAIL_COND_V_MSG(offset < 12 + uint64_t(count) * 4 || next < offset || next > section.size, false, "Invalid material offsets");
		uint64_t saved_end = r.end;
		r.end = p + next;
		Dictionary material;
		bool parsed = parse_material(r, p + offset, ctx.version, material);
		r.end = saved_end;
		if (!parsed) return false;
		ctx.materials.push_back(material);
	}
	return true;
}

bool BinaryLayout::parse_material(BflytUtils::Reader &r, uint64_t offset, uint32_t version, Dictionary &material)
{
	const uint32_t header = version >= 0x08000000 ? 44 : 40;
	ERR_FAIL_COND_V_MSG(!r.has(offset, header), false, "Truncated material");
	material["name"] = r.string(offset, 28);
	uint32_t flags = r.number(offset + (version >= 0x08000000 ? 28 : 36), 4);
	material["flags"] = flags;
	const uint64_t colors = offset + (version >= 0x08000000 ? 36 : 28);
	for (int i = 0; i < 2; ++i) {
		uint64_t c = colors + i * 4;
		material[i == 0 ? "black_color" : "white_color"] = Color(r.number(c, 1) / 255.0, r.number(c + 1, 1) / 255.0, r.number(c + 2, 1) / 255.0, r.number(c + 3, 1) / 255.0);
	}
	Array maps;
	for (uint32_t i = 0; i < (flags & 3); ++i)
	{
		uint64_t p = offset + header + i * 4;
		Dictionary map;
		map["texture_index"] = r.number(p, 2);
		map["wrap_s"] = r.number(p + 2, 1);
		map["wrap_t"] = r.number(p + 3, 1);
		maps.push_back(map);
	}
	material["texture_maps"] = maps;
	Array transforms;
	for (uint32_t i = 0; i < ((flags >> 2) & 3); ++i) {
		uint64_t p = offset + header + (flags & 3) * 4 + i * 20;
		Dictionary transform;
		transform["translation"] = r.vec2(p);
		transform["rotation"] = r.real(p + 8);
		transform["scale"] = r.vec2(p + 12);
		transforms.push_back(transform);
	}
	material["texture_transforms"] = transforms;
	const bool has_color_blend_mode = flags & (1u << 10);
	const uint32_t later_material_data = flags & 0xFFFFE800u;
	if (has_color_blend_mode && later_material_data == 0 && r.has(r.end - 4, 4)) {
		Dictionary blend_mode;
		blend_mode["equation"] = r.number(r.end - 4, 1);
		blend_mode["source"] = r.number(r.end - 3, 1);
		blend_mode["destination"] = r.number(r.end - 2, 1);
		blend_mode["logic_operation"] = r.number(r.end - 1, 1);
		material["color_blend_mode"] = blend_mode;
	}
	material["raw_data"] = r.data.slice(offset, r.end);
	return true;
}

bool BinaryLayout::parse_pas1(BflytContext &ctx)
{
	ERR_FAIL_COND_V_MSG(ctx.last_pane < 0, false, "pas1 without a preceding pane");
	ctx.parents.push_back(ctx.last_pane);
	ctx.last_pane = -1;
	return true;
}

bool BinaryLayout::parse_pae1(BflytContext &ctx)
{
	ERR_FAIL_COND_V_MSG(ctx.parents.empty(), false, "Unmatched pae1");
	ctx.parents.pop_back();
	ctx.last_pane = -1;
	return true;
}

bool BinaryLayout::parse_pane(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx)
{
	const uint64_t p = section.offset;
	ERR_FAIL_COND_V_MSG(!r.has(p, 84), false, "Truncated pane: " + section.tag);
	Dictionary pane;
	pane["type"] = section.tag;
	pane["name"] = r.string(p + 12, 24);
	pane["user_name"] = r.string(p + 36, 8);
	pane["flags_ex"] = r.number(p + 11, 1);
	pane["parent"] = ctx.parents.empty() ? -1 : ctx.parents.back();
	pane["flags"] = r.number(p + 8, 1);
	pane["origin"] = r.number(p + 9, 1);
	pane["alpha"] = r.number(p + 10, 1);
	pane["translation"] = r.vec3(p + 44);
	pane["rotation"] = r.vec3(p + 56);
	pane["scale"] = r.vec2(p + 68);
	pane["size"] = r.vec2(p + 76);
	if (!parse_pane_payload(r, section, ctx, pane)) return false;
	pane["raw_data"] = r.data.slice(p, p + section.size);
	ctx.last_pane = ctx.panes.size();
	ctx.panes.push_back(pane);
	return true;
}

bool BinaryLayout::parse_pane_payload(BflytUtils::Reader &r, const BflytSection &section,
		BflytContext &ctx, Dictionary &pane)
{
	if (section.tag == "pic1") return parse_pic1(r, section, pane);
	if (section.tag == "wnd1") return parse_wnd1(r, section, pane);
	if (section.tag == "prt1") return parse_prt1(r, section, pane);
	if (section.tag == "txt1") return parse_txt1(r, section, pane);
	if (section.tag == "ali1" || section.tag == "scr1" || section.tag == "cpt1")
		return parse_unrendered_pane(section, ctx);
	return true;
}

bool BinaryLayout::parse_prt1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane)
{
	const uint64_t p = section.offset;
	ERR_FAIL_COND_V_MSG(!r.has(p + 84, 12), false, "Truncated parts pane");
	const uint32_t count = r.number(p + 84, 4);
	ERR_FAIL_COND_V_MSG(!r.has(p + 96, uint64_t(count) * 40 + 1), false, "Truncated parts table");
	pane["parts_scale"] = r.vec2(p + 88);
	pane["parts_name"] = r.cstring(p + 96 + uint64_t(count) * 40);
	pane["parts_override_count"] = count;
	return true;
}

bool BinaryLayout::parse_txt1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane)
{
	const uint64_t p = section.offset;
	ERR_FAIL_COND_V_MSG(!r.has(p + 84, 44), false, "Truncated text pane");
	pane["text_alignment"] = r.number(p + 92, 1);
	pane["font_size"] = r.vec2(p + 112);
	pane["font_index"] = r.number(p + 90, 2);
	const uint64_t c = p + 104;
	pane["text_color"] = Color(r.number(c, 1) / 255.0, r.number(c + 1, 1) / 255.0, r.number(c + 2, 1) / 255.0, r.number(c + 3, 1) / 255.0);
	const uint32_t offset = r.number(p + 100, 4);
	String text;
	if (offset) {
		ERR_FAIL_COND_V_MSG(offset < 128 || !r.has(p + offset, 2), false, "Invalid text offset");
		std::vector<char16_t> characters;
		for (uint64_t at = p + offset; ; at += 2) {
			ERR_FAIL_COND_V_MSG(!r.has(at, 2), false, "Unterminated text");
			uint32_t character = r.number(at, 2);
			if (!character) break;
			characters.push_back(char16_t(character));
		}
		characters.push_back(0);
		text = String::utf16(characters.data());
	}
	pane["text"] = text;
	return true;
}

bool BinaryLayout::parse_wnd1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane)
{
	const uint64_t p = section.offset;
	// WND1 stores four stretch insets, four custom insets, frame metadata,
	// then relative offsets to its content region and frame table.
	ERR_FAIL_COND_V_MSG(!r.has(p + 84, 28), false, "Truncated window pane");
	PackedInt32Array stretch;
	PackedInt32Array custom_insets;
	for (int i = 0; i < 4; ++i) stretch.push_back(r.number(p + 84 + i * 2, 2));
	for (int i = 0; i < 4; ++i) custom_insets.push_back(r.number(p + 92 + i * 2, 2));
	const uint32_t frame_count = r.number(p + 100, 1);
	const uint32_t flags = r.number(p + 101, 1);
	const uint32_t content_offset = r.number(p + 104, 4);
	const uint32_t frame_table_offset = r.number(p + 108, 4);
	ERR_FAIL_COND_V_MSG(content_offset < 112 || !r.has(p + content_offset, 20), false,
		"Invalid window content offset");

	pane["window_stretch"] = stretch;
	pane["window_custom_insets"] = custom_insets;
	pane["window_flags"] = flags;
	pane["window_frame_count"] = frame_count;

	const uint64_t content = p + content_offset;
	PackedColorArray colors;
	for (int i = 0; i < 4; ++i)
	{
		const uint64_t color = content + i * 4;
		colors.push_back(Color(r.number(color, 1) / 255.0, r.number(color + 1, 1) / 255.0,
			r.number(color + 2, 1) / 255.0, r.number(color + 3, 1) / 255.0));
	}
	pane["colors"] = colors;
	pane["material_index"] = r.number(content + 16, 2);
	const uint32_t uv_count = r.number(content + 18, 1);
	ERR_FAIL_COND_V_MSG(!r.has(content + 20, uint64_t(uv_count) * 32), false,
		"Truncated window content UVs");
	Array uv_sets;
	for (uint32_t i = 0; i < uv_count; ++i)
	{
		PackedVector2Array uv;
		for (int vertex = 0; vertex < 4; ++vertex)
			uv.push_back(r.vec2(content + 20 + i * 32 + vertex * 8));
		uv_sets.push_back(uv);
	}
	pane["uv_sets"] = uv_sets;

	Array frames;
	if (frame_count)
	{
		ERR_FAIL_COND_V_MSG(frame_table_offset < 112 ||
			!r.has(p + frame_table_offset, uint64_t(frame_count) * 4), false,
			"Invalid window frame table offset");
		for (uint32_t i = 0; i < frame_count; ++i)
		{
			const uint64_t offset_entry = p + frame_table_offset + uint64_t(i) * 4;
			const uint32_t frame_offset = r.number(offset_entry, 4);
			ERR_FAIL_COND_V_MSG(frame_offset < 112 || !r.has(p + frame_offset, 4), false,
				"Invalid window frame offset");
			Dictionary frame;
			frame["material_index"] = r.number(p + frame_offset, 2);
			frame["texture_flip"] = r.number(p + frame_offset + 2, 1);
			frames.push_back(frame);
		}
	}
	pane["window_frames"] = frames;
	return true;
}

bool BinaryLayout::parse_unrendered_pane(const BflytSection &section, BflytContext &ctx)
{
	if (!ctx.warnings.has(section.tag)) ctx.warnings.push_back(section.tag);
	return true;
}

bool BinaryLayout::parse_pic1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane)
{
	const uint64_t p = section.offset;
	PackedColorArray colors;
	for (int k = 0; k < 4; ++k)
	{
		uint64_t c = p + 84 + k * 4;
		colors.push_back(Color(r.number(c, 1) / 255.0, r.number(c + 1, 1) / 255.0, r.number(c + 2, 1) / 255.0, r.number(c + 3, 1) / 255.0));
	}
	pane["colors"] = colors;
	pane["material_index"] = r.number(p + 100, 2);
	uint32_t n = r.number(p + 102, 1);
	Array sets;
	ERR_FAIL_COND_V_MSG(!r.has(p + 104, uint64_t(n) * 32), false, "Truncated picture UVs");
	for (uint32_t k = 0; k < n; ++k)
	{
		PackedVector2Array uv;
		for (int v = 0; v < 4; ++v) uv.push_back(r.vec2(p + 104 + k * 32 + v * 8));
		sets.push_back(uv);
	}
	pane["uv_sets"] = sets;
	return true;
}

bool BinaryLayout::validate_references(const BflytContext &ctx)
{
	for (int i = 0; i < ctx.materials.size(); ++i)
	{
		Dictionary material = ctx.materials[i];
		Array maps = material["texture_maps"];
		for (int j = 0; j < maps.size(); ++j)
		{
			Dictionary map = maps[j];
			ERR_FAIL_COND_V_MSG(int(map["texture_index"]) >= ctx.textures.size(), false, "Invalid material texture index");
		}
	}
	for (int i = 0; i < ctx.panes.size(); ++i)
	{
		Dictionary pane = ctx.panes[i];
		ERR_FAIL_COND_V_MSG(pane.has("material_index") && int(pane["material_index"]) >= ctx.materials.size(), false, "Invalid pane material index");
		if (pane.has("window_frames"))
		{
			Array frames = pane["window_frames"];
			for (int j = 0; j < frames.size(); ++j)
			{
				Dictionary frame = frames[j];
				ERR_FAIL_COND_V_MSG(int(frame["material_index"]) >= ctx.materials.size(), false,
					"Invalid window frame material index");
			}
		}
	}
	return true;
}

// ---------------- Resource loader ----------------

String ResourceFormatLoaderBFLYT::_get_resource_type(const String &path) const
{
	return path.get_extension().to_lower() == "bflyt" ? "BinaryLayout" : "";
}

Variant ResourceFormatLoaderBFLYT::_load(const String &path, const String &, bool, int32_t) const
{
	Ref<BinaryLayout> bflyt;
	bflyt.instantiate();
	Error error = bflyt->LoadFromFile(path);
	if (error != OK) return error;
	return bflyt;
}

PackedStringArray ResourceFormatLoaderBFLYT::_get_recognized_extensions() const
{
	PackedStringArray extensions;
	extensions.push_back("bflyt");
	return extensions;
}

bool ResourceFormatLoaderBFLYT::_handles_type(const StringName &type) const
{
	return type == StringName("BinaryLayout");
}

// ---------------- Shared utils ----------------

bool BflytUtils::Reader::has(uint64_t p, uint64_t n)
{
	if (p > end || n > end - p) valid = false;
	return valid;
}

uint32_t BflytUtils::Reader::number(uint64_t p, unsigned n)
{
	if (!has(p, n)) return 0;
	uint32_t v = 0;
	for (unsigned i = 0; i < n; ++i) v |= uint32_t(data[p + i]) << (8 * (be ? n - i - 1 : i));
	return v;
}

float BflytUtils::Reader::real(uint64_t p)
{
	uint32_t bits = number(p, 4);
	float v;
	std::memcpy(&v, &bits, 4);
	if (!std::isfinite(v)) valid = false;
	return v;
}

String BflytUtils::Reader::string(uint64_t p, uint64_t length, bool terminated)
{
	if (!has(p, length)) return {};
	uint64_t n = 0;
	while (n < length && data[p + n]) ++n;
	if (terminated && n == length) valid = false;
	return String::utf8(reinterpret_cast<const char *>(data.ptr() + p), n);
}

String BflytUtils::Reader::cstring(uint64_t p)
{
	if (!has(p, 1)) return {};
	return string(p, end - p, true);
}

Vector2 BflytUtils::Reader::vec2(uint64_t p)
{
	float x = real(p);
	float y = real(p + 4);
	return Vector2(x, y);
}

Vector3 BflytUtils::Reader::vec3(uint64_t p)
{
	float x = real(p);
	float y = real(p + 4);
	float z = real(p + 8);
	return Vector3(x, y, z);
}
