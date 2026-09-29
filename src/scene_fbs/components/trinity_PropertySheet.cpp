#include "trinity_PropertySheet.h"

using namespace godot;

void TrinityPropertyMetadata::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityPropertyMetadata, unk0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityPropertyMetadata, unk1, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TrinityPropertyMetadata::LoadFromTable(const Titan::TrinityScene::TrinityPropertyMetadata* table)
{
    ERR_FAIL_NULL(table);
    set_unk0(table->unk0());
    set_unk1(table->unk1());
}

void TrinityProperty::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityProperty, name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityProperty, value, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityProperty, unk0, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TrinityPropertyMetadata")
}

void TrinityProperty::LoadFromTable(const Titan::TrinityScene::TrinityProperty* table)
{
    ERR_FAIL_NULL(table);
    set_name(Utils::toGodotString(table->name()));
    set_value(table->value());
    Ref<TrinityPropertyMetadata> unk0_value;
    if (auto value = table->unk0()) {
        unk0_value.instantiate();
        unk0_value->LoadFromTable(value);
    }
    set_unk0(unk0_value);
}

void TrinityPropertyGroup::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityPropertyGroup, properties, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityProperty")
}

void TrinityPropertyGroup::LoadFromTable(const Titan::TrinityScene::TrinityPropertyGroup* table)
{
    ERR_FAIL_NULL(table);
    Array properties_values;
    if (auto values = table->properties()) {
        for (auto value : *values) {
            Ref<TrinityProperty> entry;
            entry.instantiate();
            entry->LoadFromTable(value);
            properties_values.push_back(entry);
        }
    }
    set_properties(properties_values);
}

void TrinityPropertySheet::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityPropertySheet, property_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityPropertySheet, property_template, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityPropertySheet, unk0, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityPropertyGroup")
}

void TrinityPropertySheet::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityPropertySheet(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityPropertySheet");
    set_property_name(Utils::toGodotString(component->property_name()));
    set_property_template(Utils::toGodotString(component->property_template()));
    Array unk0_values;
    if (auto values = component->unk0()) {
        for (auto value : *values) {
            Ref<TrinityPropertyGroup> entry;
            entry.instantiate();
            entry->LoadFromTable(value);
            unk0_values.push_back(entry);
        }
    }
    set_unk0(unk0_values);
}
