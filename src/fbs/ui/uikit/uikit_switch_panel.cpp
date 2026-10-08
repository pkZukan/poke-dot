#include "uikit_switch_panel.h"

using namespace godot;

void UIKitSwitchPanel::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitSwitchPanel, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void UIKitSwitchPanel::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::pe::UIKit::GetUIKitSwitchPanel(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse UIKitSwitchPanel");

    set_Name(Utils::toGodotString(component->name()));
}
