#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/uikit_shortcut_generated.h"
#include <utils.h>

namespace godot {

class UIKitShortcut : public Resource {
    GDCLASS(UIKitShortcut, Resource)
protected:
	static void _bind_methods();
public:
	UIKitShortcut(){}
	~UIKitShortcut(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(int, shortcutKey)
    GETTER_SETTER_DEFINE(bool, repeat)
    GETTER_SETTER_DEFINE(String, action)
    GETTER_SETTER_DEFINE(String, sfx)
    GETTER_SETTER_DEFINE(bool, sfx_apply)

private:
    String Name;
    int shortcutKey;
    bool repeat;
    String action;
    String sfx;
    bool sfx_apply;
};

}