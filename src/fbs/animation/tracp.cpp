#include "tracp.h"

using namespace godot;

void AnimationChannelParamInt::_bind_methods()
{
    GETTER_SETTER_BIND(AnimationChannelParamInt, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationChannelParamInt, min, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationChannelParamInt, max, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationChannelParamInt, default_val, Variant::INT, PROPERTY_HINT_NONE)
}

void AnimationChannelParamFloat::_bind_methods()
{
    GETTER_SETTER_BIND(AnimationChannelParamFloat, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationChannelParamFloat, min, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationChannelParamFloat, max, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationChannelParamFloat, default_val, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void AnimationChannelParamBool::_bind_methods()
{
    GETTER_SETTER_BIND(AnimationChannelParamBool, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationChannelParamBool, Val, Variant::BOOL, PROPERTY_HINT_NONE)
}

void AnimationChannelParamTrigger::_bind_methods()
{
    GETTER_SETTER_BIND(AnimationChannelParamTrigger, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void AnimationChannelParamEntry::_bind_methods()
{
    GETTER_SETTER_BIND(AnimationChannelParamEntry, animations, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "AnimationChannelParamInt")
    GETTER_SETTER_BIND(AnimationChannelParamEntry, moves, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "AnimationChannelParamFloat")
    GETTER_SETTER_BIND(AnimationChannelParamEntry, switches, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "AnimationChannelParamBool")
    GETTER_SETTER_BIND(AnimationChannelParamEntry, triggers, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "AnimationChannelParamTrigger")
}

void TRAnimationChannelParams::_bind_methods() 
{
    GETTER_SETTER_BIND(TRAnimationChannelParams, table, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "AnimationChannelParamEntry")
}

void TRAnimationChannelParams::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRACP file: %s", file));
    auto tracp = Titan::Animation::GetTRACP(buf.ptr());
    ERR_FAIL_COND_MSG(tracp == nullptr, "Couldn't load TRACP");

    table.instantiate();
    
    auto tbl = tracp->table();

    auto anims = tbl->int_params();
    if(anims)
    {
        Array _arr_anim;
        for (size_t i = 0; i < anims->size(); i++) {
            auto anim = anims->Get(i);

            Ref<AnimationChannelParamInt> obj;
            obj.instantiate();
            obj->set_Name(Utils::toGodotString(anim->name()));
            obj->set_min(anim->min());
            obj->set_max(anim->max());
            obj->set_default_val(anim->default_val());

            _arr_anim.append(obj);
        }
        table->set_animations(_arr_anim);
    }

    auto moves = tbl->float_params();
    if(moves)
    {
        Array _arr_move;
        for (size_t i = 0; i < moves->size(); i++) {
            auto move = moves->Get(i);

            Ref<AnimationChannelParamFloat> obj;
            obj.instantiate();
            obj->set_Name(Utils::toGodotString(move->name()));
            obj->set_min(move->min());
            obj->set_max(move->max());
            obj->set_default_val(move->default_val());

            _arr_move.append(obj);
        }
        table->set_moves(_arr_move);
    }

    auto switches = tbl->bool_params();
    if(switches)
    {
        Array _arr_switch;
        for (size_t i = 0; i < switches->size(); i++) {
            auto swit = switches->Get(i);

            Ref<AnimationChannelParamBool> obj;
            obj.instantiate();
            obj->set_Name(Utils::toGodotString(swit->name()));
            obj->set_Val(swit->val());

            _arr_switch.append(obj);
        }
        table->set_switches(_arr_switch);
    }

    auto trigs = tbl->triggers();
    if(trigs)
    {
        Array _arr_trig;
        for (size_t i = 0; i < trigs->size(); i++) {
            auto trig = trigs->Get(i);

            Ref<AnimationChannelParamTrigger> obj;
            obj.instantiate();
            obj->set_Name(Utils::toGodotString(trig->name()));

            _arr_trig.append(obj);
        }
        table->set_triggers(_arr_trig);
    }
}

Variant ResourceFormatLoaderTRACP::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRAnimationChannelParams> tracn;
    tracn.instantiate();
    tracn->LoadFromFile(p_path);
    return tracn;
}

PackedStringArray ResourceFormatLoaderTRACP::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("tracp");

    return exts;
}

bool ResourceFormatLoaderTRACP::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRAnimationChannelParams");
}