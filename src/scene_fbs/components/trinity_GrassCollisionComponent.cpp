#include "trinity_GrassCollisionComponent.h"

using namespace godot;

void TrinityGrassCollisionComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityGrassCollisionComponent, name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityGrassCollisionComponent, min, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityGrassCollisionComponent, max, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void TrinityGrassCollisionComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityGrassCollisionComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityGrassCollisionComponent");
    set_name(Utils::toGodotString(component->name()));
    set_min(component->min());
    set_max(component->max());
}
