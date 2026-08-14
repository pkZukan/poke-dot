#include "trinity_ObjectTemplate.h"

using namespace godot;

void TrinityObjectTemplate::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityObjectTemplate, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, Scope, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, IsExpanded, Variant::BOOL, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, EntityType, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, EntityData, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "Resource")
}

void TrinityObjectTemplate::LoadFromBuffer(const void* buffer)
{
    auto object_template = Titan::TrinityScene::GetObjectTemplate(buffer);
    set_Name(Utils::toGodotString(object_template->name()));
    set_Scope(Utils::toGodotString(object_template->scope()));
    set_FilePath(Utils::toGodotString(object_template->file_path()));
    set_IsExpanded(object_template->is_expanded());

    String entityType = Utils::toGodotString(object_template->entity_type());
    set_EntityType(entityType);

    auto entity_data_vec = object_template->entity_data();
    Ref<Resource> entityData = TrinitySceneParser::FromData(entityType, entity_data_vec->data());
    set_EntityData(entityData);
}