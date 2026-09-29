#include "trinity_GluePlugin.h"

using namespace godot;

void TrinityGluePlugin::_bind_methods()
{
}

void TrinityGluePlugin::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityGluePlugin(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityGluePlugin");
}
