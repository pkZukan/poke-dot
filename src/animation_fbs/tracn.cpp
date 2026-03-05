#include "tracn.h"

using namespace godot;

void AnimationEntry::_bind_methods()
{
    GETTER_SETTER_BIND(AnimationEntry, animation_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationEntry, filename, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRAnimationChannelNames::_bind_methods() 
{
    GETTER_SETTER_BIND(TRAnimationChannelNames, list, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "AnimationEntry")
}

void TRAnimationChannelNames::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRACN file: %s", file));
    auto tracn = Titan::Animation::GetTRACN(buf.ptr());
    ERR_FAIL_COND_MSG(tracn == nullptr, "Couldn't load TRACN");

    auto list = tracn->list();
    Array channelArr;
    if (list) {
        for (size_t i = 0; i < list->size(); i++) {
            auto ent = list->Get(i);

            Ref<AnimationEntry> animEnt;
            animEnt.instantiate();
            animEnt->set_animation_name(Utils::toGodotString(ent->animation_name()));
            animEnt->set_filename(Utils::toGodotString(ent->filename()));

            channelArr.append(animEnt);
        }
    }
    set_list(channelArr);
}

Variant ResourceFormatLoaderTRACN::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRAnimationChannelNames> tracn;
    tracn.instantiate();
    tracn->LoadFromFile(p_path);
    return tracn;
}

PackedStringArray ResourceFormatLoaderTRACN::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("tracn");

    return exts;
}

bool ResourceFormatLoaderTRACN::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRAnimationChannelNames");
}