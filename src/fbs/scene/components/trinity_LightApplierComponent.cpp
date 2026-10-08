#include "trinity_LightApplierComponent.h"

using namespace godot;

void TrinityLightApplierComponent::_bind_methods()
{
}

void TrinityLightApplierComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityLightApplierComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityLightApplierComponent");
}
