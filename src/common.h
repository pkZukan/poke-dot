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

class RGBA : public Resource {
	GDCLASS(RGBA, Resource)
protected:
	static void _bind_methods();
public:
	RGBA(){}
	~RGBA(){}
	
	GETTER_SETTER_DEFINE(float, R)
	GETTER_SETTER_DEFINE(float, G)
    GETTER_SETTER_DEFINE(float, B)
	GETTER_SETTER_DEFINE(float, A)

private:
	float R, G, B, A;
};
}