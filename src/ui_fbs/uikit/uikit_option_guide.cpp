#include "uikit_option_guide.h"

using namespace godot;

void UIKitOptionGuide::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitOptionGuide, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void UIKitOptionGuide::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::pe::UIKit::GetUIKitOptionGuide(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse UIKitOptionGuide");

    set_Name(Utils::toGodotString(component->name()));
}
