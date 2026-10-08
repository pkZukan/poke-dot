#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "generated/uikit_scroll_panel_generated.h"
#include <utils.h>

namespace godot {

class UIKitScrollPanel : public Resource {
    GDCLASS(UIKitScrollPanel, Resource)
protected:
    static void _bind_methods();
public:
    UIKitScrollPanel(){}
    ~UIKitScrollPanel(){}

    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(String, Name)

private:
    String Name;
};

}
