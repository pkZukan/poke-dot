#include "pe_InputEventTriggerComponent.h"

using namespace godot;

void PeInputEventTriggerComponent::_bind_methods()
{
    GETTER_SETTER_BIND(PeInputEventTriggerComponent, InputName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PeInputEventTriggerComponent, ResourceName, Variant::STRING, PROPERTY_HINT_NONE)
}

void PeInputEventTriggerComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetPeInputEventTriggerComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse PeInputEventTriggerComponent");
    set_InputName(Utils::toGodotString(component->input_name()));
    set_ResourceName(Utils::toGodotString(component->resource_name()));
}
