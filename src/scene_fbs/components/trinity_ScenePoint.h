#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_ScenePoint_generated.h"
#include <utils.h>

namespace godot {

class TrinityScenePoint : public Resource {
    GDCLASS(TrinityScenePoint, Resource)
protected:
	static void _bind_methods();
public:
	TrinityScenePoint(){}
	~TrinityScenePoint(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Vector3, Point)
    GETTER_SETTER_DEFINE(bool, AttachParent)

private:
    String Name;
    Vector3 Point;
    bool AttachParent;
};

}