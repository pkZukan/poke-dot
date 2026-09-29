#include "uikit_button_info.h"

using namespace godot;

void UIKitButtonInfo::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitButtonInfo, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButtonInfo, Pos, Variant::VECTOR2I, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButtonInfo, Size, Variant::VECTOR2I, PROPERTY_HINT_NONE)
}

void UIKitButtonInfo::LoadFromBuffer(const Titan::pe::UIKit::ButtonInfoTable* button_info)
{
    ERR_FAIL_COND_MSG(button_info == nullptr, "Couldn't parse UIKitButtonInfo");

    set_Name(Utils::toGodotString(button_info->name()));
    set_Pos(button_info->pos() ? Vector2i(button_info->pos()->x(), button_info->pos()->y()) : Vector2i());
    set_Size(button_info->size() ? Vector2i(button_info->size()->x(), button_info->size()->y()) : Vector2i());
}
