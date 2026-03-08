#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/tracr_generated.h"
#include <utils.h>

namespace godot {

class TRAnimationTrackResource : public Resource {
	GDCLASS(TRAnimationTrackResource, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationTrackResource(){}
	~TRAnimationTrackResource(){}

	GETTER_SETTER_DEFINE(String, FileName)

private:
	String FileName;
};

class TRAnimationTrackResourceTable : public Resource {
	GDCLASS(TRAnimationTrackResourceTable, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationTrackResourceTable(){}
	~TRAnimationTrackResourceTable(){}

	GETTER_SETTER_DEFINE(Ref<TRAnimationTrackResource>, animation)
	GETTER_SETTER_DEFINE(Ref<TRAnimationTrackResource>, material)
	GETTER_SETTER_DEFINE(Ref<TRAnimationTrackResource>, effect)

private:
	Ref<TRAnimationTrackResource> animation;
	Ref<TRAnimationTrackResource> material;
	Ref<TRAnimationTrackResource> effect;
};

class TRAnimationTrack : public Resource {
	GDCLASS(TRAnimationTrack, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationTrack(){}
	~TRAnimationTrack(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(String, res_0)
	GETTER_SETTER_DEFINE(uint32_t, res_1)
	GETTER_SETTER_DEFINE(Ref<TRAnimationTrackResourceTable>, Resources)

private:
	String Name;
	String res_0;
	uint32_t res_1;
	Ref<TRAnimationTrackResourceTable> Resources;
};

class TRAnimationTrackListTable : public Resource {
	GDCLASS(TRAnimationTrackListTable, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationTrackListTable(){}
	~TRAnimationTrackListTable(){}

	GETTER_SETTER_DEFINE(Array, list)

private:
	Array list;
};

class TRAnimationChannelResource : public Resource {
    GDCLASS(TRAnimationChannelResource, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationChannelResource(){}
	~TRAnimationChannelResource(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Ref<TRAnimationTrackListTable>, table)

private:
    Ref<TRAnimationTrackListTable> table;
};

class ResourceFormatLoaderTRACR : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRACR, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRACR(){}
	~ResourceFormatLoaderTRACR(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}