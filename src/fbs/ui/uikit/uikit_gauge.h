#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/uikit_gauge_generated.h"
#include <utils.h>

namespace godot {

class UIKitGauge : public Resource {
    GDCLASS(UIKitGauge, Resource)
protected:
	static void _bind_methods();
public:
	UIKitGauge(){}
	~UIKitGauge(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, ControlName)
    GETTER_SETTER_DEFINE(int, ControlIndex)
    GETTER_SETTER_DEFINE(int, Type)

private:
    String Name;
    String ControlName;
    int ControlIndex;
    int Type;
};

}