#include "trinity_ScriptComponent.h"

using namespace godot;

void TrinityScriptComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityScriptComponent, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityScriptComponent, PackageName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityScriptComponent, IsParallelized, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityScriptComponent, Priority, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityScriptComponent, IsStatic, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityScriptComponent, ResName, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityScriptComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityScriptComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityScriptComponent");
    set_FilePath(Utils::toGodotString(component->file_path()));
    set_PackageName(Utils::toGodotString(component->package_name()));
    set_IsParallelized(component->is_parallelized());
    set_Priority(component->priority());
    set_IsStatic(component->is_static());
    set_ResName(Utils::toGodotString(component->res_name()));
}
