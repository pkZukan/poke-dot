#include "trinity_CollisionComponent.h"

using namespace godot;

void TrinityCollisionComponent::_bind_methods()
{
}

void TrinityCollisionComponent::LoadFromBuffer(const void* buffer)
{
    auto collision_component = Titan::TrinityScene::GetTrinityCollisionComponent(buffer);
    ERR_FAIL_COND_MSG(collision_component == nullptr, "Couldn't parse collision component");
}