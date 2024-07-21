#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "utils.h"

namespace godot {

class BoundingBox : public Resource {
	GDCLASS(BoundingBox, Resource)
protected:
	static void _bind_methods();
public:
	BoundingBox(){}
	~BoundingBox(){}
	
	GETTER_SETTER_DEFINE(Vector3, Min)
	GETTER_SETTER_DEFINE(Vector3, Max)

private:
	Vector3 Min, Max;
};
}