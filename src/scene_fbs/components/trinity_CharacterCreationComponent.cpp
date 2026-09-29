#include "trinity_CharacterCreationComponent.h"

using namespace godot;

void TrinityCCDataMasterEntry::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityCCDataMasterEntry, Part, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCCDataMasterEntry, File, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCCDataMasterEntry, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityCCDataMasterEntry::LoadFromTable(const Titan::TrinityScene::TrinityCCDataMasterEntry* table)
{
    ERR_FAIL_NULL(table);
    set_Part(Utils::toGodotString(table->part()));
    set_File(Utils::toGodotString(table->file()));
    set_Name(Utils::toGodotString(table->name()));
}

void TrinityCharacterCreationComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk1, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk2, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk3, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk4, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk5, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk6, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk7, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk8, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk9, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk10, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk11, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk12, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk13, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, ccdataMasterList, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityCCDataMasterEntry")
    GETTER_SETTER_BIND(TrinityCharacterCreationComponent, unk14, Variant::PACKED_INT64_ARRAY, PROPERTY_HINT_NONE)
}

void TrinityCharacterCreationComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityCharacterCreationComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityCharacterCreationComponent");
    set_Name(Utils::toGodotString(component->name()));
    set_unk0(component->unk0());
    set_unk1(component->unk1());
    set_unk2(component->unk2());
    set_unk3(component->unk3());
    set_unk4(component->unk4());
    set_unk5(component->unk5());
    set_unk6(component->unk6());
    set_unk7(component->unk7());
    set_unk8(component->unk8());
    set_unk9(component->unk9());
    set_unk10(component->unk10());
    set_unk11(component->unk11());
    set_unk12(component->unk12());
    set_unk13(component->unk13());
    Array ccdata_master_list_values;
    if (auto values = component->ccdata_master_list()) {
        for (auto value : *values) {
            Ref<TrinityCCDataMasterEntry> entry;
            entry.instantiate();
            entry->LoadFromTable(value);
            ccdata_master_list_values.push_back(entry);
        }
    }
    set_ccdataMasterList(ccdata_master_list_values);
    PackedInt64Array unk14_values;
    if (auto values = component->unk14()) {
        for (auto value : *values) unk14_values.push_back(value);
    }
    set_unk14(unk14_values);
}
