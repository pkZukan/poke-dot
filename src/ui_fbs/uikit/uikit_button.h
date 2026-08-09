#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/uikit_button_generated.h"
#include <utils.h>

namespace godot {

class UIKitButton : public Resource {
    GDCLASS(UIKitButton, Resource)
protected:
	static void _bind_methods();
public:
	UIKitButton(){}
	~UIKitButton(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, ControlName)
    GETTER_SETTER_DEFINE(int, ControlIndex)
    GETTER_SETTER_DEFINE(String, Action)
    GETTER_SETTER_DEFINE(int, ShortcutKey)
    GETTER_SETTER_DEFINE(bool, Repeat)
    GETTER_SETTER_DEFINE(String, DecideSfx)
    GETTER_SETTER_DEFINE(String, ShortcutSfx)
    GETTER_SETTER_DEFINE(String, ReleaseSfx)

private:
    String Name;
    String ControlName;
    int ControlIndex;
    String Action;
    int ShortcutKey;
    bool Repeat;
    String DecideSfx;
    String ShortcutSfx;
    String ReleaseSfx;
};

}