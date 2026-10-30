#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/Ikkaku/Model/trmmt_generated.h"
#include <utils.h>

namespace godot
{

class VariationControl : public Resource
{
    GDCLASS(VariationControl, Resource)
protected:
    static void _bind_methods();

public:
    VariationControl() = default;
    ~VariationControl() = default;

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(int, FrameMin)
    GETTER_SETTER_DEFINE(int, FrameMax)
    GETTER_SETTER_DEFINE(int, DefaultOverride)
    GETTER_SETTER_DEFINE(PackedByteArray, Tracm)
    GETTER_SETTER_DEFINE(Dictionary, Rule)
    GETTER_SETTER_DEFINE(bool, TableDriven)
    GETTER_SETTER_DEFINE(int, VariationCount)
    GETTER_SETTER_DEFINE(Dictionary, MaterialValues)
    GETTER_SETTER_DEFINE(Dictionary, MeshVisibility)

private:
    String Name;
    int FrameMin = 0;
    int FrameMax = 0;
    int DefaultOverride = -1;
    PackedByteArray Tracm;
    Dictionary Rule;
    bool TableDriven = false;
    int VariationCount = 0;
    Dictionary MaterialValues;
    Dictionary MeshVisibility;
};

class ModelVariation : public Resource
{
    GDCLASS(ModelVariation, Resource)
protected:
    static void _bind_methods();

public:
    ModelVariation() = default;
    ~ModelVariation() = default;

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(PackedStringArray, Materials)
    GETTER_SETTER_DEFINE(Dictionary, MeshVisibility)
    GETTER_SETTER_DEFINE(Array, Controls)

private:
    String Name;
    PackedStringArray Materials;
    Dictionary MeshVisibility;
    Array Controls;
};

class TRModelMeta : public Resource
{
    GDCLASS(TRModelMeta, Resource)
protected:
    static void _bind_methods();

public:
    TRModelMeta() = default;
    ~TRModelMeta() = default;

    void LoadFromFile(String file);
    Ref<ModelVariation> find_variation(const String& name) const;

    GETTER_SETTER_DEFINE(Array, Variations)

private:
    Array Variations;
};

class ResourceFormatLoaderTRMMT : public ResourceFormatLoader
{
    GDCLASS(ResourceFormatLoaderTRMMT, ResourceFormatLoader)
protected:
    static void _bind_methods()
    {
    }

public:
    ResourceFormatLoaderTRMMT() = default;
    ~ResourceFormatLoaderTRMMT() = default;

    virtual PackedStringArray _get_recognized_extensions() const override;
    virtual bool _handles_type(const StringName& p_type) const override;
    virtual Variant _load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                          int32_t p_cache_mode) const override;
};

}
