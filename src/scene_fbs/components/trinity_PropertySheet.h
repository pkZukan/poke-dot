#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_PropertySheet_generated.h"
#include "utils.h"

namespace godot {

class TrinityPropertyMetadata : public Resource {
    GDCLASS(TrinityPropertyMetadata, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::TrinityPropertyMetadata* table);
    GETTER_SETTER_DEFINE(uint64_t, unk0)
    GETTER_SETTER_DEFINE(bool, unk1)
private:
    uint64_t unk0 = 0;
    bool unk1 = false;
};

class TrinityProperty : public Resource {
    GDCLASS(TrinityProperty, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::TrinityProperty* table);
    GETTER_SETTER_DEFINE(String, name)
    GETTER_SETTER_DEFINE(bool, value)
    GETTER_SETTER_DEFINE(Ref<TrinityPropertyMetadata>, unk0)
private:
    String name;
    bool value = false;
    Ref<TrinityPropertyMetadata> unk0;
};

class TrinityPropertyGroup : public Resource {
    GDCLASS(TrinityPropertyGroup, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::TrinityPropertyGroup* table);
    GETTER_SETTER_DEFINE(Array, properties)
private:
    Array properties;
};

class TrinityPropertySheet : public Resource {
    GDCLASS(TrinityPropertySheet, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, property_name)
    GETTER_SETTER_DEFINE(String, property_template)
    GETTER_SETTER_DEFINE(Array, unk0)
private:
    String property_name;
    String property_template;
    Array unk0;
};

} // namespace godot
