#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trscn_generated.h"
#include <utils.h>

namespace godot {

class TRScene : public Resource {
	GDCLASS(TRScene, Resource)
protected:
	static void _bind_methods();
public:
	TRScene(){}
	~TRScene(){}

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, Type)

private:
    String Name;
    String Type;
};

class TRSCN : public Resource {
    GDCLASS(TRSCN, Resource)
protected:
	static void _bind_methods();
public:
	TRSCN(){}
	~TRSCN(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, Extra)
    GETTER_SETTER_DEFINE(Array, scenes)

private:
    String Name;
    String Extra;
    Array scenes;
};

class ResourceFormatLoaderTRSCN : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRSCN, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRSCN(){}
	~ResourceFormatLoaderTRSCN(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}