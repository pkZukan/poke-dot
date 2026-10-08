#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/uikit_body_generated.h"
#include <utils.h>

namespace godot {

class UIKitBody : public Resource {
    GDCLASS(UIKitBody, Resource)
protected:
	static void _bind_methods();
public:
	UIKitBody(){}
	~UIKitBody(){}

    void LoadFromBuffer(const void* buffer);
	
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(String, ControlName)
    GETTER_SETTER_DEFINE(int, ControlIndex)
    GETTER_SETTER_DEFINE(String, InSfx)
    GETTER_SETTER_DEFINE(String, OutSfx)
    GETTER_SETTER_DEFINE(bool, InSfxApply)
    GETTER_SETTER_DEFINE(bool, OutSfxApply)

private:
    String Name;
    String ControlName;
    int ControlIndex;
    String InSfx;
    String OutSfx;
    bool InSfxApply;
    bool OutSfxApply;
};

}