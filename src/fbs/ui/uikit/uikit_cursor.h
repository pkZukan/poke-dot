#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "generated/Ikkaku/pe/UIKit/uikit_cursor_generated.h"
#include <utils.h>

namespace godot
{

class UIKitCursor : public Resource
{
    GDCLASS(UIKitCursor, Resource)
protected:
    static void _bind_methods();

public:
    UIKitCursor() = default;
    ~UIKitCursor() = default;

    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, ControlName)
    GETTER_SETTER_DEFINE(int, ControlIndex)

private:
    String Name;
    String ControlName;
    int ControlIndex = 0;
};

}
