#include "pe_UikitViewComponent.h"

using namespace godot;

void PeUikitViewComponent::_bind_methods()
{
    GETTER_SETTER_BIND(PeUikitViewComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PeUikitViewComponent, BluaPath, Variant::STRING, PROPERTY_HINT_NONE)
}

void PeUikitViewComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetPeUikitViewComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse PeUikitViewComponent");
    set_FilePath(Utils::toGodotString(component->file_path()));
    set_BluaPath(Utils::toGodotString(component->blua_path()));
}
