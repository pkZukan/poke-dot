#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "model/common.h"

#define GETTER_SETTER_BIND(c, x, y, ...)  \
	ClassDB::bind_method(D_METHOD("get_"#x), &c::get_##x); \
	ClassDB::bind_method(D_METHOD("set_"#x), &c::set_##x); \
	ADD_PROPERTY(PropertyInfo(y, #x, __VA_ARGS__), "set_"#x, "get_"#x);

#define GETTER_SETTER_DEFINE(x, y) \
	x get_##y() { return y; } \
	void set_##y(x val) { y = val; }

#define Bit(n)  ((uint32_t) 1 << (n))
#define Mask(n) (Bit(n) - 1)

namespace godot {

class Utils : public Resource {
	GDCLASS(Utils, Resource)
protected:
	static void _bind_methods();
public:
	Utils(){}
	~Utils(){}

    static godot::Vector3 toGodotVec3(const Titan::Model::Vec3 *vec);
    static godot::String toGodotString(const flatbuffers::String *str);

    static float half_to_float(uint16_t half);
};
}