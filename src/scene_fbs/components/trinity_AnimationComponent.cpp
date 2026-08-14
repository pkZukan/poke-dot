#include "trinity_AnimationComponent.h"

using namespace godot;

void TrinityAnimationComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityAnimationComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityAnimationComponent, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityAnimationComponent::LoadFromBuffer(const void* buffer)
{
    auto animation_component = Titan::TrinityScene::GetTrinityAnimationComponent(buffer);
    ERR_FAIL_COND_MSG(animation_component == nullptr, "Couldn't parse animation component");

    set_FilePath(Utils::toGodotString(animation_component->file_path()));
    set_Name(Utils::toGodotString(animation_component->name()));
}