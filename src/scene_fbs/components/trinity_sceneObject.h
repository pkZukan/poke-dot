#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_sceneObject_generated.h"
#include <utils.h>

namespace godot {

class TrinityObjectLayer : public Resource {
    GDCLASS(TrinityObjectLayer, Resource)
protected:
	static void _bind_methods();
public:
	TrinityObjectLayer(){}
	~TrinityObjectLayer(){}
	
    GETTER_SETTER_DEFINE(int, Priority)
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Transform3D, transform)

private:
    int Priority;
    String Name;
    Transform3D transform;
};

class TrinitySceneObject : public Resource {
    GDCLASS(TrinitySceneObject, Resource)
protected:
	static void _bind_methods();
public:
	TrinitySceneObject(){}
	~TrinitySceneObject(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Transform3D, transform)
    GETTER_SETTER_DEFINE(bool, keep_world_srt)
    GETTER_SETTER_DEFINE(bool, attach_xform)
    GETTER_SETTER_DEFINE(String, attach_joint)
    GETTER_SETTER_DEFINE(bool, scriptable)
    GETTER_SETTER_DEFINE(int, Priority)
    GETTER_SETTER_DEFINE(Array, Layers)
    GETTER_SETTER_DEFINE(Array, TagList)

private:
    String Name;
    Transform3D transform;
    bool keep_world_srt;
    bool attach_xform;
    String attach_joint;
    bool scriptable;
    int Priority;
    Array Layers;
    Array TagList;
};

}