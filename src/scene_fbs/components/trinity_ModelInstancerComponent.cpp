#include "trinity_ModelInstancerComponent.h"

using namespace godot;

void TrinityModelInstancerComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityModelInstancerComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityModelInstancerComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityModelInstancerComponent(buffer);
    set_FilePath(Utils::toGodotString(component->file_path()));
}