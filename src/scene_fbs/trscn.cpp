#include "trscn.h"

using namespace godot;

void TRScene::_bind_methods()
{
    GETTER_SETTER_BIND(TRScene, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRScene, Type, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRSCN::_bind_methods()
{
    GETTER_SETTER_BIND(TRSCN, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSCN, Extra, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSCN, scenes, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRScene")
}

void TRSCN::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load SCN file: %s", file));
    auto trscn = Titan::TrinityScene::GetTRSCN(buf.ptr());
    ERR_FAIL_COND_MSG(trscn == nullptr, "Couldn't parse SCN");

    // todo: parse scene
}

Variant ResourceFormatLoaderTRSCN::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRSCN> trscn;
    trscn.instantiate();
    trscn->LoadFromFile(p_path);
    return trscn;
}

PackedStringArray ResourceFormatLoaderTRSCN::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trscn");

    return exts;
}

bool ResourceFormatLoaderTRSCN::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRSCN");
}