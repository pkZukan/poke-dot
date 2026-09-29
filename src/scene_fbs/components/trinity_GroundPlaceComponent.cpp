#include "trinity_GroundPlaceComponent.h"

using namespace godot;

void TrinityGroundPlaceComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityGroundPlaceComponent, index, Variant::INT, PROPERTY_HINT_NONE)
}

void TrinityGroundPlaceComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityGroundPlaceComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityGroundPlaceComponent");
    set_index(component->index());
}
