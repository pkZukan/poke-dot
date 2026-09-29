#include "pe_TextComponent.h"

using namespace godot;

void PeTextComponent::_bind_methods()
{
    GETTER_SETTER_BIND(PeTextComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
}

void PeTextComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetPeTextComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse PeTextComponent");
    set_FilePath(Utils::toGodotString(component->file_path()));
}
