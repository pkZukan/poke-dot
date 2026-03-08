#include "tracr.h"

using namespace godot;

void TRAnimationTrackResource::_bind_methods()
{
    GETTER_SETTER_BIND(TRAnimationTrackResource, FileName, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRAnimationTrackResourceTable::_bind_methods()
{
    GETTER_SETTER_BIND(TRAnimationTrackResourceTable, animation, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationTrackResource")
    GETTER_SETTER_BIND(TRAnimationTrackResourceTable, material, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationTrackResource")
    GETTER_SETTER_BIND(TRAnimationTrackResourceTable, effect, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationTrackResource")
}

void TRAnimationTrack::_bind_methods()
{
    GETTER_SETTER_BIND(TRAnimationTrack, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationTrack, res_0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationTrack, res_1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationTrack, Resources, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationTrackResourceTable")
}

void TRAnimationTrackListTable::_bind_methods()
{
    GETTER_SETTER_BIND(TRAnimationTrackListTable, list, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRAnimationTrack")
}

void TRAnimationChannelResource::_bind_methods() 
{
    GETTER_SETTER_BIND(TRAnimationChannelResource, table, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationTrackListTable")
}

void TRAnimationChannelResource::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRACR file: %s", file));
    auto tracr = Titan::Animation::GetTRACR(buf.ptr());
    ERR_FAIL_COND_MSG(tracr == nullptr, "Couldn't load TRACR");

    auto track_table = tracr->track_list();

    Ref<TRAnimationTrackListTable> t_table;
    t_table.instantiate();
    Array trackList;
    auto list = track_table->list();
    for (size_t i = 0; i < list->size(); i++) {
        auto ent = list->Get(i);

        Ref<TRAnimationTrack> track;
        track.instantiate();
        track->set_Name(Utils::toGodotString(ent->track_name()));
        track->set_res_0(Utils::toGodotString(ent->res_0()));
        track->set_res_1(ent->res_1());

        auto res = ent->track_resource();
        Ref<TRAnimationTrackResourceTable> trackRes;
        trackRes.instantiate();

        if(res->animation())
        {
            Ref<TRAnimationTrackResource> anim;
            anim.instantiate();
            anim->set_FileName(Utils::toGodotString(res->animation()->filename()));
            trackRes->set_animation(anim);
        }

        if(res->material())
        {
            Ref<TRAnimationTrackResource> mat;
            mat.instantiate();
            mat->set_FileName(Utils::toGodotString(res->material()->filename()));
            trackRes->set_material(mat);
        }

        if(res->effect())
        {
            Ref<TRAnimationTrackResource> eff;
            eff.instantiate();
            eff->set_FileName(Utils::toGodotString(res->effect()->filename()));
            trackRes->set_effect(eff);
        }

        track->set_Resources(trackRes);
        trackList.append(track);
    }
    t_table->set_list(trackList);
    set_table(t_table);
}

Variant ResourceFormatLoaderTRACR::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRAnimationChannelResource> tracr;
    tracr.instantiate();
    tracr->LoadFromFile(p_path);
    return tracr;
}

PackedStringArray ResourceFormatLoaderTRACR::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("tracr");

    return exts;
}

bool ResourceFormatLoaderTRACR::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRAnimationChannelResource");
}