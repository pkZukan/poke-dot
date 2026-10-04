#pragma once

#include "bffnt.h"
#include <godot_cpp/templates/hash_set.hpp>

namespace godot {

class BinaryCompositeFont : public FontFile {
	GDCLASS(BinaryCompositeFont, FontFile)
protected:
	static void _bind_methods();
public:
	Error LoadFromFile(const String &path);
	Error LoadFromBuffer(const PackedByteArray &buffer, const String &base_dir);
	Ref<BinaryCompositeFont> load_bfcpx(const String &path);
	PackedStringArray get_warnings() const { return warnings; }
private:
	struct Entry {
		String name;
		Vector<Vector2i> ranges;
		bool outline = false;
		float size = 0;
		float scale = 1;
		float advance_scale = 1;
		float adjustment = 0;
		uint32_t flags = 0;
	};
	PackedStringArray warnings;
	Error parse_node(const Ref<StreamPeerBuffer> &stream, uint64_t offset, Vector<Entry> &entries, HashSet<uint64_t> &active, int depth);
	bool read_name(const Ref<StreamPeerBuffer> &stream, uint64_t offset, String &name);
	bool read_ranges(const Ref<StreamPeerBuffer> &stream, uint64_t offset, uint32_t count, Vector<Vector2i> &ranges);
	Error load_outline(const String &path, const Entry &entry, Ref<FontFile> &font);
};

class ResourceFormatLoaderBFCPX : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderBFCPX, ResourceFormatLoader)
protected:
	static void _bind_methods() {}
public:
	PackedStringArray _get_recognized_extensions() const override;
	String _get_resource_type(const String &path) const override;
	bool _handles_type(const StringName &type) const override;
	Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};

}
