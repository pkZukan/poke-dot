#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include "utils.h"

#define HAVOK_TAG0_MAGIC 0x54414730

#define HAVOK_TAG_DATA   0x44415441
#define HAVOK_TAG_TYPE   0x54595045
#define HAVOK_TAG_INDX   0x494E4458

#define HAVOK_TAG_TST1   0x54535431
#define HAVOK_TAG_TNA1   0x544E4131
#define HAVOK_TAG_FST1   0x46535431
#define HAVOK_TAG_TBDY   0x54424459

#define HAVOK_TAG_ITEM   0x4954454D

namespace godot {

struct TrcolHeader
{
	String Magic;

	TrcolHeader(Ref<StreamPeerBuffer> sp)
	{
		sp->get_32(); //header
		Magic = sp->get_string(4);
	}
};

struct HavokSectionHeader
{
    uint32_t size_and_flags;
    uint32_t tag;

	HavokSectionHeader(Ref<StreamPeerBuffer> sp)
	{
		size_and_flags = __builtin_bswap32(sp->get_32());
		tag = sp->get_32();
	}

	uint32_t get_section_size() 
	{
    	return size_and_flags & 0x3FFFFFFF;
	}
};

class TRCOL : public Shape3D {
    GDCLASS(TRCOL, Shape3D)
protected:
	static void _bind_methods();
public:
	TRCOL(){}
	~TRCOL(){}

    void LoadFromFile(String file);
};

class ResourceFormatLoaderTRCOL : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRCOL, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRCOL(){}
	~ResourceFormatLoaderTRCOL(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}