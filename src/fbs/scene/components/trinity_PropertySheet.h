#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_PropertySheet_generated.h"
#include "utils.h"

namespace godot {

class PropTable;

class PropInt : public Resource {
    GDCLASS(PropInt, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropInt* table);
    GETTER_SETTER_DEFINE(uint64_t, data)
    GETTER_SETTER_DEFINE(int, size)
private:
    uint64_t data = 0;
    int size = 0;
};

class PropDec : public Resource {
    GDCLASS(PropDec, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropDec* table);
    GETTER_SETTER_DEFINE(double, data)
    GETTER_SETTER_DEFINE(int, size)
private:
    double data = 0.0;
    int size = 0;
};

class PropStr : public Resource {
    GDCLASS(PropStr, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropStr* table);
    GETTER_SETTER_DEFINE(String, data)
private:
    String data;
};

class PropEnum : public Resource {
    GDCLASS(PropEnum, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropEnum* table);
    GETTER_SETTER_DEFINE(String, name)
    int get_val() { return val; }
    void set_val(int value) { val = value; }
private:
    String name;
    int val = 0;
};

class PropUnion : public Resource {
    GDCLASS(PropUnion, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropUnion* table);
    GETTER_SETTER_DEFINE(Ref<PropEnum>, type)
    Ref<PropTable> get_val() { return val; }
    void set_val(Ref<PropTable> value) { val = value; }
private:
    Ref<PropEnum> type;
    Ref<PropTable> val;
};

class PropField : public Resource {
    GDCLASS(PropField, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropField* table);
    GETTER_SETTER_DEFINE(String, name)
    GETTER_SETTER_DEFINE(int, value_type)
    GETTER_SETTER_DEFINE(Variant, value)
private:
    String name;
    int value_type = 0;
    Variant value;
};

class PropTable : public Resource {
    GDCLASS(PropTable, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropTable* table);
    GETTER_SETTER_DEFINE(Array, fields)
private:
    Array fields;
};

class PropList : public Resource {
    GDCLASS(PropList, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::PropList* table);
    GETTER_SETTER_DEFINE(Array, vals)
    GETTER_SETTER_DEFINE(PackedInt32Array, vals_type)
private:
    Array vals;
    PackedInt32Array vals_type;
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
