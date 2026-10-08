#include "trinity_TextureBufferComponent.h"

using namespace godot;

void TrinityTextureBufferComponent::_bind_methods()
{
}

void TrinityTextureBufferComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityTextureBufferComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityTextureBufferComponent");
}
