#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "trmtr_generated.h"
#include <utils.h>

namespace godot {

class ShaderIntParam : public Resource {
    GDCLASS(ShaderIntParam, Resource)
protected:
	static void _bind_methods();
public:
	ShaderIntParam(){}
	~ShaderIntParam(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(int, Value)

private:
    String Name;
	int Value;
};

class ShaderStringParam : public Resource {
    GDCLASS(ShaderStringParam, Resource)
protected:
	static void _bind_methods();
public:
	ShaderStringParam(){}
	~ShaderStringParam(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(String, Value)

private:
    String Name;
	String Value;
};

class SamplerEntry : public Resource {
    GDCLASS(SamplerEntry, Resource)
protected:
	static void _bind_methods();
public:
	SamplerEntry(){}
	~SamplerEntry(){}

};

class TextureEntry : public Resource {
    GDCLASS(TextureEntry, Resource)
protected:
	static void _bind_methods();
public:
	TextureEntry(){}
	~TextureEntry(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(String, File)
	GETTER_SETTER_DEFINE(int, Slot)

private:
    String Name;
	String File;
	int Slot;
};

class ShaderEntry : public Resource {
    GDCLASS(ShaderEntry, Resource)
protected:
	static void _bind_methods();
public:
	ShaderEntry(){}
	~ShaderEntry(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(Array, StringParams)

private:
    String Name;
	Array StringParams;
};

class MaterialEntry : public Resource {
    GDCLASS(MaterialEntry, Resource)
protected:
	static void _bind_methods();
public:
	MaterialEntry(){}
	~MaterialEntry(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(Array, Shaders)
	GETTER_SETTER_DEFINE(Array, Textures)
	GETTER_SETTER_DEFINE(Array, Samplers)
	GETTER_SETTER_DEFINE(Array, FloatParams)
	GETTER_SETTER_DEFINE(String, Unk5)

private:
    String Name;
	Array Shaders;
	Array Textures;
	Array Samplers;
	Array FloatParams;
	String Unk5;

};

class TRMaterial : public Resource {
    GDCLASS(TRMaterial, Resource)
protected:
	static void _bind_methods();
public:
	TRMaterial(){}
	~TRMaterial(){}

    void LoadFromFile(String file);

	GETTER_SETTER_DEFINE(Array, Materials)

private:
    Array Materials;
};

class ResourceFormatLoaderTRMTR : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRMTR, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRMTR(){}
	~ResourceFormatLoaderTRMTR(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}