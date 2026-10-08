#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/tracp_generated.h"
#include <utils.h>

namespace godot {

class AnimationChannelParamInt : public Resource {
	GDCLASS(AnimationChannelParamInt, Resource)
protected:
	static void _bind_methods();
public:
	AnimationChannelParamInt(){}
	~AnimationChannelParamInt(){}

	GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(uint32_t, min)
	GETTER_SETTER_DEFINE(uint32_t, max)
    GETTER_SETTER_DEFINE(uint32_t, default_val)

private:
	String Name;
    uint32_t min;
	uint32_t max;
    uint32_t default_val;
};

class AnimationChannelParamTrigger : public Resource {
	GDCLASS(AnimationChannelParamTrigger, Resource)
protected:
	static void _bind_methods();
public:
	AnimationChannelParamTrigger(){}
	~AnimationChannelParamTrigger(){}

	GETTER_SETTER_DEFINE(String, Name)

private:
	String Name;
};

class AnimationChannelParamBool : public Resource {
	GDCLASS(AnimationChannelParamBool, Resource)
protected:
	static void _bind_methods();
public:
	AnimationChannelParamBool(){}
	~AnimationChannelParamBool(){}

	GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(bool, Val)

private:
	String Name;
    bool Val;
};

class AnimationChannelParamFloat : public Resource {
	GDCLASS(AnimationChannelParamFloat, Resource)
protected:
	static void _bind_methods();
public:
	AnimationChannelParamFloat(){}
	~AnimationChannelParamFloat(){}

	GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(float, min)
	GETTER_SETTER_DEFINE(float, max)
    GETTER_SETTER_DEFINE(float, default_val)

private:
	String Name;
    float min;
	float max;
    float default_val;
};

class AnimationChannelParamEntry : public Resource {
	GDCLASS(AnimationChannelParamEntry, Resource)
protected:
	static void _bind_methods();
public:
	AnimationChannelParamEntry(){}
	~AnimationChannelParamEntry(){}

	GETTER_SETTER_DEFINE(Array, animations)
    GETTER_SETTER_DEFINE(Array, moves)
	GETTER_SETTER_DEFINE(Array, switches)
    GETTER_SETTER_DEFINE(Array, triggers)

private:
	Array animations;
    Array moves;
	Array switches;
    Array triggers;
};

class TRAnimationChannelParams : public Resource {
    GDCLASS(TRAnimationChannelParams, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationChannelParams(){}
	~TRAnimationChannelParams(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Ref<AnimationChannelParamEntry>, table)

private:
    Ref<AnimationChannelParamEntry> table;
};

class ResourceFormatLoaderTRACP : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRACP, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRACP(){}
	~ResourceFormatLoaderTRACP(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}