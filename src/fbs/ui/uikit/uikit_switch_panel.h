#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "generated/uikit_switch_panel_generated.h"
#include <utils.h>

namespace godot {

class UIKitSwitchPanel : public Resource {
    GDCLASS(UIKitSwitchPanel, Resource)
protected:
    static void _bind_methods();
public:
    UIKitSwitchPanel(){}
    ~UIKitSwitchPanel(){}

    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(String, Name)

private:
    String Name;
};

}
