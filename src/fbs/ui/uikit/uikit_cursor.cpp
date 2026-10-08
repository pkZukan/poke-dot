#include "uikit_cursor.h"

using namespace godot;

void UIKitCursor::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitCursor, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitCursor, ControlName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitCursor, ControlIndex, Variant::INT, PROPERTY_HINT_NONE)
}

void UIKitCursor::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::pe::UIKit::GetUIKitCursor(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse UIKitCursor");

    set_Name(Utils::toGodotString(component->name()));
    set_ControlName(Utils::toGodotString(component->control_name()));
    set_ControlIndex(component->control_index());
}
