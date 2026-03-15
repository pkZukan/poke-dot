#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "animation_fbs/anim_common.h"
#include "generated/tranm_generated.h"
#include <utils.h>

namespace godot {

class QuaternionHelper
{
private:
	static constexpr int32_t SCALE = 0x7FFF;
    static constexpr float PI_QUARTER = Math_PI / 4.0f;
    static constexpr float PI_HALF = Math_PI / 2.0f;

	static float expand_float(int32_t i);

public:
    static Quaternion Unpack(const Vector3i &vec);
};

class BoneTrack : public Resource {
	GDCLASS(BoneTrack, Resource)
protected:
	static void _bind_methods();
public:
	BoneTrack(){}
	~BoneTrack(){}

	GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Ref<Resource>, scale)
    GETTER_SETTER_DEFINE(Ref<Resource>, rotate)
    GETTER_SETTER_DEFINE(Ref<Resource>, translate)

private:
	String Name;
    Ref<Resource> scale;
    Ref<Resource> rotate;
    Ref<Resource> translate;
};

class BoneInit : public Resource {
	GDCLASS(BoneInit, Resource)
protected:
	static void _bind_methods();
public:
	BoneInit(){}
	~BoneInit(){}

	GETTER_SETTER_DEFINE(uint32_t, isInit)
    GETTER_SETTER_DEFINE(Transform3D, transform)

private:
	uint32_t isInit;
    Transform3D transform;
};

class BoneAnimation : public Resource {
	GDCLASS(BoneAnimation, Resource)
protected:
	static void _bind_methods();
public:
	BoneAnimation(){}
	~BoneAnimation(){}

	GETTER_SETTER_DEFINE(Array, tracks)
    GETTER_SETTER_DEFINE(Ref<BoneInit>, initData)

private:
	Array tracks;
    Ref<BoneInit> initData;
};

class TRAnimation : public Resource {
    GDCLASS(TRAnimation, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimation(){}
	~TRAnimation(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Ref<TRAnimationInfo>, Info)
    GETTER_SETTER_DEFINE(Ref<BoneAnimation>, Track)

private:
	Ref<Resource> ParseRotationTrack(Titan::Animation::RotationTrack type, const void* data);
	Ref<Resource> ParseVectorTrack(Titan::Animation::VectorTrack type, const void* data);

    Ref<TRAnimationInfo> Info;
    Ref<BoneAnimation> Track;
};

class ResourceFormatLoaderTRANM : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRANM, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRANM(){}
	~ResourceFormatLoaderTRANM(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}