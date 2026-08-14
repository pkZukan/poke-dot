#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trscn_generated.h"
#include "trinity_scene_parser.h"
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
	GETTER_SETTER_DEFINE(Ref<Resource>, nested_type)
	GETTER_SETTER_DEFINE(Array, sub_objects)

private:
    String Name;
	Ref<Resource> nested_type;
	Array sub_objects;
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
	GETTER_SETTER_DEFINE(uint32_t, res_2)
	GETTER_SETTER_DEFINE(uint32_t, res_3)
	GETTER_SETTER_DEFINE(Array, chunks)
	GETTER_SETTER_DEFINE(Array, res_4)
	GETTER_SETTER_DEFINE(uint8_t, unk_6)

private:
	Ref<TRScene> _createTRScene(const Titan::TrinityScene::SceneEntry* chunk);
    String Name;
    String Extra;
	uint32_t res_2;
	uint32_t res_3;
    Array chunks;
	Array res_4;
	uint8_t unk_6;
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