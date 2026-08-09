#include "uikit_body.h"

using namespace godot;

void UIKitBody::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitBody, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(UIKitBody, ControlName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitBody, ControlIndex, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitBody, InSfx, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitBody, OutSfx, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitBody, InSfxApply, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitBody, OutSfxApply, Variant::BOOL, PROPERTY_HINT_NONE)
}

void UIKitBody::LoadFromBuffer(const void* buffer)
{
    auto body = Titan::pe::UIKit::GetUIKitBody(buffer);
    ERR_FAIL_COND_MSG(body == nullptr, "Couldn't parse UIkitBody");

    set_Name(Utils::toGodotString(body->name()));
    set_ControlName(Utils::toGodotString(body->control_name()));
    set_ControlIndex(body->control_index());
    set_InSfx(Utils::toGodotString(body->in_sfx()));
    set_OutSfx(Utils::toGodotString(body->out_sfx()));
    set_InSfxApply(body->in_sfx_apply());
    set_OutSfxApply(body->out_sfx_apply());
}