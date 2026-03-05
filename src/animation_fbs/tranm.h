#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
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

class Framed8RotationTrack : public Resource {
	GDCLASS(Framed8RotationTrack, Resource)
protected:
	static void _bind_methods();
public:
	Framed8RotationTrack(){}
	~Framed8RotationTrack(){}

    GETTER_SETTER_DEFINE(Array, frames)
	GETTER_SETTER_DEFINE(Array, co)

private:
    Array frames;
	Array co;
};

class Framed16RotationTrack : public Resource {
	GDCLASS(Framed16RotationTrack, Resource)
protected:
	static void _bind_methods();
public:
	Framed16RotationTrack(){}
	~Framed16RotationTrack(){}

    GETTER_SETTER_DEFINE(Array, frames)
	GETTER_SETTER_DEFINE(Array, co)

private:
    Array frames;
	Array co;
};

class DynamicRotationTrack : public Resource {
	GDCLASS(DynamicRotationTrack, Resource)
protected:
	static void _bind_methods();
public:
	DynamicRotationTrack(){}
	~DynamicRotationTrack(){}

	GETTER_SETTER_DEFINE(Array, co)

private:
	Array co;
};

class FixedRotationTrack : public Resource {
	GDCLASS(FixedRotationTrack, Resource)
protected:
	static void _bind_methods();
public:
	FixedRotationTrack(){}
	~FixedRotationTrack(){}

	GETTER_SETTER_DEFINE(Quaternion, co)

private:
	Quaternion co;
};

class Framed8VectorTrack : public Resource {
	GDCLASS(Framed8VectorTrack, Resource)
protected:
	static void _bind_methods();
public:
	Framed8VectorTrack(){}
	~Framed8VectorTrack(){}

    GETTER_SETTER_DEFINE(Array, frames)
	GETTER_SETTER_DEFINE(Array, co)

private:
    Array frames;
	Array co;
};

class Framed16VectorTrack : public Resource {
	GDCLASS(Framed16VectorTrack, Resource)
protected:
	static void _bind_methods();
public:
	Framed16VectorTrack(){}
	~Framed16VectorTrack(){}

    GETTER_SETTER_DEFINE(Array, frames)
	GETTER_SETTER_DEFINE(Array, co)

private:
    Array frames;
	Array co;
};

class DynamicVectorTrack : public Resource {
	GDCLASS(DynamicVectorTrack, Resource)
protected:
	static void _bind_methods();
public:
	DynamicVectorTrack(){}
	~DynamicVectorTrack(){}

	GETTER_SETTER_DEFINE(Array, co)

private:
	Array co;
};

class FixedVectorTrack : public Resource {
	GDCLASS(FixedVectorTrack, Resource)
protected:
	static void _bind_methods();
public:
	FixedVectorTrack(){}
	~FixedVectorTrack(){}

	GETTER_SETTER_DEFINE(Vector3, co)

private:
	Vector3 co;
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

class AnimationInfo : public Resource {
	GDCLASS(AnimationInfo, Resource)
protected:
	static void _bind_methods();
public:
	AnimationInfo(){}
	~AnimationInfo(){}

	GETTER_SETTER_DEFINE(uint32_t, does_loop)
    GETTER_SETTER_DEFINE(uint32_t, animation_count)
    GETTER_SETTER_DEFINE(uint32_t, animation_rate)

private:
	uint32_t does_loop;
    uint32_t animation_count;
    uint32_t animation_rate;
};

class TRAnimation : public Resource {
    GDCLASS(TRAnimation, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimation(){}
	~TRAnimation(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(Ref<AnimationInfo>, Info)
    GETTER_SETTER_DEFINE(Ref<BoneAnimation>, Track)

private:
	Ref<Resource> ParseRotationTrack(Titan::Animation::RotationTrack type, const void* data);
	Ref<Resource> ParseVectorTrack(Titan::Animation::VectorTrack type, const void* data);

    Ref<AnimationInfo> Info;
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