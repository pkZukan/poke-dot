#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/Ikkaku/gfx2/trlgt_generated.h"
#include <utils.h>

namespace godot
{

class TRLightCurve : public Resource
{
    GDCLASS(TRLightCurve, Resource)
protected:
    static void _bind_methods();

public:
    GETTER_SETTER_DEFINE(Array, keys)
    float sample(double time) const;

private:
    Array keys;
};

class TRLightCurve4 : public Resource
{
    GDCLASS(TRLightCurve4, Resource)
protected:
    static void _bind_methods();

public:
    GETTER_SETTER_DEFINE(Ref<TRLightCurve>, x)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve>, y)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve>, z)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve>, w)
    Vector4 sample(double time) const;

private:
    Ref<TRLightCurve> x, y, z, w;
};

class TRFloatParameter : public Resource
{
    GDCLASS(TRFloatParameter, Resource)
protected:
    static void _bind_methods();

public:
    TRFloatParameter() = default;
    ~TRFloatParameter() = default;
    GETTER_SETTER_DEFINE(String, param_name)
    GETTER_SETTER_DEFINE(float, param_val)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve>, curve)

    float sample(double time) const
    {
        return curve.is_valid() ? curve->sample(time) : param_val;
    }

private:
    String param_name;
    float param_val = 0.0f;
    Ref<TRLightCurve> curve;
};

class TRVec4Parameter : public Resource
{
    GDCLASS(TRVec4Parameter, Resource)
protected:
    static void _bind_methods();

public:
    TRVec4Parameter() = default;
    ~TRVec4Parameter() = default;
    GETTER_SETTER_DEFINE(String, param_name)
    GETTER_SETTER_DEFINE(Vector4, value)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve4>, curve)

    Vector4 sample(double time) const
    {
        return curve.is_valid() ? curve->sample(time) : value;
    }

private:
    String param_name;
    Vector4 value;
    Ref<TRLightCurve4> curve;
};

class TRStringParameter : public Resource
{
    GDCLASS(TRStringParameter, Resource)
protected:
    static void _bind_methods();

public:
    TRStringParameter() = default;
    ~TRStringParameter() = default;
    GETTER_SETTER_DEFINE(String, map_name)
    GETTER_SETTER_DEFINE(String, map_file)

private:
    String map_name;
    String map_file;
};

class BoundMatrix : public Resource
{
    GDCLASS(BoundMatrix, Resource)
protected:
    static void _bind_methods();

public:
    BoundMatrix() = default;
    ~BoundMatrix() = default;
    GETTER_SETTER_DEFINE(Vector4, x)
    GETTER_SETTER_DEFINE(Vector4, y)
    GETTER_SETTER_DEFINE(Vector4, z)

private:
    Vector4 x;
    Vector4 y;
    Vector4 z;
};

class TRLight : public Resource
{
    GDCLASS(TRLight, Resource)
protected:
    static void _bind_methods();

public:
    TRLight() = default;
    ~TRLight() = default;

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, Type)
    GETTER_SETTER_DEFINE(Ref<BoundMatrix>, Matrix)
    GETTER_SETTER_DEFINE(Array, float_params)
    GETTER_SETTER_DEFINE(Array, vec4_params)
    GETTER_SETTER_DEFINE(Array, string_params)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve4>, scale_curve)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve4>, rotation_curve)
    GETTER_SETTER_DEFINE(Ref<TRLightCurve4>, translation_curve)
    Ref<BoundMatrix> sample_matrix(double time) const;

private:
    String Name;
    String Type;
    Ref<BoundMatrix> Matrix;
    Array float_params;
    Array vec4_params;
    Array string_params;
    Ref<TRLightCurve4> scale_curve, rotation_curve, translation_curve;
};

class TRLGT : public Resource
{
    GDCLASS(TRLGT, Resource)
protected:
    static void _bind_methods();

public:
    TRLGT() = default;
    ~TRLGT() = default;

    Error LoadFromFile(String file);

    GETTER_SETTER_DEFINE(uint32_t, version)
    GETTER_SETTER_DEFINE(Array, lights)

private:
    uint32_t version = 0;
    Array lights;
};

class ResourceFormatLoaderTRLGT : public ResourceFormatLoader
{
    GDCLASS(ResourceFormatLoaderTRLGT, ResourceFormatLoader)
protected:
    static void _bind_methods()
    {
    }

public:
    ResourceFormatLoaderTRLGT() = default;
    ~ResourceFormatLoaderTRLGT() = default;

    virtual PackedStringArray _get_recognized_extensions() const override;
    String _get_resource_type(const String& path) const override;
    virtual bool _handles_type(const StringName& p_type) const override;
    virtual Variant _load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                          int32_t p_cache_mode) const override;
};

}
