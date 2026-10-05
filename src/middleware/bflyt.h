#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <vector>

namespace godot {

namespace BflytUtils
{

struct Reader
{
	const PackedByteArray &data;
	uint64_t end;
	bool be = false;
	bool valid = true;

	bool has(uint64_t p, uint64_t n);
	uint32_t number(uint64_t p, unsigned n);
	float real(uint64_t p);
	String string(uint64_t p, uint64_t length, bool terminated = false);
	String cstring(uint64_t p);
	Vector2 vec2(uint64_t p);
	Vector3 vec3(uint64_t p);
};

}

struct BflytSection
{
	String tag;
	uint64_t offset = 0;
	uint64_t size = 0;
};

struct BflytContext
{
	uint32_t version = 0;
	Dictionary layout;
	Array panes;
	Array materials;
	Array sections;
	PackedStringArray textures;
	PackedStringArray fonts;
	PackedStringArray warnings;
	std::vector<int> parents;
	int last_pane = -1;
};

class BinaryLayout : public Resource {
	GDCLASS(BinaryLayout, Resource)

protected:
	static void _bind_methods();

public:
	Error LoadFromBuffer(const PackedByteArray &buffer);
	Error LoadFromFile(const String &path);
	Dictionary get_layout() const { return layout.duplicate(true); }

private:
	Dictionary layout;

	bool parse(BflytUtils::Reader &r, BflytContext &ctx);
	bool parse_section(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx);
	bool parse_lyt1(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx);
	bool parse_strings(BflytUtils::Reader &r, const BflytSection &section, PackedStringArray &strings);
	bool parse_mat1(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx);
	bool parse_material(BflytUtils::Reader &r, uint64_t offset, uint32_t version, Dictionary &material);
	bool parse_pas1(BflytContext &ctx);
	bool parse_pae1(BflytContext &ctx);
	bool parse_pane(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx);
	bool parse_pane_payload(BflytUtils::Reader &r, const BflytSection &section, BflytContext &ctx, Dictionary &pane);
	bool parse_pic1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane);
	bool parse_wnd1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane);
	bool parse_prt1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane);
	bool parse_txt1(BflytUtils::Reader &r, const BflytSection &section, Dictionary &pane);
	bool parse_unrendered_pane(const BflytSection &section, BflytContext &ctx);
	bool validate_references(const BflytContext &ctx);
};

class ResourceFormatLoaderBFLYT : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderBFLYT, ResourceFormatLoader)
protected:
	static void _bind_methods() {}
public:
	PackedStringArray _get_recognized_extensions() const override;
	String _get_resource_type(const String &path) const override;
	bool _handles_type(const StringName &type) const override;
	Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};
}
