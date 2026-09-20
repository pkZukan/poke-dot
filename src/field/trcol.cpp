#include "trcol.h"

using namespace godot;

void TRCOL::_bind_methods() 
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "file"), &TRCOL::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_tag"), &TRCOL::get_tag);
}

void TRCOL::LoadFromFile(String file)
{
	tag.instantiate();
	tag->LoadFromFile(file);
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
	return p_type == StringName("TRCOL");
}