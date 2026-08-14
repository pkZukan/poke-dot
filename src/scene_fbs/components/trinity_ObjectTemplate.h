#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_ObjectTemplate_generated.h"
#include <scene_fbs/trinity_scene_parser.h>
#include <utils.h>

namespace godot {

class TrinityObjectTemplate : public Resource {
    GDCLASS(TrinityObjectTemplate, Resource)
protected:
	static void _bind_methods();
public:
	TrinityObjectTemplate(){}
	~TrinityObjectTemplate(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, Scope)
    GETTER_SETTER_DEFINE(String, FilePath)
    GETTER_SETTER_DEFINE(bool, IsExpanded)
    GETTER_SETTER_DEFINE(String, EntityType)
    GETTER_SETTER_DEFINE(Ref<Resource>, EntityData)

private:
    String Name;
    String Scope;
    String FilePath;
    bool IsExpanded;
    String EntityType;
    Ref<Resource> EntityData;
};

}