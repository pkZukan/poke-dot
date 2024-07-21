#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "trskl_generated.h"
#include <utils.h>

namespace godot {

class TransformNode : public Resource {
	GDCLASS(TransformNode, Resource)
protected:
	static void _bind_methods();
public:
	TransformNode(){}
	~TransformNode(){}

	GETTER_SETTER_DEFINE(String, Name)

private:
	String Name;
};

class IKControl : public Resource {
	GDCLASS(IKControl, Resource)
protected:
	static void _bind_methods();
public:
	IKControl(){}
	~IKControl(){}

};

class BoneMatrix : public Resource {
	GDCLASS(BoneMatrix, Resource)
protected:
	static void _bind_methods();
public:
	BoneMatrix(){}
	~BoneMatrix(){}

    GETTER_SETTER_DEFINE(Vector3, X)
    GETTER_SETTER_DEFINE(Vector3, Y)
	GETTER_SETTER_DEFINE(Vector3, Z)
	GETTER_SETTER_DEFINE(Vector3, W)

private:
    Vector3 X, Y, Z, W;
};

class BoneEntry : public Resource {
	GDCLASS(BoneEntry, Resource)
protected:
	static void _bind_methods();
public:
	BoneEntry(){}
	~BoneEntry(){}

    GETTER_SETTER_DEFINE(uint8_t, InheritPosition)
    GETTER_SETTER_DEFINE(uint8_t, unk0)
	GETTER_SETTER_DEFINE(Ref<BoneMatrix>, Matrix)

private:
    uint8_t InheritPosition;
	uint8_t unk0;
	Ref<BoneMatrix> Matrix;
};

class TRSkeleton : public Resource {
	GDCLASS(TRSkeleton, Resource)
protected:
	static void _bind_methods();
public:
	TRSkeleton(){}
	~TRSkeleton(){}

    void LoadFromFile(String file);

	GETTER_SETTER_DEFINE(int, res0)
	GETTER_SETTER_DEFINE(Array, TransformNodes)
	GETTER_SETTER_DEFINE(Array, Bones)
	GETTER_SETTER_DEFINE(Array, IKs)
	GETTER_SETTER_DEFINE(int, RigOffset)

private:
	int res0;
	Array TransformNodes;
	Array Bones;
	Array IKs;
	int RigOffset;
};

class ResourceFormatLoaderTRSKL : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRSKL, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRSKL(){}
	~ResourceFormatLoaderTRSKL(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}