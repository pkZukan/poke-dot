#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/uikit_switch_generated.h"
#include <utils.h>

namespace godot {

class UIKitSwitch : public Resource {
    GDCLASS(UIKitSwitch, Resource)
protected:
	static void _bind_methods();
public:
	UIKitSwitch(){}
	~UIKitSwitch(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, ControlName)

private:
    String Name;
    String ControlName;
};

}