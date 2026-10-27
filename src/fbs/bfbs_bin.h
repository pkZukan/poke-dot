#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "bfbs.h"

namespace godot
{

class FlatbufferBin : public Resource
{
    GDCLASS(FlatbufferBin, Resource)

    Ref<FlatbufferSchema> schema;
    Dictionary data;

protected:
    static void _bind_methods();

public:
    Error LoadFromFile(const String& file);
    Ref<FlatbufferSchema> get_schema() const;
    Dictionary get_data() const;
};

class ResourceFormatLoaderBFBSBIN : public ResourceFormatLoader
{
    GDCLASS(ResourceFormatLoaderBFBSBIN, ResourceFormatLoader)
protected:
    static void _bind_methods()
    {
    }

public:
    virtual PackedStringArray _get_recognized_extensions() const override;
    virtual bool _handles_type(const StringName& p_type) const override;
    virtual bool _recognize_path(const String& p_path, const StringName& p_type) const override;
    virtual String _get_resource_type(const String& p_path) const override;
    virtual Variant _load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                          int32_t p_cache_mode) const override;
};

}
