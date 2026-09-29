#include "uikit_switch_item.h"

using namespace godot;

void UIKitSwitchItem::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitSwitchItem, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitSwitchItem, ControlName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitSwitchItem, ControlIndex, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitSwitchItem, ActionName, Variant::STRING, PROPERTY_HINT_NONE)
}

void UIKitSwitchItem::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::pe::UIKit::GetUIKitSwitchItem(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse UIKitSwitchItem");

    set_Name(Utils::toGodotString(component->name()));
    set_ControlName(Utils::toGodotString(component->control_name()));
    set_ControlIndex(component->control_index());
    set_ActionName(Utils::toGodotString(component->action_name()));
}
