#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_ModelInstancerComponent_generated.h"
#include <utils.h>

namespace godot {

class TrinityModelInstancerComponent : public Resource {
    GDCLASS(TrinityModelInstancerComponent, Resource)
protected:
    static void _bind_methods();
public:
    TrinityModelInstancerComponent(){}
    ~TrinityModelInstancerComponent(){}

    void LoadFromBuffer(const void* buffer);
    
    GETTER_SETTER_DEFINE(String, FilePath)
private:
    String FilePath;
};
}