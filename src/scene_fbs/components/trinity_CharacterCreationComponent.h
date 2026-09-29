#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_CharacterCreationComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityCCDataMasterEntry : public Resource {
    GDCLASS(TrinityCCDataMasterEntry, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::TrinityCCDataMasterEntry* table);
    GETTER_SETTER_DEFINE(String, Part)
    GETTER_SETTER_DEFINE(String, File)
    GETTER_SETTER_DEFINE(String, Name)
private:
    String Part;
    String File;
    String Name;
};

class TrinityCharacterCreationComponent : public Resource {
    GDCLASS(TrinityCharacterCreationComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(uint32_t, unk0)
    GETTER_SETTER_DEFINE(float, unk1)
    GETTER_SETTER_DEFINE(float, unk2)
    GETTER_SETTER_DEFINE(float, unk3)
    GETTER_SETTER_DEFINE(uint32_t, unk4)
    GETTER_SETTER_DEFINE(float, unk5)
    GETTER_SETTER_DEFINE(uint32_t, unk6)
    GETTER_SETTER_DEFINE(float, unk7)
    GETTER_SETTER_DEFINE(uint32_t, unk8)
    GETTER_SETTER_DEFINE(float, unk9)
    GETTER_SETTER_DEFINE(uint32_t, unk10)
    GETTER_SETTER_DEFINE(float, unk11)
    GETTER_SETTER_DEFINE(uint32_t, unk12)
    GETTER_SETTER_DEFINE(uint32_t, unk13)
    GETTER_SETTER_DEFINE(Array, ccdataMasterList)
    GETTER_SETTER_DEFINE(PackedInt64Array, unk14)
private:
    String Name;
    uint32_t unk0 = 0;
    float unk1 = 0.0f;
    float unk2 = 0.0f;
    float unk3 = 0.0f;
    uint32_t unk4 = 0;
    float unk5 = 0.0f;
    uint32_t unk6 = 0;
    float unk7 = 0.0f;
    uint32_t unk8 = 0;
    float unk9 = 0.0f;
    uint32_t unk10 = 0;
    float unk11 = 0.0f;
    uint32_t unk12 = 0;
    uint32_t unk13 = 0;
    Array ccdataMasterList;
    PackedInt64Array unk14;
};

} // namespace godot
