#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "trmtr_generated.h"
#include <utils.h>
#include <common.h>

namespace godot {

class ByteExtra : public Resource {
    GDCLASS(ByteExtra, Resource)
protected:
	static void _bind_methods();
public:
	ByteExtra(){}
	~ByteExtra(){}

};

class IntExtra : public Resource {
    GDCLASS(IntExtra, Resource)
protected:
	static void _bind_methods();
public:
	IntExtra(){}
	~IntExtra(){}

};

class FloatParameter : public Resource {
    GDCLASS(FloatParameter, Resource)
protected:
	static void _bind_methods();
public:
	FloatParameter(){}
	~FloatParameter(){}

};

class Float4Parameter : public Resource {
    GDCLASS(Float4Parameter, Resource)
protected:
	static void _bind_methods();
public:
	Float4Parameter(){}
	~Float4Parameter(){}

};

class IntParameter : public Resource {
    GDCLASS(IntParameter, Resource)
protected:
	static void _bind_methods();
public:
	IntParameter(){}
	~IntParameter(){}

};

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

	GETTER_SETTER_DEFINE(int, SamplerState_0)
	GETTER_SETTER_DEFINE(int, SamplerState_1)
	GETTER_SETTER_DEFINE(int, SamplerState_2)
	GETTER_SETTER_DEFINE(int, SamplerState_3)
	GETTER_SETTER_DEFINE(int, SamplerState_4)
	GETTER_SETTER_DEFINE(int, SamplerState_5)
	GETTER_SETTER_DEFINE(int, SamplerState_6)
	GETTER_SETTER_DEFINE(int, SamplerState_7)
	GETTER_SETTER_DEFINE(int, SamplerState_8)
	GETTER_SETTER_DEFINE(String, RepeatU)
	GETTER_SETTER_DEFINE(String, RepeatV)
	GETTER_SETTER_DEFINE(String, RepeatW)
	GETTER_SETTER_DEFINE(Ref<RGBA>, BorderColor)

private:
	int SamplerState_0;
	int SamplerState_1;
	int SamplerState_2;
	int SamplerState_3;
	int SamplerState_4;
	int SamplerState_5;
	int SamplerState_6;
	int SamplerState_7;
	int SamplerState_8;
	String RepeatU;
	String RepeatV;
	String RepeatW;
	Ref<RGBA> BorderColor;
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
	GETTER_SETTER_DEFINE(String, unk_5)
	GETTER_SETTER_DEFINE(Array, FloatLightParams)
	GETTER_SETTER_DEFINE(Array, Float4Params)
	GETTER_SETTER_DEFINE(String, unk_8)
	GETTER_SETTER_DEFINE(Array, IntParams)
	GETTER_SETTER_DEFINE(String, unk_10)
	GETTER_SETTER_DEFINE(String, unk_11)
	GETTER_SETTER_DEFINE(String, unk_12)
	GETTER_SETTER_DEFINE(Ref<ByteExtra>, ByteExt)
	GETTER_SETTER_DEFINE(Ref<IntExtra>, IntExt)
	GETTER_SETTER_DEFINE(String, AlphaType)

private:
    String Name;
	Array Shaders;
	Array Textures;
	Array Samplers;
	Array FloatParams;
	String unk_5;
	Array FloatLightParams;
	Array Float4Params;
	String unk_8;
	Array IntParams;
	String unk_10;
	String unk_11;
	String unk_12;
	Ref<ByteExtra> ByteExt;
	Ref<IntExtra> IntExt;
	String AlphaType;
};

class TRMaterial : public Resource {
    GDCLASS(TRMaterial, Resource)
protected:
	static void _bind_methods();
public:
	TRMaterial(){}
	~TRMaterial(){}

    void LoadFromFile(String file);

	GETTER_SETTER_DEFINE(int, unk0)
	GETTER_SETTER_DEFINE(Array, Materials)

private:
	int unk0;
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