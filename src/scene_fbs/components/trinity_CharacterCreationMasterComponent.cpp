#include "trinity_CharacterCreationMasterComponent.h"

using namespace godot;

void TrinityCharacterCreationMasterComponent::_bind_methods()
{
}

void TrinityCharacterCreationMasterComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityCharacterCreationMasterComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityCharacterCreationMasterComponent");
}
