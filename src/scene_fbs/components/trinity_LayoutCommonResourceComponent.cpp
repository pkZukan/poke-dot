#include "trinity_LayoutCommonResourceComponent.h"

using namespace godot;

void TrinityLayoutCommonResourceComponent::_bind_methods()
{
}

void TrinityLayoutCommonResourceComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityLayoutCommonResourceComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityLayoutCommonResourceComponent");
}
