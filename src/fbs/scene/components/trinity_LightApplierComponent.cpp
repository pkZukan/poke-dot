#include "trinity_LightApplierComponent.h"

using namespace godot;

void TrinityLightApplierComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityLightApplierComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, TimeSpan, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, CurrentTime, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, StartWithOffsetData, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, OffsetDataName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, OffsetBlendWeight, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, ApplyGameObjectSRT, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, Priority, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, UniqueFlag, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityLightApplierComponent, UseInstancerSRT, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TrinityLightApplierComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityLightApplierComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityLightApplierComponent");
    set_FilePath(Utils::toGodotString(component->file_path()));
    set_TimeSpan(component->time_span());
    set_CurrentTime(component->current_time());
    set_StartWithOffsetData(component->start_with_offset_data());
    set_OffsetDataName(Utils::toGodotString(component->offset_data_name()));
    set_OffsetBlendWeight(component->offset_blend_weight());
    set_ApplyGameObjectSRT(component->apply_game_object_srt());
    set_Priority(component->priority());
    set_UniqueFlag(component->unique_flag());
    set_UseInstancerSRT(component->use_instancer_srt());
}
