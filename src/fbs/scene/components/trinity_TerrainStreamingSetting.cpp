#include "trinity_TerrainStreamingSetting.h"

using namespace godot;

void TrinityTerrainStreamingSetting::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityTerrainStreamingSetting, LowLoadRadius, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityTerrainStreamingSetting, MediumLoadRadius, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityTerrainStreamingSetting, HighLoadRadius, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityTerrainStreamingSetting, CollisionLoadRadius, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityTerrainStreamingSetting, TreeLoadRadius, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void TrinityTerrainStreamingSetting::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityTerrainStreamingSetting(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityTerrainStreamingSetting");
    set_LowLoadRadius(component->low_load_radius());
    set_MediumLoadRadius(component->medium_load_radius());
    set_HighLoadRadius(component->high_load_radius());
    set_CollisionLoadRadius(component->collision_load_radius());
    set_TreeLoadRadius(component->tree_load_radius());
}
