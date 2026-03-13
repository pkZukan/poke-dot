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

	GETTER_SETTER_DEFINE(bool, value)

private:
	bool value;
};

class TRBlendTable : public Resource {
    GDCLASS(TRBlendTable, Resource)
protected:
	static void _bind_methods();
public:
	TRBlendTable(){}
	~TRBlendTable(){}
	
	GETTER_SETTER_DEFINE(String, Name)

private:
	String Name;
};

class TRTrackMaterialValueList : public Resource {
    GDCLASS(TRTrackMaterialValueList, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackMaterialValueList(){}
	~TRTrackMaterialValueList(){}
	
	GETTER_SETTER_DEFINE(Array, values)

private:
	Array values;
};

class TRTrackMaterialChannels : public Resource {
    GDCLASS(TRTrackMaterialChannels, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackMaterialChannels(){}
	~TRTrackMaterialChannels(){}
	
	GETTER_SETTER_DEFINE(String, Name)

private:
	String Name;
};

class TRTrackBlendShape : public Resource {
    GDCLASS(TRTrackBlendShape, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackBlendShape(){}
	~TRTrackBlendShape(){}
	
	GETTER_SETTER_DEFINE(String, Name)

private:
	String Name;
};

class TRTrackFlagsInfo : public Resource {
    GDCLASS(TRTrackFlagsInfo, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackFlagsInfo(){}
	~TRTrackFlagsInfo(){}
	
	GETTER_SETTER_DEFINE(Ref<Resource>, values)

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
	GETTER_SETTER_DEFINE(uint8_t, res_3)
	GETTER_SETTER_DEFINE(Array, blend_list)
	
private:
	Ref<TRAnimationInfo> info;
	uint32_t res_1;
	Array blendshape_tracks;
	uint8_t res_3;
	Array blend_list;
};

class TRTrackMaterialValue : public Resource {
    GDCLASS(TRTrackMaterialValue, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackMaterialValue(){}
	~TRTrackMaterialValue(){}

	GETTER_SETTER_DEFINE(float, Time)
	GETTER_SETTER_DEFINE(float, Value)
	GETTER_SETTER_DEFINE(uint32_t, config_0)
	GETTER_SETTER_DEFINE(uint32_t, config_1)
	GETTER_SETTER_DEFINE(uint32_t, config_2)
	
private:
	float Time;
	float Value;
	uint32_t config_0;
	uint32_t config_1;
	uint32_t config_2;
};

class TRTrackMaterialInit : public Resource {
    GDCLASS(TRTrackMaterialInit, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackMaterialInit(){}
	~TRTrackMaterialInit(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(Ref<TRTrackMaterialValueList>, list)
	
private:
	String Name;
	Ref<TRTrackMaterialValueList> list;
};

class TRTrackMaterialAnim : public Resource {
    GDCLASS(TRTrackMaterialAnim, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackMaterialAnim(){}
	~TRTrackMaterialAnim(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(Array, list)
	
private:
	String Name;
	Array list;
};

class TRTrackMaterial : public Resource {
    GDCLASS(TRTrackMaterial, Resource)
protected:
	static void _bind_methods();
public:
	TRTrackMaterial(){}
	~TRTrackMaterial(){}

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(Array, init_values)
	GETTER_SETTER_DEFINE(Array, anim_values)
	
private:
	String Name;
	Array init_values;
	Array anim_values;
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

class TRMeshAnimeTrack : public Resource {
    GDCLASS(TRMeshAnimeTrack, Resource)
protected:
	static void _bind_methods();
public:
	TRMeshAnimeTrack(){}
	~TRMeshAnimeTrack(){}
	
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

class TRAnimationChannelMeshes : public Resource {
    GDCLASS(TRAnimationChannelMeshes, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationChannelMeshes(){}
	~TRAnimationChannelMeshes(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Ref<TRAnimationInfo>, info)
	GETTER_SETTER_DEFINE(Array, tracks)

private:
	Ref<TRTrackMaterialTimeline> _LoadMaterialAnims(const Titan::Animation::TrackMaterialTimeline *matAnim);
	Ref<TRVisibilityShapeTimeline> _LoadVisibilityAnims(const Titan::Animation::VisibilityShapeTimeline *visAnim);
	Ref<TRBlendShapeTimeline> _LoadBlendshapeAnims(const Titan::Animation::BlendShapeTimeline *blendAnim);

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