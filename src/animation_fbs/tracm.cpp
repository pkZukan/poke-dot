#include "tracm.h"

using namespace godot;

void Framed8BoolTrack::_bind_methods()
{
    //
}

void Framed16BoolTrack::_bind_methods()
{
    //
}

void DynamicBoolTrack::_bind_methods()
{
    //
}

void FixedBoolTrack::_bind_methods()
{
    //
}

void TRTrackFlagsInfo::_bind_methods()
{
    //
}

void TRBlendShapeTimeline::_bind_methods()
{
    //
}

void TRVisibilityShapeTimeline::_bind_methods()
{
    //
}

void TRTrackMaterialTimeline::_bind_methods()
{
    //
}

void TRMaterialAnimTrack::_bind_methods()
{
    //
}

void TRAnimationChannelMaterials::_bind_methods() 
{
    GETTER_SETTER_BIND(TRAnimationChannelMaterials, info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationInfo")
    GETTER_SETTER_BIND(TRAnimationChannelMaterials, tracks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRMaterialAnimTrack")
}

void TRAnimationChannelMaterials::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRACN file: %s", file));
    auto tracm = Titan::Animation::GetTRACM(buf.ptr());
    ERR_FAIL_COND_MSG(tracm == nullptr, "Couldn't load TRACM");

    Ref<TRAnimationInfo> _info;
    _info->set_does_loop(tracm->info()->does_loop());
    _info->set_animation_count(tracm->info()->animation_count());
    _info->set_animation_rate(tracm->info()->animation_rate());
    _info.instantiate();

    set_info(_info);

    auto tracks = tracm->tracks();
    Array trackArr;
    for(int i = 0; i < tracks->size(); i++)
    {
        auto track = tracks->Get(i);

        Ref<TRMaterialAnimTrack> anim;
        anim.instantiate();
        anim->set_path(Utils::toGodotString(track->track_path()));
        anim->set_res_1(track->res_1());
        anim->set_res_2(track->res_2());
        anim->set_res_3(track->res_3());

        Ref<TRTrackMaterialTimeline> _mat_anim;
        _mat_anim.instantiate();

        Ref<TRAnimationInfo> _m_info;
        _m_info.instantiate();
        _m_info->set_does_loop(track->material_animation()->res_0()->does_loop());
        _m_info->set_animation_count(track->material_animation()->res_0()->animation_count());
        _m_info->set_animation_rate(track->material_animation()->res_0()->animation_rate());

        _mat_anim->set_info(_m_info);
        anim->set_mat_anim(_mat_anim);

        Ref<TRVisibilityShapeTimeline> _vis_anim;
        _vis_anim.instantiate();
        anim->set_vis_anim(_vis_anim);

        Ref<TRBlendShapeTimeline> _blendshape_anim;
        _blendshape_anim.instantiate();
        anim->set_blendshape_anim(_blendshape_anim);

        trackArr.push_back(anim);
    }
    set_tracks(trackArr);
}

Variant ResourceFormatLoaderTRACM::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRAnimationChannelMaterials> tracm;
    tracm.instantiate();
    tracm->LoadFromFile(p_path);
    return tracm;
}

PackedStringArray ResourceFormatLoaderTRACM::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("tracm");

    return exts;
}

bool ResourceFormatLoaderTRACM::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRAnimationChannelMaterials");
}