#pragma once

#include "bflyt.h"

namespace godot {

class BinaryLayoutAnimation : public Resource {
	GDCLASS(BinaryLayoutAnimation, Resource)
protected:
	static void _bind_methods();
public:
	void LoadFromBuffer(const PackedByteArray &buffer);
	void LoadFromFile(const String &path);
	Dictionary get_animation() const { return animation.duplicate(true); }
private:
	Dictionary animation;
	bool parse(BflytUtils::Reader &r, Dictionary &result);
	bool parse_pat1(BflytUtils::Reader &r, uint64_t p, uint32_t version, Dictionary &result);
	bool parse_pai1(BflytUtils::Reader &r, uint64_t p, Dictionary &result);
	bool parse_entry(BflytUtils::Reader &r, uint64_t p, Dictionary &entry);
	bool parse_tag(BflytUtils::Reader &r, uint64_t p, uint32_t target, Dictionary &tag);
	bool parse_track(BflytUtils::Reader &r, uint64_t p, Dictionary &track);
};

class ResourceFormatLoaderBFLAN : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderBFLAN, ResourceFormatLoader)
protected:
	static void _bind_methods() {}
public:
	PackedStringArray _get_recognized_extensions() const override;
	String _get_resource_type(const String &path) const override;
	bool _handles_type(const StringName &type) const override;
	Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};
}
