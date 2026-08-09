#include "uikit_shortcut.h"

using namespace godot;

void UIKitShortcut::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitShortcut, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(UIKitShortcut, shortcutKey, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitShortcut, repeat, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitShortcut, action, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitShortcut, sfx, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitShortcut, sfx_apply, Variant::BOOL, PROPERTY_HINT_NONE)
}

void UIKitShortcut::LoadFromBuffer(const void* buffer)
{
    auto shortcut = Titan::pe::UIKit::GetUIKitShortcut(buffer);
    ERR_FAIL_COND_MSG(shortcut == nullptr, "Couldn't parse UIkitShortcut");

    set_Name(Utils::toGodotString(shortcut->name()));
    set_shortcutKey(shortcut->shortcutKey());
    set_repeat(shortcut->repeat());
    set_action(Utils::toGodotString(shortcut->action()));
    set_sfx(Utils::toGodotString(shortcut->sfx()));
    set_sfx_apply(shortcut->sfx_apply());
}