#include "pe_FlatbuffersDataComponent.h"

using namespace godot;

void PeFlatbuffersDataEntry::_bind_methods()
{
    GETTER_SETTER_BIND(PeFlatbuffersDataEntry, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PeFlatbuffersDataEntry, FilePath, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void PeFlatbuffersDataEntry::LoadFromTable(const Titan::TrinityScene::PeFlatbuffersDataEntry* table)
{
    ERR_FAIL_NULL(table);
    set_Name(Utils::toGodotString(table->name()));
    set_FilePath(table->file_path());
}

void PeFlatbuffersDataComponent::_bind_methods()
{
    GETTER_SETTER_BIND(PeFlatbuffersDataComponent, BfbsFilePath, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PeFlatbuffersDataComponent, Data, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "PeFlatbuffersDataEntry")
}

void PeFlatbuffersDataComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetPeFlatbuffersDataComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse PeFlatbuffersDataComponent");
    set_BfbsFilePath(Utils::toGodotString(component->bfbs_file_path()));
    Array data_values;
    if (auto values = component->data()) {
        for (auto value : *values) {
            Ref<PeFlatbuffersDataEntry> entry;
            entry.instantiate();
            entry->LoadFromTable(value);
            data_values.push_back(entry);
        }
    }
    set_Data(data_values);
}
