#include "trinity_LightDirectApplierComponent.h"

using namespace godot;

void TrinityLightDirectApplierComponent::_bind_methods()
{
}

void TrinityLightDirectApplierComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityLightDirectApplierComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityLightDirectApplierComponent");
}
