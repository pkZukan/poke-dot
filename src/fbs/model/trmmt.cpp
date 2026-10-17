#include "trmmt.h"

using namespace godot;

void VariationControl::_bind_methods()
{
    GETTER_SETTER_BIND(VariationControl, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, FrameMin, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, FrameMax, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, DefaultOverride, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, Tracm, Variant::PACKED_BYTE_ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, Rule, Variant::DICTIONARY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, TableDriven, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, VariationCount, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, MaterialValues, Variant::DICTIONARY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(VariationControl, MeshVisibility, Variant::DICTIONARY, PROPERTY_HINT_NONE)
}

void ModelVariation::_bind_methods()
{
    GETTER_SETTER_BIND(ModelVariation, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(ModelVariation, Materials, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(ModelVariation, MeshVisibility, Variant::DICTIONARY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(ModelVariation, Controls, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "VariationControl")
}

void TRModelMeta::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("find_variation", "name"), &TRModelMeta::find_variation);
    GETTER_SETTER_BIND(TRModelMeta, Variations, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "ModelVariation")
}

// Collects named per-variation series into name -> Array.
template <typename T, typename F>
static void read_series(const flatbuffers::Vector<flatbuffers::Offset<T>> *list, Dictionary &out, F convert)
{
    if (list == nullptr) return;
    for (auto series : *list) {
        Array values;
        if (series->values())
            for (auto v : *series->values()) values.push_back(convert(v));
        out[Utils::toGodotString(series->name())] = values;
    }
}

static Dictionary read_rule(const Titan::Model::ControlRule *rule)
{
    Dictionary out;
    if (rule == nullptr) return out;
    out["name"] = Utils::toGodotString(rule->name());
    out["upper"] = rule->upper();
    out["initial"] = rule->initial();

    Dictionary cond;
    if (auto copy = rule->condition_as_RuleCopy()) {
        cond["type"] = "copy";
        cond["source"] = Utils::toGodotString(copy->source());
    } else if (auto copy_if = rule->condition_as_RuleCopyIfPresent()) {
        cond["type"] = "copy_if_present";
        cond["probe"] = Utils::toGodotString(copy_if->probe());
        cond["source"] = Utils::toGodotString(copy_if->source());
    } else if (auto sw = rule->condition_as_RuleSwitch()) {
        cond["type"] = "switch";
        cond["source"] = Utils::toGodotString(sw->source());
        Dictionary cases;
        if (sw->cases())
            for (auto c : *sw->cases()) cases[c->match()] = c->result();
        cond["cases"] = cases;
    }
    if (!cond.is_empty()) out["condition"] = cond;
    return out;
}

static Ref<VariationControl> read_control(const Titan::Model::VariationControl *src)
{
    Ref<VariationControl> ctrl;
    ctrl.instantiate();
    ctrl->set_Name(Utils::toGodotString(src->name()));
    ctrl->set_FrameMin(src->frame_min());
    ctrl->set_FrameMax(src->frame_max());
    ctrl->set_DefaultOverride(src->default_override());
    ctrl->set_Rule(read_rule(src->rule()));

    if (src->tracm() && src->tracm()->bytebuffer()) {
        auto bytes = src->tracm()->bytebuffer();
        PackedByteArray tracm;
        tracm.resize(bytes->size());
        memcpy(tracm.ptrw(), bytes->data(), bytes->size());
        ctrl->set_Tracm(tracm);
    }

    auto table = src->variations();
    ctrl->set_TableDriven(src->table_driven() && table != nullptr);
    if (table == nullptr) return ctrl;

    ctrl->set_VariationCount(table->count());
    Dictionary materials;
    if (table->materials()) {
        for (auto mat : *table->materials()) {
            Dictionary params;
            read_series(mat->floats(), params, [](float v) { return Variant(v); });
            read_series(mat->vec3s(), params, [](const Titan::Math::Vec3 *v) { return Variant(Utils::toGodotVec3(v)); });
            read_series(mat->vec4s(), params, [](const Titan::Math::Vec4 *v) { return Variant(Utils::toGodotVec4(v)); });
            read_series(mat->ints(), params, [](int32_t v) { return Variant(v); });
            materials[Utils::toGodotString(mat->material())] = params;
        }
    }
    ctrl->set_MaterialValues(materials);

    Dictionary visibility;
    read_series(table->mesh_visibility(), visibility, [](uint8_t v) { return Variant(v != 0); });
    ctrl->set_MeshVisibility(visibility);
    return ctrl;
}

void TRModelMeta::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRMMT file: %s", file));

    auto meta = Titan::Model::GetTRMMT(buf.ptr());
    ERR_FAIL_COND_MSG(meta == NULL, vformat("Couldn't load TRMMT flatbuffer"));
    if (meta->variations() == nullptr) return;

    for (auto src : *meta->variations())
    {
        Ref<ModelVariation> variation;
        variation.instantiate();
        variation->set_Name(Utils::toGodotString(src->name()));

        PackedStringArray materials;
        if (src->materials())
            for (auto path : *src->materials()) materials.push_back(Utils::toGodotString(path));
        variation->set_Materials(materials);

        Dictionary visibility;
        if (src->mesh_visibility())
            for (auto toggle : *src->mesh_visibility())
                visibility[Utils::toGodotString(toggle->mesh())] = toggle->visible();
        variation->set_MeshVisibility(visibility);

        Array controls;
        if (src->controls())
            for (auto ctrl : *src->controls()) controls.push_back(read_control(ctrl));
        variation->set_Controls(controls);

        Variations.push_back(variation);
    }
}

Ref<ModelVariation> TRModelMeta::find_variation(const String &name) const
{
    for (int i = 0; i < Variations.size(); i++) {
        Ref<ModelVariation> variation = Variations[i];
        if (variation.is_valid() && variation->get_Name() == name) return variation;
    }
    return Ref<ModelVariation>();
}

Variant ResourceFormatLoaderTRMMT::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRModelMeta> trmmt;
    trmmt.instantiate();
    trmmt->LoadFromFile(p_path);
    return trmmt;
}

PackedStringArray ResourceFormatLoaderTRMMT::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trmmt");

    return exts;
}

bool ResourceFormatLoaderTRMMT::_handles_type(const StringName &p_type) const
{
    return p_type == String("TRModelMeta");
}
