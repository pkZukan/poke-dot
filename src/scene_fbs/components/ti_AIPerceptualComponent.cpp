#include "ti_AIPerceptualComponent.h"

using namespace godot;

void TiAIPerceptualComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TiAIPerceptualComponent, value, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TiAIPerceptualComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTiAIPerceptualComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TiAIPerceptualComponent");
    set_value(component->value());
}
