#include "trinity_OverrideSensorData.h"

using namespace godot;

void TrinityOverrideSensorData::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityOverrideSensorData, RealizingDistance, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityOverrideSensorData, UnrealizingDistance, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityOverrideSensorData, LoadingDistance, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityOverrideSensorData, UnloadingDistance, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void TrinityOverrideSensorData::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityOverrideSensorData(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityOverrideSensorData");
    set_RealizingDistance(component->realizing_distance());
    set_UnrealizingDistance(component->unrealizing_distance());
    set_LoadingDistance(component->loading_distance());
    set_UnloadingDistance(component->unloading_distance());
}
