#pragma once

#include <godot_cpp/classes/font_file.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

namespace BinaryFontUtils {
bool has_bytes(const Ref<StreamPeerBuffer> &stream, uint64_t offset, uint64_t length);
Ref<StreamPeerBuffer> open_stream(const PackedByteArray &data);
bool in_ranges(uint32_t code, const Vector<Vector2i> &ranges);
}

class BinaryFont : public FontFile {
	GDCLASS(BinaryFont, FontFile)
protected:
	static void _bind_methods();
public:
	Error LoadFromFile(const String &path);
	Error LoadFromBuffer(const PackedByteArray &buffer);
	Error load_bffnt(const String &path) { return LoadFromFile(path); }
	Error load_bffnt_filtered(const String &path, const Vector<Vector2i> &ranges);
	Ref<BinaryFont> from_bffnt(const String &path);
private:
	struct GlyphWidth {
		int8_t left = 0;
		uint8_t width = 0;
		int8_t advance = 0;
	};
	Error parse(const PackedByteArray &buffer, const Vector<Vector2i> &ranges);
};

class ResourceFormatLoaderBFFNT : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderBFFNT, ResourceFormatLoader)
protected:
	static void _bind_methods() {}
public:
	PackedStringArray _get_recognized_extensions() const override;
	String _get_resource_type(const String &path) const override;
	bool _handles_type(const StringName &type) const override;
	Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};

}
