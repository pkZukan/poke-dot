#include "trinity_PropertySheet.h"

using namespace godot;

void PropInt::_bind_methods()
{
    GETTER_SETTER_BIND(PropInt, data, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PropInt, size, Variant::INT, PROPERTY_HINT_NONE)
}

void PropDec::_bind_methods()
{
    GETTER_SETTER_BIND(PropDec, data, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PropDec, size, Variant::INT, PROPERTY_HINT_NONE)
}

void PropStr::_bind_methods()
{
    GETTER_SETTER_BIND(PropStr, data, Variant::STRING, PROPERTY_HINT_NONE)
}

void PropEnum::_bind_methods()
{
    GETTER_SETTER_BIND(PropEnum, name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PropEnum, val, Variant::INT, PROPERTY_HINT_NONE)
}

void PropUnion::_bind_methods()
{
    GETTER_SETTER_BIND(PropUnion, type, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "PropEnum")
    GETTER_SETTER_BIND(PropUnion, val, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "PropTable")
}

void PropField::_bind_methods()
{
    GETTER_SETTER_BIND(PropField, name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PropField, value_type, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PropField, value, Variant::NIL, PROPERTY_HINT_NONE , "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_NIL_IS_VARIANT)
}

void PropTable::_bind_methods()
{
    GETTER_SETTER_BIND(PropTable, fields, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "PropField")
}

void PropList::_bind_methods()
{
    GETTER_SETTER_BIND(PropList, vals, Variant::ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PropList, vals_type, Variant::PACKED_INT32_ARRAY, PROPERTY_HINT_NONE)
}

void TrinityPropertySheet::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityPropertySheet, property_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityPropertySheet, property_template, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityPropertySheet, unk0, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "PropTable")
}

namespace {
Variant parse_property_value(Titan::TrinityScene::PropVal type, const void* data)
{
    if (!data) return Variant();
    switch (type) {
        case Titan::TrinityScene::PropVal_PropInt: {
            Ref<PropInt> value;
            value.instantiate();
            value->LoadFromTable(static_cast<const Titan::TrinityScene::PropInt*>(data));
            return value;
        }
        case Titan::TrinityScene::PropVal_PropDec: {
            Ref<PropDec> value;
            value.instantiate();
            value->LoadFromTable(static_cast<const Titan::TrinityScene::PropDec*>(data));
            return value;
        }
        case Titan::TrinityScene::PropVal_PropStr: {
            Ref<PropStr> value;
            value.instantiate();
            value->LoadFromTable(static_cast<const Titan::TrinityScene::PropStr*>(data));
            return value;
        }
        case Titan::TrinityScene::PropVal_PropEnum: {
            Ref<PropEnum> value;
            value.instantiate();
            value->LoadFromTable(static_cast<const Titan::TrinityScene::PropEnum*>(data));
            return value;
        }
        case Titan::TrinityScene::PropVal_PropUnion: {
            Ref<PropUnion> value;
            value.instantiate();
            value->LoadFromTable(static_cast<const Titan::TrinityScene::PropUnion*>(data));
            return value;
        }
        case Titan::TrinityScene::PropVal_PropTable: {
            Ref<PropTable> value;
            value.instantiate();
            value->LoadFromTable(static_cast<const Titan::TrinityScene::PropTable*>(data));
            return value;
        }
        case Titan::TrinityScene::PropVal_PropList: {
            Ref<PropList> value;
            value.instantiate();
            value->LoadFromTable(static_cast<const Titan::TrinityScene::PropList*>(data));
            return value;
        }
        case Titan::TrinityScene::PropVal_Titan_Math_Vec2: {
            auto v = static_cast<const Titan::Math::Vec2*>(data);
            return Vector2(v->u(), v->v());
        }
        case Titan::TrinityScene::PropVal_Titan_Math_Vec3: {
            auto v = static_cast<const Titan::Math::Vec3*>(data);
            return Vector3(v->x(), v->y(), v->z());
        }
        case Titan::TrinityScene::PropVal_Titan_Math_Vec4: {
            auto v = static_cast<const Titan::Math::Vec4*>(data);
            return Vector4(v->x(), v->y(), v->z(), v->w());
        }
        default: return Variant();
    }
}
}

void PropInt::LoadFromTable(const Titan::TrinityScene::PropInt* table)
{
    ERR_FAIL_NULL(table);
    set_data(table->data());
    set_size(table->size());
}

void PropDec::LoadFromTable(const Titan::TrinityScene::PropDec* table)
{
    ERR_FAIL_NULL(table);
    set_data(table->data());
    set_size(table->size());
}

void PropStr::LoadFromTable(const Titan::TrinityScene::PropStr* table)
{
    ERR_FAIL_NULL(table);
    set_data(Utils::toGodotString(table->data()));
}

void PropEnum::LoadFromTable(const Titan::TrinityScene::PropEnum* table)
{
    ERR_FAIL_NULL(table);
    set_name(Utils::toGodotString(table->name()));
    set_val(table->val());
}

void PropUnion::LoadFromTable(const Titan::TrinityScene::PropUnion* table)
{
    ERR_FAIL_NULL(table);
    Ref<PropEnum> value_type;
    if (auto item = table->type()) {
        value_type.instantiate();
        value_type->LoadFromTable(item);
    }
    set_type(value_type);
    Ref<PropTable> value_val;
    if (auto item = table->val()) {
        value_val.instantiate();
        value_val->LoadFromTable(item);
    }
    set_val(value_val);
}

void PropField::LoadFromTable(const Titan::TrinityScene::PropField* table)
{
    ERR_FAIL_NULL(table);
    set_name(Utils::toGodotString(table->name()));
    set_value_type(table->value_type());
    set_value(parse_property_value(table->value_type(), table->value()));
}

void PropTable::LoadFromTable(const Titan::TrinityScene::PropTable* table)
{
    ERR_FAIL_NULL(table);
    Array values;
    if (auto entries = table->fields()) {
        for (auto item : *entries) {
            Ref<PropField> value;
            value.instantiate();
            value->LoadFromTable(item);
            values.push_back(value);
        }
    }
    set_fields(values);
}

void PropList::LoadFromTable(const Titan::TrinityScene::PropList* table)
{
    ERR_FAIL_NULL(table);
    Array values;
    PackedInt32Array types;
    auto tags = table->vals_type();
    auto entries = table->vals();
    if (tags && entries) {
        ERR_FAIL_COND(tags->size() != entries->size());
        for (flatbuffers::uoffset_t i = 0; i < entries->size(); i++) {
            types.push_back(tags->Get(i));
            values.push_back(parse_property_value(static_cast<Titan::TrinityScene::PropVal>(tags->Get(i)), entries->Get(i)));
        }
    }
    set_vals(values);
    set_vals_type(types);
}

void TrinityPropertySheet::LoadFromBuffer(const void* buffer)
{
    ERR_FAIL_NULL(buffer);
    auto table = Titan::TrinityScene::GetTrinityPropertySheet(buffer);
    set_property_name(Utils::toGodotString(table->property_name()));
    set_property_template(Utils::toGodotString(table->property_template()));
    Array values;
    if (auto entries = table->unk0()) {
        for (auto item : *entries) {
            Ref<PropTable> value;
            value.instantiate();
            value->LoadFromTable(item);
            values.push_back(value);
        }
    }
    set_unk0(values);
}
