#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_AnimationComponent_generated.h"
#include <utils.h>

namespace godot {

class TrinityAnimationComponent : public Resource {
    GDCLASS(TrinityAnimationComponent, Resource)
protected:
	static void _bind_methods();
public:
	TrinityAnimationComponent(){}
	~TrinityAnimationComponent(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, FilePath)
    GETTER_SETTER_DEFINE(String, Name)

private:
    String FilePath;
    String Name;
    //TODO: other fields
};

}