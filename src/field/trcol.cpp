#include "trcol.h"

using namespace godot;

void TRCOL::_bind_methods() 
{
    //
}

void TRCOL::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRCOL file: %s", file));

    Ref<StreamPeerBuffer> sp;
    sp.instantiate();
    sp->set_data_array(buf);

    TrcolHeader header(sp);
    ERR_FAIL_COND_MSG(header.Magic != "TAG0", "Invalid TRCOL magic");

    //TODO
}

Variant ResourceFormatLoaderTRCOL::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRCOL> trcol;
    trcol.instantiate();
    trcol->LoadFromFile(p_path);
    return trcol;
}

PackedStringArray ResourceFormatLoaderTRCOL::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trcol");
    return exts;
}

bool ResourceFormatLoaderTRCOL::_handles_type(const StringName &p_type) const
{
    // Must match the actual class name, not ImageTexture
    return p_type == String("TRCOL");
}