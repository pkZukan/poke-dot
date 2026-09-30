#include "bflan.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/error_macros.hpp>

using namespace godot;

void BinaryLayoutAnimation::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("LoadFromBuffer", "buffer"), &BinaryLayoutAnimation::LoadFromBuffer);
	ClassDB::bind_method(D_METHOD("LoadFromFile", "path"), &BinaryLayoutAnimation::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_animation"), &BinaryLayoutAnimation::get_animation);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "animation", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_animation");
}

void BinaryLayoutAnimation::LoadFromBuffer(const PackedByteArray &buffer)
{
	Dictionary result;
	BflytUtils::Reader reader{buffer, uint64_t(buffer.size())};
	if (!parse(reader, result)) return;
	animation = result;
}

void BinaryLayoutAnimation::LoadFromFile(const String &path)
{
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	ERR_FAIL_COND_MSG(file.is_null(), "Could not open " + path);

	LoadFromBuffer(file->get_buffer(file->get_length()));
}

static bool read_offsets(BflytUtils::Reader &r, uint64_t base, uint64_t table, uint32_t count,
	uint64_t minimum, std::vector<uint64_t> &offsets)
{
	ERR_FAIL_COND_V_MSG(!r.has(table, uint64_t(count) * 4), false, "Truncated BFLAN offset table");
	for (uint32_t i = 0; i < count; ++i)
	{
		uint64_t offset = r.number(table + i * 4, 4);
		ERR_FAIL_COND_V_MSG(offset < minimum || !r.has(base + offset, 1), false, "Invalid BFLAN offset");
		offsets.push_back(base + offset);
	}
	return true;
}

bool BinaryLayoutAnimation::parse(BflytUtils::Reader &r, Dictionary &result)
{
	ERR_FAIL_COND_V_MSG(!r.has(0, 20) || r.string(0, 4) != "FLAN", false, "Missing FLAN header");
	if (r.data[4] == 0xfe && r.data[5] == 0xff) r.be = true;
	else ERR_FAIL_COND_V_MSG(r.data[4] != 0xff || r.data[5] != 0xfe, false, "Invalid BFLAN byte-order mark");
	uint32_t version = r.number(8, 4);
	ERR_FAIL_COND_V_MSG(version < 0x05000000 || version >= 0x0a000000, false, "Unsupported BFLAN version (expected 5.x through 9.x)");
	uint64_t p = r.number(6, 2), file_end = r.end;
	ERR_FAIL_COND_V_MSG(p < 20 || p > file_end || r.number(12, 4) != file_end, false, "Invalid BFLAN header/file size");
	uint32_t count = r.number(16, 2);
	result["version"] = version;
	result["big_endian"] = r.be;
	Array sections;
	PackedStringArray unsupported;
	for (uint32_t i = 0; i < count; ++i)
	{
		r.end = file_end;
		ERR_FAIL_COND_V_MSG(!r.has(p, 8), false, "Truncated BFLAN section");
		String tag = r.string(p, 4);
		uint64_t size = r.number(p + 4, 4);
		ERR_FAIL_COND_V_MSG(size < 8 || size % 4 || !r.has(p, size), false, "Invalid BFLAN section size");
		r.end = p + size;
		Dictionary section;
		section["tag"] = tag;
		section["offset"] = int64_t(p);
		section["data"] = r.data.slice(p, r.end);
		if (tag == "pat1")
		{
			ERR_FAIL_COND_V_MSG(result.has("pat1"), false, "Duplicate pat1");
			Dictionary metadata;
			if (!parse_pat1(r, p, version, metadata)) return false;
			result["pat1"] = metadata;
		}
		else if (tag == "pai1")
		{
			ERR_FAIL_COND_V_MSG(result.has("pai1"), false, "Duplicate pai1");
			Dictionary content;
			if (!parse_pai1(r, p, content)) return false;
			result["pai1"] = content;
		}
		else if (!unsupported.has(tag)) unsupported.push_back(tag);
		ERR_FAIL_COND_V_MSG(!r.valid, false, "Invalid BFLAN field in " + tag);
		sections.push_back(section);
		p += size;
	}
	ERR_FAIL_COND_V_MSG(p != file_end || !result.has("pai1"), false, "Invalid BFLAN section count or missing pai1");
	result["sections"] = sections;
	result["unsupported_sections"] = unsupported;
	return true;
}

bool BinaryLayoutAnimation::parse_pat1(BflytUtils::Reader &r, uint64_t p, uint32_t version, Dictionary &result)
{
	uint64_t extra = version >= 0x08000000 ? 4 : 0, header = 28 + extra;
	ERR_FAIL_COND_V_MSG(!r.has(p, header), false, "Truncated pat1");
	uint32_t count = r.number(p + 10, 2);
	uint64_t name = r.number(p + 12, 4), groups = r.number(p + 16, 4);
	ERR_FAIL_COND_V_MSG(name < header || (count && groups < header) || !r.has(p + groups, uint64_t(count) * 28), false, "Invalid pat1 offsets");
	result["order"] = r.number(p + 8, 2);
	result["name"] = r.cstring(p + name);
	if (extra) result["unknown"] = r.number(p + 20, 4);
	result["start_frame"] = int16_t(r.number(p + 20 + extra, 2));
	result["end_frame"] = int16_t(r.number(p + 22 + extra, 2));
	result["child_binding"] = bool(r.number(p + 24 + extra, 1));
	PackedStringArray names;
	for (uint32_t i = 0; i < count; ++i) names.push_back(r.string(p + groups + i * 28, 28));
	result["groups"] = names;
	return r.valid;
}

