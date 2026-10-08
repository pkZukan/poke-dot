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

class UIKitGridPanel : public Resource {
    GDCLASS(UIKitGridPanel, Resource)
protected:
    static void _bind_methods();
public:
    UIKitGridPanel(){}
    ~UIKitGridPanel(){}

    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, CursorName)
    GETTER_SETTER_DEFINE(Vector2i, GridSize)
    GETTER_SETTER_DEFINE(int, Mode)
    GETTER_SETTER_DEFINE(Array, ButtonInfo)

private:
    String Name;
    String CursorName;
    Vector2i GridSize;
    int Mode = 0;
    Array ButtonInfo;
};

}
