#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "trmmt_generated.h"
#include <utils.h>

namespace godot {

class MaterialTable : public Resource {
    GDCLASS(MaterialTable, Resource)
protected:
	static void _bind_methods();
public:
	MaterialTable(){}
	~MaterialTable(){}
    
	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(PackedStringArray, MatNames)

private:
    String Name;
    PackedStringArray MatNames;
};

class TRModelMaterialTable : public Resource {
    GDCLASS(TRModelMaterialTable, Resource)
protected:
	static void _bind_methods();
public:
	TRModelMaterialTable(){}
	~TRModelMaterialTable(){}

    void LoadFromFile(String file);

	GETTER_SETTER_DEFINE(Array, MaterialTables)

private:
    Array MaterialTables;
};

class ResourceFormatLoaderTRMMT : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRMMT, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRMMT(){}
	~ResourceFormatLoaderTRMMT(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}