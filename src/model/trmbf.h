#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "trmbf_generated.h"
#include <utils.h>

namespace godot {

class ModelBuffer : public Resource {
	GDCLASS(ModelBuffer, Resource)
protected:
	static void _bind_methods();
public:
	ModelBuffer(){}
	~ModelBuffer(){}

	GETTER_SETTER_DEFINE(Array, IndexBuffers)
    GETTER_SETTER_DEFINE(Array, VertexBuffers)

private:
	Array IndexBuffers;
    Array VertexBuffers;
};

class TRModelBuffer : public Resource {
    GDCLASS(TRModelBuffer, Resource)
protected:
	static void _bind_methods();
public:
	TRModelBuffer(){}
	~TRModelBuffer(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Array, Buffers)

private:
    Array Buffers;
};

class ResourceFormatLoaderTRMBF : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRMBF, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRMBF(){}
	~ResourceFormatLoaderTRMBF(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}