#include "trinity_LayoutComponent.h"

using namespace godot;

void TrinityLayoutComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityLayoutComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLayoutComponent, LayoutName, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityLayoutComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityLayoutComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityLayoutComponent");
    set_FilePath(Utils::toGodotString(component->file_path()));
    set_LayoutName(Utils::toGodotString(component->layout_name()));
}
