#include "uikit_gauge.h"

using namespace godot;

void UIKitGauge::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitGauge, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(UIKitGauge, ControlName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitGauge, ControlIndex, Variant::INT, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(UIKitGauge, Type, Variant::INT, PROPERTY_HINT_NONE)
}

void UIKitGauge::LoadFromBuffer(const void* buffer)
{
    auto gauge = Titan::pe::UIKit::GetUIKitGauge(buffer);
    ERR_FAIL_COND_MSG(gauge == nullptr, "Couldn't parse UIkitGauge");

    set_Name(Utils::toGodotString(gauge->name()));
    set_ControlName(Utils::toGodotString(gauge->control_name()));
    set_ControlIndex(gauge->control_index());
    set_Type(gauge->type());
}