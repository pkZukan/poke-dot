#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "trmdl_generated.h"
#include <utils.h>
#include <common.h>

namespace godot {

class LodIndex : public Resource {
    GDCLASS(LodIndex, Resource)
protected:
	static void _bind_methods();
public:
	LodIndex(){}
	~LodIndex(){}

	GETTER_SETTER_DEFINE(int, unk0)

private:
	int unk0;
};

class LOD : public Resource {
    GDCLASS(LOD, Resource)
protected:
	static void _bind_methods();
public:
	LOD(){}
	~LOD(){}

	GETTER_SETTER_DEFINE(Array, Index)
	GETTER_SETTER_DEFINE(String, Type)

private:
	Array Index;
	String Type;
};

class TRModel : public Resource {
    GDCLASS(TRModel, Resource)
protected:
	static void _bind_methods();
public:
	TRModel(){}
	~TRModel(){}

	void LoadFromFile(String file);

	GETTER_SETTER_DEFINE(int, unk0)
	GETTER_SETTER_DEFINE(PackedStringArray, Meshes)
	GETTER_SETTER_DEFINE(String, Skeleton)
	GETTER_SETTER_DEFINE(PackedStringArray, Materials)
	GETTER_SETTER_DEFINE(Array, LODs)
	GETTER_SETTER_DEFINE(Ref<BoundingBox>, BBox)

private:
	int unk0;
	PackedStringArray Meshes;
	String Skeleton;
	PackedStringArray Materials;
	Array LODs;
	Ref<BoundingBox> BBox;
};

class ResourceFormatLoaderTRMDL : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRMDL, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRMDL(){}
	~ResourceFormatLoaderTRMDL(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}