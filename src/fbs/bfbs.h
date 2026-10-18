#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "flatbuffers/reflection_generated.h"

namespace godot {

class FlatbufferSchema : public Resource {
    GDCLASS(FlatbufferSchema, Resource)
    PackedByteArray schema_data;

protected:
    static void _bind_methods();

public:
    Error LoadFromFile(const String &file);
    const reflection::Schema *get_schema() const;

    String get_root_type() const;
    Variant decode(const PackedByteArray &data) const;
    Variant decode_file(const String &path) const;

    static Variant decode_table(const String &path);
};

class ResourceFormatLoaderBFBS : public ResourceFormatLoader {
    GDCLASS(ResourceFormatLoaderBFBS, ResourceFormatLoader)
protected:
    static void _bind_methods() {}
public:
    virtual PackedStringArray _get_recognized_extensions() const override;
    virtual bool _handles_type(const StringName &p_type) const override;
    virtual String _get_resource_type(const String &p_path) const override;
    virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}
