#include "uikit_scroll_panel.h"

using namespace godot;

void UIKitScrollPanel::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitScrollPanel, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void UIKitScrollPanel::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::pe::UIKit::GetUIKitScrollPanel(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse UIKitScrollPanel");

    set_Name(Utils::toGodotString(component->name()));
}
