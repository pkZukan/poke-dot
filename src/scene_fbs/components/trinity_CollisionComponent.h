#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_CollisionComponent_generated.h"
#include <utils.h>

namespace godot {

class TrinityCollisionComponent : public Resource {
    GDCLASS(TrinityCollisionComponent, Resource)
protected:
	static void _bind_methods();
public:
	TrinityCollisionComponent(){}
	~TrinityCollisionComponent(){}

    void LoadFromBuffer(const void* buffer);
};

}