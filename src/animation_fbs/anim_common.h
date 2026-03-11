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
}