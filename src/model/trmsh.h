#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "trmsh_generated.h"
#include <utils.h>
#include <common.h>

namespace godot {

class Sphere : public Resource {
	GDCLASS(Sphere, Resource)
protected:
	static void _bind_methods();
public:
	Sphere(){}
	~Sphere(){}

	GETTER_SETTER_DEFINE(float, X)
	GETTER_SETTER_DEFINE(float, Y)
	GETTER_SETTER_DEFINE(float, Z)
	GETTER_SETTER_DEFINE(float, Radius)

private:
	float X, Y, Z;
	float Radius;
};

class InfluenceEntry : public Resource {
	GDCLASS(InfluenceEntry, Resource)
protected:
	static void _bind_methods();
public:
	InfluenceEntry(){}
	~InfluenceEntry(){}
	
	GETTER_SETTER_DEFINE(int, Index)
	GETTER_SETTER_DEFINE(float, Scale)

private:
	int Index;
	float Scale;
};

class MaterialInfo : public Resource {
	GDCLASS(MaterialInfo, Resource)
protected:
	static void _bind_methods();
public:
	MaterialInfo(){}
	~MaterialInfo(){}

	GETTER_SETTER_DEFINE(int, PolyCount)
	GETTER_SETTER_DEFINE(int, PolyOffset)
	GETTER_SETTER_DEFINE(int, unk0)
	GETTER_SETTER_DEFINE(String, MaterialName)
	GETTER_SETTER_DEFINE(int, unk1)

private:
	int PolyCount;
	int PolyOffset;
	int unk0;
	String MaterialName;
	int unk1;
};

class MeshAttrib : public Resource {
	GDCLASS(MeshAttrib, Resource)
protected:
	static void _bind_methods();
public:
	MeshAttrib(){}
	~MeshAttrib(){}

	GETTER_SETTER_DEFINE(Dictionary, Descriptor)
	GETTER_SETTER_DEFINE(int, Stride)

private:
	Dictionary Descriptor;
	int Stride;
};

class MeshShape : public Resource {
	GDCLASS(MeshShape, Resource)
protected:
	static void _bind_methods();
public:
	MeshShape(){}
	~MeshShape(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(Ref<BoundingBox>, BBox)
	GETTER_SETTER_DEFINE(int, PolygonType)
	GETTER_SETTER_DEFINE(Array, Attributes)
	GETTER_SETTER_DEFINE(Array, Materials)
	GETTER_SETTER_DEFINE(String, MeshName)
	GETTER_SETTER_DEFINE(int, res0)
	GETTER_SETTER_DEFINE(int, res1)
	GETTER_SETTER_DEFINE(int, res2)
	GETTER_SETTER_DEFINE(int, res3)
	GETTER_SETTER_DEFINE(Ref<Sphere>, ClipSphere)
	GETTER_SETTER_DEFINE(Ref<InfluenceEntry>, Influences)

private:
	String Name;
	Ref<BoundingBox> BBox;
	int PolygonType;
	Array Attributes;
	Array Materials;
	int res0, res1, res2, res3;
	Ref<Sphere> ClipSphere;
	Ref<InfluenceEntry> Influences;

	String MeshName;
};

class TRMesh : public Resource {
    GDCLASS(TRMesh, Resource)
protected:
	static void _bind_methods();
public:
	TRMesh(){}
	~TRMesh(){}

    void LoadFromFile(String file);

	GETTER_SETTER_DEFINE(int, unk0)
	GETTER_SETTER_DEFINE(String, BufferName)
	GETTER_SETTER_DEFINE(Array, MeshDescriptors)

private:
	int unk0;
	String BufferName;
    Array MeshDescriptors;
};

class ResourceFormatLoaderTRMSH : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRMSH, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRMSH(){}
	~ResourceFormatLoaderTRMSH(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}