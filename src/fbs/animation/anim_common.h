#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/anim_common_generated.h"
#include <utils.h>

namespace godot {

class TRAnimationInfo : public Resource {
	GDCLASS(TRAnimationInfo, Resource)
protected:
	static void _bind_methods();
public:
	TRAnimationInfo(){}
	~TRAnimationInfo(){}

	GETTER_SETTER_DEFINE(uint32_t, does_loop)
    GETTER_SETTER_DEFINE(uint32_t, animation_count)
    GETTER_SETTER_DEFINE(uint32_t, animation_rate)

private:
	uint32_t does_loop;
    uint32_t animation_count;
    uint32_t animation_rate;
};

#define DEFINE_FIXED_TRACK(ClassName, ValueType, VariantType) \
class ClassName : public Resource { \
    GDCLASS(ClassName, Resource) \
protected: \
    static void _bind_methods() { \
        GETTER_SETTER_BIND(ClassName, value, VariantType, PROPERTY_HINT_NONE) \
    } \
public: \
    ClassName(){} \
    ~ClassName(){} \
    GETTER_SETTER_DEFINE(ValueType, value) \
private: \
    ValueType value; \
};

#define DEFINE_DYNAMIC_TRACK(ClassName, HintString) \
class ClassName : public Resource { \
    GDCLASS(ClassName, Resource) \
protected: \
    static void _bind_methods() { \
        GETTER_SETTER_BIND(ClassName, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, HintString) \
    } \
public: \
    ClassName(){} \
    ~ClassName(){} \
    GETTER_SETTER_DEFINE(Array, values) \
private: \
    Array values; \
};

#define DEFINE_FRAMED8_TRACK(ClassName, HintString) \
class ClassName : public Resource { \
    GDCLASS(ClassName, Resource) \
protected: \
    static void _bind_methods() { \
        GETTER_SETTER_BIND(ClassName, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int") \
        GETTER_SETTER_BIND(ClassName, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, HintString) \
    } \
public: \
    ClassName(){} \
    ~ClassName(){} \
    GETTER_SETTER_DEFINE(Array, frames) \
    GETTER_SETTER_DEFINE(Array, values) \
private: \
    Array frames; \
    Array values; \
};

#define DEFINE_FRAMED16_TRACK(ClassName, HintString) \
class ClassName : public Resource { \
    GDCLASS(ClassName, Resource) \
protected: \
    static void _bind_methods() { \
        GETTER_SETTER_BIND(ClassName, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int") \
        GETTER_SETTER_BIND(ClassName, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, HintString) \
    } \
public: \
    ClassName(){} \
    ~ClassName(){} \
    GETTER_SETTER_DEFINE(Array, frames) \
    GETTER_SETTER_DEFINE(Array, values) \
private: \
    Array frames; \
    Array values; \
};

DEFINE_FIXED_TRACK(FixedFloatTrack, float, Variant::FLOAT)
DEFINE_DYNAMIC_TRACK(DynamicFloatTrack, "float")
DEFINE_FRAMED8_TRACK(Framed8FloatTrack, "float")
DEFINE_FRAMED16_TRACK(Framed16FloatTrack, "float")

DEFINE_FIXED_TRACK(FixedBoolTrack, bool, Variant::BOOL)
DEFINE_DYNAMIC_TRACK(DynamicBoolTrack, "bool")
DEFINE_FRAMED8_TRACK(Framed8BoolTrack, "bool")
DEFINE_FRAMED16_TRACK(Framed16BoolTrack, "bool")

DEFINE_FIXED_TRACK(FixedRotationTrack, Quaternion, Variant::QUATERNION)
DEFINE_DYNAMIC_TRACK(DynamicRotationTrack, "Quaternion")
DEFINE_FRAMED8_TRACK(Framed8RotationTrack, "Quaternion")
DEFINE_FRAMED16_TRACK(Framed16RotationTrack, "Quaternion")

DEFINE_FIXED_TRACK(FixedVectorTrack, Vector3, Variant::VECTOR3)
DEFINE_DYNAMIC_TRACK(DynamicVectorTrack, "Vector3")
DEFINE_FRAMED8_TRACK(Framed8VectorTrack, "Vector3")
DEFINE_FRAMED16_TRACK(Framed16VectorTrack, "Vector3")

}