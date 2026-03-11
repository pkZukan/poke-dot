#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "animation_fbs/anim_common.h"
#include "generated/tracm_generated.h"
#include <utils.h>

namespace godot {

class Framed8BoolTrack : public Resource {
	GDCLASS(Framed8BoolTrack, Resource)
protected:
	static void _bind_methods();
public:
	Framed8BoolTrack(){}
	~Framed8BoolTrack(){}

    GETTER_SETTER_DEFINE(Array, frames)
	GETTER_SETTER_DEFINE(Array, values)

private:
    Array frames;
	Array values;
};

class Framed16BoolTrack : public Resource {
	GDCLASS(Framed16BoolTrack, Resource)
protected:
	static void _bind_methods();
public:
	Framed16BoolTrack(){}
	~Framed16BoolTrack(){}

    GETTER_SETTER_DEFINE(Array, frames)
	GETTER_SETTER_DEFINE(Array, values)

private:
    Array frames;
	Array values;
};

class DynamicBoolTrack : public Resource {
	GDCLASS(DynamicBoolTrack, Resource)
protected:
	static void _bind_methods();
public:
	DynamicBoolTrack(){}
	~DynamicBoolTrack(){}

	GETTER_SETTER_DEFINE(Array, values)

private:
	Array values;
};

class FixedBoolTrack : public Resource {
	GDCLASS(FixedBoolTrack, Resource)
protected:
	static void _bind_methods();
public:
	FixedBoolTrack(){}
	~FixedBoolTrack(){}

	GETTER_SETTER_DEFINE(bool, values)

private:
	bool values;
};


class TRTrackFlagsInfo : public Resource {
    GDCLASS(TRTrackFlagsInfo, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackFlagsInfo(){}
	~TRTrackFlagsInfo(){}
	
private:
	Ref<Resource> values; //TRTrackFlag
};

class TRBlendShapeTimeline : public Resource {
    GDCLASS(TRBlendShapeTimeline, Resource)
protected:
	static void _bind_methods();
public:
	TRBlendShapeTimeline(){}
	~TRBlendShapeTimeline(){}

	GETTER_SETTER_DEFINE(Ref<TRAnimationInfo>, info)
	GETTER_SETTER_DEFINE(uint32_t, res_1)
	GETTER_SETTER_DEFINE(Array, blendshape_tracks)
	GETTER_SETTER_DEFINE(uint8_t, unk3)
	GETTER_SETTER_DEFINE(Array, blend_list)
	
private:
	Ref<TRAnimationInfo> info;
	uint32_t res_1;
	Array blendshape_tracks;
	uint8_t unk3;
	Array blend_list;
};

class TRVisibilityShapeTimeline : public Resource {
    GDCLASS(TRVisibilityShapeTimeline, Resource)
protected:
	static void _bind_methods();
public:
	TRVisibilityShapeTimeline(){}
	~TRVisibilityShapeTimeline(){}

	GETTER_SETTER_DEFINE(float, time)
	GETTER_SETTER_DEFINE(float, value)
	GETTER_SETTER_DEFINE(Ref<TRTrackFlagsInfo>, info)
	
private:
	float time;
	float value;
	Ref<TRTrackFlagsInfo> info;
};

class TRTrackMaterialTimeline : public Resource {
    GDCLASS(TRTrackMaterialTimeline, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackMaterialTimeline(){}
	~TRTrackMaterialTimeline(){}

	GETTER_SETTER_DEFINE(Ref<TRAnimationInfo>, info)
	GETTER_SETTER_DEFINE(uint32_t, res_1)
	GETTER_SETTER_DEFINE(Array, material_tracks)
	GETTER_SETTER_DEFINE(uint8_t, unk3)
	GETTER_SETTER_DEFINE(uint8_t, unk4)

private:
	Ref<TRAnimationInfo> info;
	uint32_t res_1;
	Array material_tracks;
	uint8_t unk3;
	uint8_t unk4;
};

class TRMaterialAnimTrack : public Resource {
    GDCLASS(TRMaterialAnimTrack, Resource)
protected:
	static void _bind_methods();
public:
	TRMaterialAnimTrack(){}
	~TRMaterialAnimTrack(){}
	
    GETTER_SETTER_DEFINE(String, path)
	GETTER_SETTER_DEFINE(int, res_1)
	GETTER_SETTER_DEFINE(int, res_2)
	GETTER_SETTER_DEFINE(int, res_3)
	GETTER_SETTER_DEFINE(Ref<TRTrackMaterialTimeline>, mat_anim)
	GETTER_SETTER_DEFINE(Ref<TRVisibilityShapeTimeline>, vis_anim)
	GETTER_SETTER_DEFINE(Ref<TRBlendShapeTimeline>, blendshape_anim)

private:
	String path;
	int res_1;
	int res_2;
	int res_3;
    Ref<TRTrackMaterialTimeline> mat_anim;
	Ref<TRVisibilityShapeTimeline> vis_anim;
	Ref<TRBlendShapeTimeline> blendshape_anim;
};

class TRAnimationChannelMaterials : public Resource {
    GDCLASS(TRAnimationChannelMaterials, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationChannelMaterials(){}
	~TRAnimationChannelMaterials(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Ref<TRAnimationInfo>, info)
	GETTER_SETTER_DEFINE(Array, tracks)

private:
	Ref<TRAnimationInfo> info;
    Array tracks;
};

class ResourceFormatLoaderTRACM : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRACM, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRACM(){}
	~ResourceFormatLoaderTRACM(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}