#include "uikit_grid_panel.h"

using namespace godot;

void UIKitGridPanel::_bind_methods()
{
    GETTER_SETTER_BIND(UIKitGridPanel, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitGridPanel, CursorName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitGridPanel, GridSize, Variant::VECTOR2I, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitGridPanel, Mode, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(UIKitGridPanel, ButtonInfo, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "UIKitButtonInfo")
}

void UIKitGridPanel::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::pe::UIKit::GetUIKitGridPanel(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse UIKitGridPanel");

    set_Name(Utils::toGodotString(component->name()));
    set_CursorName(Utils::toGodotString(component->cursor_name()));
    set_GridSize(component->grid_size() ? Vector2i(component->grid_size()->x(), component->grid_size()->y()) : Vector2i());
    set_Mode(component->mode());
    Array buttonInfoArray;
    if (auto entries = component->button_info())
    {
        for (flatbuffers::uoffset_t i = 0; i < entries->size(); i++)
        {
            Ref<UIKitButtonInfo> buttonInfo;
            buttonInfo.instantiate();
            buttonInfo->LoadFromBuffer(entries->Get(i));
            buttonInfoArray.push_back(buttonInfo);
        }
    }
    set_ButtonInfo(buttonInfoArray);
}
