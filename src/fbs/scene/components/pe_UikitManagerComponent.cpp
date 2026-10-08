#include "pe_UikitManagerComponent.h"

using namespace godot;

void PeUikitManagerComponent::_bind_methods()
{
}

void PeUikitManagerComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetPeUikitManagerComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse PeUikitManagerComponent");
}
