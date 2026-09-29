#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "generated/uikit_switch_item_generated.h"
#include <utils.h>

namespace godot {

class UIKitSwitchItem : public Resource {
    GDCLASS(UIKitSwitchItem, Resource)
protected:
    static void _bind_methods();
public:
    UIKitSwitchItem(){}
    ~UIKitSwitchItem(){}

    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, ControlName)
    GETTER_SETTER_DEFINE(int, ControlIndex)
    GETTER_SETTER_DEFINE(String, ActionName)

private:
    String Name;
    String ControlName;
    int ControlIndex = 0;
    String ActionName;
};

}
