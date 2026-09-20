#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include "middleware/havok_tag.h"
#include "utils.h"

#include <vector>
#include <string>
#include <utility>
#include <cmath>
#include <cstring>
#include <algorithm>

namespace godot {

class TRCOL : public Resource {
	GDCLASS(TRCOL, Resource)

protected:
	static void _bind_methods();

public:
	TRCOL() {}
	~TRCOL() {}

	void LoadFromFile(String file);

	Ref<HavokTag> get_tag() { return tag; }

private:
	Ref<HavokTag> tag;
};

class ResourceFormatLoaderTRCOL : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRCOL, ResourceFormatLoader)

protected:
	static void _bind_methods() {}

public:
	ResourceFormatLoaderTRCOL() {}
	~ResourceFormatLoaderTRCOL() {}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

} // namespace godot