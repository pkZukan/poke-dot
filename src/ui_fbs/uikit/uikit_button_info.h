#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "generated/uikit_grid_panel_generated.h"
#include <utils.h>

namespace godot {

class UIKitButtonInfo : public Resource {
    GDCLASS(UIKitButtonInfo, Resource)
protected:
    static void _bind_methods();
public:
    UIKitButtonInfo(){}
    ~UIKitButtonInfo(){}

    void LoadFromBuffer(const Titan::pe::UIKit::ButtonInfoTable* button_info);

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Vector2i, Pos)
    GETTER_SETTER_DEFINE(Vector2i, Size)

private:
    String Name;
    Vector2i Pos;
    Vector2i Size;
};

}
