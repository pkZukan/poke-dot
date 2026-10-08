#include "uikit_switch.h"

using namespace godot;

void UIKitSwitch::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitSwitch, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(UIKitSwitch, ControlName, Variant::STRING, PROPERTY_HINT_NONE)
}

void UIKitSwitch::LoadFromBuffer(const void* buffer)
{
    auto gauge = Titan::pe::UIKit::GetUIKitSwitch(buffer);
    ERR_FAIL_COND_MSG(gauge == nullptr, "Couldn't parse UIkitSwitch");

    set_Name(Utils::toGodotString(gauge->name()));
    set_ControlName(Utils::toGodotString(gauge->controlName()));
}