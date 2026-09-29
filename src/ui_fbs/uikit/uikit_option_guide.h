#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "generated/uikit_option_guide_generated.h"
#include <utils.h>

namespace godot {

class UIKitOptionGuide : public Resource {
    GDCLASS(UIKitOptionGuide, Resource)
protected:
    static void _bind_methods();
public:
    UIKitOptionGuide(){}
    ~UIKitOptionGuide(){}

    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(String, Name)

private:
    String Name;
};

}
