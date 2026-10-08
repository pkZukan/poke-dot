#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "generated/trins_generated.h"
#include <utils.h>

namespace godot {

class TRINS : public Resource {
	GDCLASS(TRINS, Resource)

protected:
	static void _bind_methods();

public:
	TRINS() {}
	~TRINS() {}

	void LoadFromFile(String file);
	GETTER_SETTER_DEFINE(TypedArray<Transform3D>, transforms)

    TypedArray<Transform3D> transforms;
};

class ResourceFormatLoaderTRINS : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRINS, ResourceFormatLoader)

protected:
	static void _bind_methods() {}

public:
	ResourceFormatLoaderTRINS() {}
	~ResourceFormatLoaderTRINS() {}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

} // namespace godot
