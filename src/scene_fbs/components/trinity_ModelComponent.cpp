#include "trinity_ModelComponent.h"

using namespace godot;

void TrinityModelComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityModelComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityModelComponent, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityModelComponent::LoadFromBuffer(const void* buffer)
{
    auto modelComponent = Titan::TrinityScene::GetTrinityModelComponent(buffer);
    set_FilePath(Utils::toGodotString(modelComponent->file_path()));
    set_Name(Utils::toGodotString(modelComponent->name()));
}   