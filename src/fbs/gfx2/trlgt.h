#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trlgt_generated.h"
#include <utils.h>

namespace godot {

class TRFloatParameter : public Resource {
    GDCLASS(TRFloatParameter, Resource)
protected:
    static void _bind_methods();
public:
    TRFloatParameter(){}
    ~TRFloatParameter(){}
    GETTER_SETTER_DEFINE(String, param_name)
    GETTER_SETTER_DEFINE(float, param_val)

private:
    String param_name;
    float param_val;
};

class TRVec4Parameter : public Resource {
    GDCLASS(TRVec4Parameter, Resource)
protected:
    static void _bind_methods();
public:
    TRVec4Parameter(){}
    ~TRVec4Parameter(){}
    GETTER_SETTER_DEFINE(String, param_name)
    GETTER_SETTER_DEFINE(Vector4, value)

private:
    String param_name;
    Vector4 value;
};

class TRStringParameter : public Resource {
    GDCLASS(TRStringParameter, Resource)
protected:
    static void _bind_methods();
public:
    TRStringParameter(){}
    ~TRStringParameter(){}
    GETTER_SETTER_DEFINE(String, map_name)
    GETTER_SETTER_DEFINE(String, map_file)

private:
    String map_name;
    String map_file;
};

class BoundMatrix : public Resource {
    GDCLASS(BoundMatrix, Resource)
protected:
    static void _bind_methods();
public:
    BoundMatrix(){}
    ~BoundMatrix(){}
    GETTER_SETTER_DEFINE(Vector4, x)
    GETTER_SETTER_DEFINE(Vector4, y)
    GETTER_SETTER_DEFINE(Vector4, z)

private:
    Vector4 x;
    Vector4 y;
    Vector4 z;
};

class TRLight : public Resource {
	GDCLASS(TRLight, Resource)
protected:
	static void _bind_methods();
public:
	TRLight(){}
	~TRLight(){}

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, Type)
    GETTER_SETTER_DEFINE(Ref<BoundMatrix>, Matrix)
    GETTER_SETTER_DEFINE(Array, float_params)
    GETTER_SETTER_DEFINE(Array, vec4_params)
    GETTER_SETTER_DEFINE(Array, string_params)

private:
    String Name;
    String Type;
    Ref<BoundMatrix> Matrix;
    Array float_params;
    Array vec4_params;
    Array string_params;
};

class TRLGT : public Resource {
    GDCLASS(TRLGT, Resource)
protected:
	static void _bind_methods();
public:
	TRLGT(){}
	~TRLGT(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(uint32_t, version)
    GETTER_SETTER_DEFINE(Array, lights)

private:
    uint32_t version;
    Array lights;
};

class ResourceFormatLoaderTRLGT : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRLGT, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRLGT(){}
	~ResourceFormatLoaderTRLGT(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}