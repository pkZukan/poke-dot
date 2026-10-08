#include "trinity_EnvironmentParameter.h"

using namespace godot;

void TrinityEnvironmentParameter::_bind_methods()
{
}

void TrinityEnvironmentParameter::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityEnvironmentParameter(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityEnvironmentParameter");
}
