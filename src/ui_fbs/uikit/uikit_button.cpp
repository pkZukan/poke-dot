#include "uikit_button.h"

using namespace godot;

void UIKitButton::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitButton, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(UIKitButton, ControlName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButton, ControlIndex, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButton, Action, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButton, ShortcutKey, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButton, Repeat, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButton, DecideSfx, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButton, ShortcutSfx, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitButton, ReleaseSfx, Variant::STRING, PROPERTY_HINT_NONE)
}

void UIKitButton::LoadFromBuffer(const void* buffer)
{
    auto button = Titan::pe::UIKit::GetUIKitButton(buffer);
    ERR_FAIL_COND_MSG(button == nullptr, "Couldn't parse UIkitButton");

    set_Name(Utils::toGodotString(button->name()));
    set_ControlName(Utils::toGodotString(button->control_name()));
    set_ControlIndex(button->control_index());
    set_Action(Utils::toGodotString(button->action()));
    set_ShortcutKey(button->shortcut_key());
    set_Repeat(button->repeat());
    set_DecideSfx(Utils::toGodotString(button->decide_sfx()));
    set_ShortcutSfx(Utils::toGodotString(button->shortcut_sfx()));
    set_ReleaseSfx(Utils::toGodotString(button->release_sfx()));
}