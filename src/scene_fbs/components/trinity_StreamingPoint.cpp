#include "trinity_StreamingPoint.h"

using namespace godot;

void TrinityStreamingObject::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityStreamingObject, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityStreamingObject, NestedType, Variant::PACKED_BYTE_ARRAY, PROPERTY_HINT_NONE)
}

void TrinityStreamingObject::LoadFromTable(const Titan::TrinityScene::TrinityStreamingObject* table)
{
    ERR_FAIL_NULL(table);
    set_Name(Utils::toGodotString(table->name()));
    PackedByteArray nested_type_values;
    if (auto values = table->nested_type()) {
        for (auto value : *values) nested_type_values.push_back(value);
    }
    set_NestedType(nested_type_values);
}

void TrinityStreamingPointData::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityStreamingPointData, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityStreamingPointData, Position, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

void TrinityStreamingPointData::LoadFromTable(const Titan::TrinityScene::TrinityStreamingPointData* table)
{
    ERR_FAIL_NULL(table);
    set_Name(Utils::toGodotString(table->name()));
    set_Position(table->position() ? Utils::toGodotVec3(table->position()) : Vector3());
}

void TrinityStreamingEntry::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityStreamingEntry, Point, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TrinityStreamingPointData")
    GETTER_SETTER_BIND(TrinityStreamingEntry, Objects, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityStreamingObject")
}

void TrinityStreamingEntry::LoadFromTable(const Titan::TrinityScene::TrinityStreamingEntry* table)
{
    ERR_FAIL_NULL(table);
    Ref<TrinityStreamingPointData> point_value;
    if (auto value = table->point()) {
        point_value.instantiate();
        point_value->LoadFromTable(value);
    }
    set_Point(point_value);
    Array objects_values;
    if (auto values = table->objects()) {
        for (auto value : *values) {
            Ref<TrinityStreamingObject> entry;
            entry.instantiate();
            entry->LoadFromTable(value);
            objects_values.push_back(entry);
        }
    }
    set_Objects(objects_values);
}

void TrinityStreamingPoint::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityStreamingPoint, Entries, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityStreamingEntry")
}

void TrinityStreamingPoint::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityStreamingPoint(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityStreamingPoint");
    Array entries_values;
    if (auto values = component->entries()) {
        for (auto value : *values) {
            Ref<TrinityStreamingEntry> entry;
            entry.instantiate();
            entry->LoadFromTable(value);
            entries_values.push_back(entry);
        }
    }
    set_Entries(entries_values);
}