bool BinaryLayoutAnimation::parse_pai1(BflytUtils::Reader &r, uint64_t p, Dictionary &result)
{
	ERR_FAIL_COND_V_MSG(!r.has(p, 20), false, "Truncated pai1");
	result["frame_count"] = r.number(p + 8, 2);
	result["loop"] = bool(r.number(p + 10, 1));
	uint32_t textures = r.number(p + 12, 2), count = r.number(p + 14, 2);
	uint64_t table = r.number(p + 16, 4);
	std::vector<uint64_t> offsets;
	if (!read_offsets(r, p + 20, p + 20, textures, uint64_t(textures) * 4, offsets)) return false;
	PackedStringArray names;
	for (uint64_t offset : offsets) names.push_back(r.cstring(offset));
	result["textures"] = names;
	offsets.clear();
	ERR_FAIL_COND_V_MSG(count && table < 20 + uint64_t(textures) * 4, false, "Invalid pai1 entry table");
	if (!read_offsets(r, p, p + table, count, table + uint64_t(count) * 4, offsets)) return false;
	Array entries;
	for (uint64_t offset : offsets)
	{
		Dictionary entry;
		if (!parse_entry(r, offset, entry)) return false;
		entries.push_back(entry);
	}
	result["entries"] = entries;
	return r.valid;
}

bool BinaryLayoutAnimation::parse_entry(BflytUtils::Reader &r, uint64_t p, Dictionary &entry)
{
	ERR_FAIL_COND_V_MSG(!r.has(p, 32), false, "Truncated BFLAN entry");
	entry["name"] = r.string(p, 28);
	uint32_t count = r.number(p + 28, 1), target = r.number(p + 29, 1);
	entry["target_type"] = target;
	std::vector<uint64_t> offsets;
	if (!read_offsets(r, p, p + 32, count, 32 + uint64_t(count) * 4, offsets)) return false;
	Array tags;
	for (uint64_t offset : offsets)
	{
		Dictionary tag;
		if (!parse_tag(r, offset, target, tag)) return false;
		tags.push_back(tag);
	}
	entry["tags"] = tags;
	return r.valid;
}

bool BinaryLayoutAnimation::parse_tag(BflytUtils::Reader &r, uint64_t p, uint32_t target, Dictionary &tag)
{
	if (target == 2)
	{
		tag["unknown"] = r.number(p, 4);
		p += 4;
	}
	ERR_FAIL_COND_V_MSG(!r.has(p, 8), false, "Truncated BFLAN tag");
	tag["type"] = r.string(p, 4);
	uint32_t count = r.number(p + 4, 1);
	std::vector<uint64_t> offsets;
	if (!read_offsets(r, p, p + 8, count, 8 + uint64_t(count) * 4, offsets)) return false;
	Array tracks;
	for (uint64_t offset : offsets)
	{
		Dictionary track;
		if (!parse_track(r, offset, track)) return false;
		tracks.push_back(track);
	}
	tag["tracks"] = tracks;
	return r.valid;
}

bool BinaryLayoutAnimation::parse_track(BflytUtils::Reader &r, uint64_t p, Dictionary &track)
{
	ERR_FAIL_COND_V_MSG(!r.has(p, 12), false, "Truncated BFLAN track");
	track["index"] = r.number(p, 1);
	track["target"] = r.number(p + 1, 1);
	uint32_t curve = r.number(p + 2, 1), count = r.number(p + 4, 2);
	track["curve_type"] = curve;
	uint64_t offset = r.number(p + 8, 4);
	// Unknown curves retain their bytes in the enclosing raw section.
	track["supported"] = curve == 1 || curve == 2;
	if (curve != 1 && curve != 2) return r.valid;
	uint64_t stride = curve == 2 ? 12 : 8;
	ERR_FAIL_COND_V_MSG(offset < 12 || !r.has(p + offset, uint64_t(count) * stride), false, "Invalid BFLAN keyframe range");
	Array keys;
	for (uint32_t i = 0; i < count; ++i)
	{
		uint64_t k = p + offset + i * stride;
		Dictionary key;
		key["frame"] = r.real(k);
		if (curve == 2)
		{
			key["value"] = r.real(k + 4);
			key["slope"] = r.real(k + 8);
		}
		else key["value"] = r.number(k + 4, 2);
		keys.push_back(key);
	}
	track["keyframes"] = keys;
	return r.valid;
}

String ResourceFormatLoaderBFLAN::_get_resource_type(const String &path) const
{
	return path.get_extension().to_lower() == "bflan" ? "BinaryLayoutAnimation" : "";
}

Variant ResourceFormatLoaderBFLAN::_load(const String &path, const String &, bool, int32_t) const
{
	Ref<BinaryLayoutAnimation> bflan;
	bflan.instantiate();
	bflan->LoadFromFile(path);
	return bflan;
}

PackedStringArray ResourceFormatLoaderBFLAN::_get_recognized_extensions() const
{
	PackedStringArray extensions;
	extensions.push_back("bflan");
	return extensions;
}

bool ResourceFormatLoaderBFLAN::_handles_type(const StringName &type) const
{
	return type == StringName("BinaryLayoutAnimation");
}
