#include "bfbs_bin.h"

#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

void FlatbufferBin::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("load_from_file", "path"), &FlatbufferBin::LoadFromFile);
    ClassDB::bind_method(D_METHOD("get_schema"), &FlatbufferBin::get_schema);
    ClassDB::bind_method(D_METHOD("get_data"), &FlatbufferBin::get_data);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "schema", PROPERTY_HINT_RESOURCE_TYPE, "FlatbufferSchema",
                             PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_schema");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data", PROPERTY_HINT_NONE, "",
                             PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_data");
}

Error FlatbufferBin::LoadFromFile(const String& file)
{
    String bfbs_path = file.get_basename() + ".bfbs";
    PackedByteArray bytes = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_V_MSG(bytes.is_empty(), ERR_FILE_CANT_OPEN, "Couldn't read BIN file: " + file);

    Ref<FlatbufferSchema> loaded_schema;
    loaded_schema.instantiate();
    Error error = loaded_schema->LoadFromFile(bfbs_path);
    ERR_FAIL_COND_V_MSG(error != OK, error, "Couldn't load schema: " + bfbs_path);
    Variant decoded = loaded_schema->decode(bytes);
    ERR_FAIL_COND_V_MSG(decoded.get_type() != Variant::DICTIONARY, ERR_FILE_CORRUPT, "Couldn't decode BIN file: " + file);
    schema = loaded_schema;
    data = decoded;
    notify_property_list_changed();
    return OK;
}

Ref<FlatbufferSchema> FlatbufferBin::get_schema() const
{
    return schema;
}

Dictionary FlatbufferBin::get_data() const
{
    return data;
}

Variant ResourceFormatLoaderBFBSBIN::_load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                                        int32_t p_cache_mode) const
{
    Ref<FlatbufferBin> bin;
    bin.instantiate();
    Error error = bin->LoadFromFile(p_path);
    if (error != OK)
    {
        return error;
    }
    return bin;
}

PackedStringArray ResourceFormatLoaderBFBSBIN::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("bin");
    return exts;
}

bool ResourceFormatLoaderBFBSBIN::_handles_type(const StringName& p_type) const
{
    return p_type == StringName("FlatbufferBin");
}

bool ResourceFormatLoaderBFBSBIN::_recognize_path(const String& p_path, const StringName& p_type) const
{
    return (p_type == StringName() || _handles_type(p_type)) && !_get_resource_type(p_path).is_empty();
}

String ResourceFormatLoaderBFBSBIN::_get_resource_type(const String& p_path) const
{
    if (p_path.get_extension().to_lower() != "bin")
    {
        return String();
    }

    // The generated personal loader supplies typed resources used by PokemonCatalog.
    if (p_path.get_file().get_basename() == "personal_array")
    {
        return String();
    }

    String bfbs_path = p_path.get_basename() + ".bfbs";

    if (FileAccess::file_exists(bfbs_path))
    {
        return String("FlatbufferBin");
    }

    return String();
}
