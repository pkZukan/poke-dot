#include "trinity_ObjectTemplate.h"

using namespace godot;

void TrinityObjectTemplate::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityObjectTemplate, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, Scope, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, FilePath, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, IsExpanded, Variant::BOOL, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, EntityType, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityObjectTemplate, EntityData, Variant::PACKED_BYTE_ARRAY, PROPERTY_HINT_NONE)
}

void TrinityObjectTemplate::LoadFromBuffer(const void* buffer)
{
    auto object_template = Titan::TrinityScene::GetObjectTemplate(buffer);
    set_Name(Utils::toGodotString(object_template->name()));
    set_Scope(Utils::toGodotString(object_template->scope()));
    set_FilePath(Utils::toGodotString(object_template->file_path()));
    set_IsExpanded(object_template->is_expanded());
    set_EntityType(Utils::toGodotString(object_template->entity_type()));

    auto entity_data_vec = object_template->entity_data();
    PackedByteArray entity_data_packed_byte_array;
    if (entity_data_vec)
    {
        entity_data_packed_byte_array.resize(entity_data_vec->size());
        memcpy(entity_data_packed_byte_array.ptrw(), entity_data_vec->data(), entity_data_vec->size());
    }
    set_EntityData(entity_data_packed_byte_array);
}