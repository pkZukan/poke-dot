#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/pe_FlatbuffersDataComponent_generated.h"
#include "utils.h"

namespace godot {

class PeFlatbuffersDataEntry : public Resource {
    GDCLASS(PeFlatbuffersDataEntry, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PeFlatbuffersDataEntry* table);
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(float, FilePath)
private:
    String Name;
    float FilePath = 0.0f;
};

class PeFlatbuffersDataComponent : public Resource {
    GDCLASS(PeFlatbuffersDataComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, BfbsFilePath)
    GETTER_SETTER_DEFINE(Array, Data)
private:
    String BfbsFilePath;
    Array Data;
};

} // namespace godot
