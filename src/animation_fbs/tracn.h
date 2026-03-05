#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/tracn_generated.h"
#include <utils.h>

namespace godot {

class AnimationEntry : public Resource {
	GDCLASS(AnimationEntry, Resource)
protected:
	static void _bind_methods();
public:
	AnimationEntry(){}
	~AnimationEntry(){}

	GETTER_SETTER_DEFINE(String, animation_name)
    GETTER_SETTER_DEFINE(String, filename)

private:
	String animation_name;
    String filename;
};

class TRAnimationChannelNames : public Resource {
    GDCLASS(TRAnimationChannelNames, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationChannelNames(){}
	~TRAnimationChannelNames(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Array, list)

private:
    Array list;
};

class ResourceFormatLoaderTRACN : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRACN, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRACN(){}
	~ResourceFormatLoaderTRACN(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}