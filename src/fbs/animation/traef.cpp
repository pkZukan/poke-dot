#include "traef.h"

#define EFFECT_VALUE "TREffectValueInt,TREffectValueFloat,TREffectValueByte,TREffectValueString"

using namespace godot;

void TREffectValueInt::_bind_methods()
{
    GETTER_SETTER_BIND(TREffectValueInt, value, Variant::INT, PROPERTY_HINT_NONE)
}

void TREffectValueFloat::_bind_methods()
{
    GETTER_SETTER_BIND(TREffectValueFloat, value, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void TREffectValueByte::_bind_methods()
{
    GETTER_SETTER_BIND(TREffectValueByte, value, Variant::INT, PROPERTY_HINT_NONE)
}

void TREffectValueString::_bind_methods()
{
    GETTER_SETTER_BIND(TREffectValueString, value, Variant::STRING, PROPERTY_HINT_NONE)
}

void TREffect::_bind_methods()
{
    GETTER_SETTER_BIND(TREffect, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREffect, value, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, EFFECT_VALUE)
}

void TRAnimationEffect::_bind_methods()
{
    GETTER_SETTER_BIND(TRAnimationEffect, res_0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationEffect, path, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationEffect, start, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationEffect, duration, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationEffect, effects, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TREffect")
}

void TRAnimationEffects::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_events", "frame_rate"), &TRAnimationEffects::get_events, DEFVAL(0.0));
    GETTER_SETTER_BIND(TRAnimationEffects, effects, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRAnimationEffect")
}

Array TRAnimationEffects::get_events(double frame_rate) const
{
    Array events;
    events.resize(effects.size());
    for (int i = 0; i < effects.size(); ++i)
    {
        Ref<TRAnimationEffect> effect = effects[i];
        Dictionary event;
        event["path"] = effect->get_path();
        event["res_0"] = effect->get_res_0();
        event["start_frame"] = effect->get_start();
        // The schema calls this duration, but asset ranges indicate an end frame.
        event["end_frame"] = effect->get_duration();
        if (frame_rate > 0.0)
        {
            event["start_time"] = effect->get_start() / frame_rate;
            event["end_time"] = effect->get_duration() / frame_rate;
        }

        Dictionary parameters;
        Array entries = effect->get_effects();
        for (int j = 0; j < entries.size(); ++j)
        {
            Ref<TREffect> entry = entries[j];
            Ref<Resource> value = entry->get_value();
            if (Ref<TREffectValueInt> integer = value; integer.is_valid())
                parameters[entry->get_Name()] = integer->get_value();
            else if (Ref<TREffectValueFloat> real = value; real.is_valid())
                parameters[entry->get_Name()] = real->get_value();
            else if (Ref<TREffectValueByte> byte = value; byte.is_valid())
                parameters[entry->get_Name()] = byte->get_value();
            else if (Ref<TREffectValueString> string = value; string.is_valid())
                parameters[entry->get_Name()] = string->get_value();
            else
                parameters[entry->get_Name()] = Variant();
        }
        event["parameters"] = parameters;
        events[i] = event;
    }
    return events;
}

Ref<Resource> TRAnimationEffects::_LoadEffectValue(Titan::Animation::EffectValue type, const void* data)
{
    if(!data)
        return Ref<Resource>();

    switch (type) {
        case Titan::Animation::EffectValue::EffectValue_EffectValue_Int:
        {
            auto src = static_cast<const Titan::Animation::EffectValue_Int*>(data);
            Ref<TREffectValueInt> value;
            value.instantiate();
            value->set_value(src->value());
            return value;
        }
        case Titan::Animation::EffectValue::EffectValue_EffectValue_Float:
        {
            auto src = static_cast<const Titan::Animation::EffectValue_Float*>(data);
            Ref<TREffectValueFloat> value;
            value.instantiate();
            value->set_value(src->value());
            return value;
        }
        case Titan::Animation::EffectValue::EffectValue_EffectValue_Byte:
        {
            auto src = static_cast<const Titan::Animation::EffectValue_Byte*>(data);
            Ref<TREffectValueByte> value;
            value.instantiate();
            value->set_value(src->value());
            return value;
        }
        case Titan::Animation::EffectValue::EffectValue_EffectValue_String:
        {
            auto src = static_cast<const Titan::Animation::EffectValue_String*>(data);
            Ref<TREffectValueString> value;
            value.instantiate();
            value->set_value(Utils::toGodotString(src->value()));
            return value;
        }
        default:
            return Ref<Resource>();
    }
}

void TRAnimationEffects::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRAEF file: %s", file));
    flatbuffers::Verifier verifier(buf.ptr(), buf.size());
    ERR_FAIL_COND_MSG(!Titan::Animation::VerifyTRAEFBuffer(verifier), "Couldn't load TRAEF");
    auto traef = Titan::Animation::GetTRAEF(buf.ptr());

    auto anims = traef->effects();
    Array animArr;
    if(anims)
    {
        for (size_t i = 0; i < anims->size(); i++) {
            auto anim = anims->Get(i);

            Ref<TRAnimationEffect> obj;
            obj.instantiate();
            obj->set_res_0(anim->res_0());
            obj->set_path(Utils::toGodotString(anim->path()));
            obj->set_start(anim->start());
            obj->set_duration(anim->duration());

            auto params = anim->effects();
            Array paramArr;
            if(params)
            {
                for (size_t j = 0; j < params->size(); j++) {
                    auto param = params->Get(j);

                    Ref<TREffect> effect;
                    effect.instantiate();
                    effect->set_Name(Utils::toGodotString(param->name()));
                    effect->set_value(_LoadEffectValue(param->value_type(), param->value()));

                    paramArr.append(effect);
                }
            }
            obj->set_effects(paramArr);

            animArr.append(obj);
        }
    }
    set_effects(animArr);
}

Variant ResourceFormatLoaderTRAEF::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRAnimationEffects> traef;
    traef.instantiate();
    traef->LoadFromFile(p_path);
    return traef;
}

PackedStringArray ResourceFormatLoaderTRAEF::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("traef");

    return exts;
}

bool ResourceFormatLoaderTRAEF::_handles_type(const StringName &p_type) const
{
    return p_type == String("TRAnimationEffects");
}
