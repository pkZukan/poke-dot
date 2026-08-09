#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/truiv_generated.h"
#include "uikit/uikit_gauge.h"
#include "uikit/uikit_body.h"
#include "uikit/uikit_shortcut.h"
#include "uikit/uikit_switch.h"
#include "uikit/uikit_button.h"
#include <utils.h>

namespace godot {

class TRUIViewChunk : public Resource {
	GDCLASS(TRUIViewChunk, Resource)
protected:
	static void _bind_methods();
public:
	TRUIViewChunk(){}
	~TRUIViewChunk(){}

    GETTER_SETTER_DEFINE(String, Type)
	GETTER_SETTER_DEFINE(Ref<Resource>, Data)
	GETTER_SETTER_DEFINE(Array, Children)

	Ref<Resource> ParseChunkData(String type, const void* data);

private:
    String Type;
	Ref<Resource> Data;
	Array Children;
};

class TRUIV : public Resource {
    GDCLASS(TRUIV, Resource)
protected:
	static void _bind_methods();
public:
	TRUIV(){}
	~TRUIV(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Array, Chunks)

private:
    Array Chunks;
};

class ResourceFormatLoaderTRUIV : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRUIV, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRUIV(){}
	~ResourceFormatLoaderTRUIV(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}