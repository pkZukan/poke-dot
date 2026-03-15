#include "tracm.h"

#define TRACK_FLAG "FixedBoolTrack,DynamicBoolTrack,Framed16BoolTrack,Framed8BoolTrack"
#define BLEND_TRACK "FixedFloatTrack,DynamicFloatTrack,Framed16FloatTrack,Framed8FloatTrack"

using namespace godot;

void TRTrackFlagsInfo::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackFlagsInfo, values, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, TRACK_FLAG)
}

void TRTrackMaterialValue::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialValue, Time, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialValue, Value, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialValue, config_0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialValue, config_1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialValue, config_2, Variant::INT, PROPERTY_HINT_NONE)
}

void TRTrackMaterialChannel::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialChannel, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTrackMaterialValue")
}

void TRTrackMaterialChannelVec4::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialChannelVec4, x, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackMaterialChannel")
    GETTER_SETTER_BIND(TRTrackMaterialChannelVec4, y, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackMaterialChannel")
    GETTER_SETTER_BIND(TRTrackMaterialChannelVec4, z, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackMaterialChannel")
    GETTER_SETTER_BIND(TRTrackMaterialChannelVec4, w, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackMaterialChannel")
}

void TRTrackBlendshapeInit::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackBlendshapeInit, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackBlendshapeInit, blendshape_list, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "String")
}

void TRTrackBlendShape::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackBlendShape, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackBlendShape, init_values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTrackBlendshapeInit")
    GETTER_SETTER_BIND(TRTrackBlendShape, track, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, BLEND_TRACK)
    GETTER_SETTER_BIND(TRTrackBlendShape, blend_list_idx, Variant::INT, PROPERTY_HINT_NONE)
}

void TRTrackMaterialInit::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialInit, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialInit, list, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackMaterialValueList")
}

void TRTrackMaterialAnim::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialAnim, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialAnim, list, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackMaterialChannels")
}

void TRTrackMaterial::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterial, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterial, init_values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTrackMaterialInit")
    GETTER_SETTER_BIND(TRTrackMaterial, anim_values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTrackMaterialAnim")
}

void TRBlendTable::_bind_methods()
{
    GETTER_SETTER_BIND(TRBlendTable, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRBlendShapeTimeline::_bind_methods()
{
    GETTER_SETTER_BIND(TRBlendShapeTimeline, info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationInfo")
    GETTER_SETTER_BIND(TRBlendShapeTimeline, res_1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRBlendShapeTimeline, blendshape_tracks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTrackBlendShape")
    GETTER_SETTER_BIND(TRBlendShapeTimeline, res_3, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRBlendShapeTimeline, blend_list, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRBlendTable")
}

void TRVisibilityShapeTimeline::_bind_methods()
{
    GETTER_SETTER_BIND(TRVisibilityShapeTimeline, time, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRVisibilityShapeTimeline, value, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRVisibilityShapeTimeline, info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackFlagsInfo")
}

void TRTrackMaterialTimeline::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialTimeline, info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationInfo")
    GETTER_SETTER_BIND(TRTrackMaterialTimeline, res_1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialTimeline, material_tracks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTrackMaterial")
    GETTER_SETTER_BIND(TRTrackMaterialTimeline, unk3, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTrackMaterialTimeline, unk4, Variant::INT, PROPERTY_HINT_NONE)
}

void TRMeshAnimeTrack::_bind_methods()
{
    GETTER_SETTER_BIND(TRMeshAnimeTrack, path, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRMeshAnimeTrack, res_1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRMeshAnimeTrack, res_2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRMeshAnimeTrack, res_3, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRMeshAnimeTrack, mat_anim, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRTrackMaterialTimeline")
    GETTER_SETTER_BIND(TRMeshAnimeTrack, vis_anim, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRVisibilityShapeTimeline")
    GETTER_SETTER_BIND(TRMeshAnimeTrack, blendshape_anim, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRBlendShapeTimeline")
}

void TRAnimationChannelMeshes::_bind_methods() 
{
    GETTER_SETTER_BIND(TRAnimationChannelMeshes, info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRAnimationInfo")
    GETTER_SETTER_BIND(TRAnimationChannelMeshes, tracks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRMeshAnimeTrack")
}

Ref<TRTrackMaterialChannel> TRAnimationChannelMeshes::_LoadTrackMaterialChannel(const Titan::Animation::TrackMaterialValueList *matValList)
{
    Array vs;
    Ref<TRTrackMaterialChannel> trkMatValList;
    trkMatValList.instantiate();
    if(matValList)
    {
        auto vals = matValList->values();
        for(int k = 0; k < vals->size(); k++)
        {
            auto val = vals->Get(k);
            Ref<TRTrackMaterialValue> mval;
            mval.instantiate();
            mval->set_Time(val->time());
            mval->set_Value(val->value());
            mval->set_config_0(val->config_0());
            mval->set_config_1(val->config_1());
            mval->set_config_2(val->config_2());
            vs.push_back(mval);
        }
        trkMatValList->set_values(vs);
    }

    return trkMatValList;
}

Ref<TRTrackMaterialTimeline> TRAnimationChannelMeshes::_LoadMaterialAnims(const Titan::Animation::TrackMaterialTimeline *matAnim)
{
    Ref<TRTrackMaterialTimeline> anim;
    anim.instantiate();

    if(matAnim)
    {
        auto m_inf = matAnim->res_0();
        if(m_inf)
        {
            Ref<TRAnimationInfo> _info;
            _info.instantiate();
            _info->set_does_loop(m_inf->does_loop());
            _info->set_animation_count(m_inf->animation_count());
            _info->set_animation_rate(m_inf->animation_rate());
            anim->set_info(_info);
        }
        anim->set_res_1(matAnim->res_1());

        auto m_tracks = matAnim->material_track();
        if(m_tracks)
        {
            Array trkArr;
            for(int i = 0; i < m_tracks->size(); i++)
            {
                auto m_trk = m_tracks->Get(i);
                Ref<TRTrackMaterial> trk;
                trk.instantiate();
                trk->set_Name(Utils::toGodotString(m_trk->name()));

                auto init_vals = m_trk->init_values();
                if(init_vals)
                {
                    Array ivs;
                    for(int j = 0; j < init_vals->size(); j++)
                    {
                        auto iv = init_vals->Get(j);
                        Ref<TRTrackMaterialInit> initVal;
                        initVal.instantiate();
                        initVal->set_Name(Utils::toGodotString(iv->name()));
                        auto list = iv->list();
                        if(list)
                        {
                            Ref<TRTrackMaterialChannel> initValList = _LoadTrackMaterialChannel(list);
                            initVal->set_list(initValList);
                        }
                        ivs.push_back(initVal);
                    }
                    trk->set_init_values(ivs);
                }

                auto anim_vals = m_trk->anim_values();
                if(anim_vals)
                {
                    Array avs;
                    for(int j = 0; j < anim_vals->size(); j++)
                    {
                        auto av = anim_vals->Get(j);
                        Ref<TRTrackMaterialAnim> animVal;
                        animVal.instantiate();
                        animVal->set_Name(Utils::toGodotString(av->name()));

                        auto tma = av->list();
                        Ref<TRTrackMaterialChannelVec4> trkMatChan;
                        trkMatChan.instantiate();

                        Ref<TRTrackMaterialChannel> x = _LoadTrackMaterialChannel(tma->x());
                        trkMatChan->set_x(x);

                        Ref<TRTrackMaterialChannel> y = _LoadTrackMaterialChannel(tma->y());
                        trkMatChan->set_y(y);

                        Ref<TRTrackMaterialChannel> z = _LoadTrackMaterialChannel(tma->z());
                        trkMatChan->set_z(z);

                        Ref<TRTrackMaterialChannel> w = _LoadTrackMaterialChannel(tma->w());
                        trkMatChan->set_w(w);

                        animVal->set_list(trkMatChan);
                        avs.push_back(animVal);
                    }
                    trk->set_anim_values(avs);
                }

                trkArr.push_back(trk);
            }
            anim->set_material_tracks(trkArr);
        }
        anim->set_unk3(matAnim->unk_3());
        anim->set_unk4(matAnim->unk_4());
    }

    return anim;
}

Ref<Resource> TRAnimationChannelMeshes::_LoadBlendTracks(Titan::Animation::BlendTrack type, const void* data)
{
    switch (type) {
        case Titan::Animation::BlendTrack::BlendTrack_FixedBlendTrack: 
        {
            auto src = static_cast<const Titan::Animation::FixedBlendTrack*>(data);
            Ref<FixedFloatTrack> t; 
            t.instantiate();
            t->set_value(src->value());
            return t;
        }
        case Titan::Animation::BlendTrack::BlendTrack_DynamicBlendTrack: 
        {
            auto src = static_cast<const Titan::Animation::DynamicBlendTrack*>(data);
            Ref<DynamicFloatTrack> t; 
            t.instantiate();
            Array vals;
            for (size_t i = 0; i < src->value()->size(); i++)
                vals.append(src->value()->Get(i));
            t->set_values(vals);
            return t;
        }
        case Titan::Animation::BlendTrack::BlendTrack_Framed16BlendTrack: 
        {
            auto src = static_cast<const Titan::Animation::Framed16BlendTrack*>(data);
            Ref<Framed16FloatTrack> t; 
            t.instantiate();
            Array frames, vals;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->value()->size(); i++)
                vals.append(src->value()->Get(i));
            t->set_frames(frames); 
            t->set_values(vals);
            return t;
        }
        case Titan::Animation::BlendTrack::BlendTrack_Framed8BlendTrack: 
        {
            auto src = static_cast<const Titan::Animation::Framed8BlendTrack*>(data);
            Ref<Framed8FloatTrack> t; 
            t.instantiate();
            Array frames, vals;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->value()->size(); i++)
                vals.append(src->value()->Get(i));
            t->set_frames(frames); 
            t->set_values(vals);
            return t;
        }
        default: 
            return Ref<Resource>();
    }
}

Ref<Resource> TRAnimationChannelMeshes::_LoadTrackFlag(Titan::Animation::TrackFlag type, const void* data)
{
    switch (type) {
        case Titan::Animation::TrackFlag::TrackFlag_FixedBoolTrack: 
        {
            auto src = static_cast<const Titan::Animation::FixedBoolTrack*>(data);
            Ref<FixedBoolTrack> t; 
            t.instantiate();
            t->set_value(src->value());
            return t;
        }
        case Titan::Animation::TrackFlag::TrackFlag_DynamicBoolTrack: 
        {
            auto src = static_cast<const Titan::Animation::DynamicBoolTrack*>(data);
            Ref<DynamicBoolTrack> t; 
            t.instantiate();
            Array vals;
            for (size_t i = 0; i < src->value()->size(); i++)
                vals.append(src->value()->Get(i));
            t->set_values(vals);
            return t;
        }
        case Titan::Animation::TrackFlag::TrackFlag_Framed16BoolTrack: 
        {
            auto src = static_cast<const Titan::Animation::Framed16BoolTrack*>(data);
            Ref<Framed16BoolTrack> t; 
            t.instantiate();
            Array frames, vals;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->value()->size(); i++)
                vals.append(src->value()->Get(i));
            t->set_frames(frames); 
            t->set_values(vals);
            return t;
        }
        case Titan::Animation::TrackFlag::TrackFlag_Framed8BoolTrack: 
        {
            auto src = static_cast<const Titan::Animation::Framed8BoolTrack*>(data);
            Ref<Framed8BoolTrack> t; 
            t.instantiate();
            Array frames, vals;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->value()->size(); i++)
                vals.append(src->value()->Get(i));
            t->set_frames(frames); 
            t->set_values(vals);
            return t;
        }
        default: 
            return Ref<Resource>();
    }
}

Ref<TRVisibilityShapeTimeline> TRAnimationChannelMeshes::_LoadVisibilityAnims(const Titan::Animation::VisibilityShapeTimeline *visAnim)
{
    Ref<TRVisibilityShapeTimeline> anim;
    anim.instantiate();

    if(visAnim)
    {
        anim->set_time(visAnim->time());
        anim->set_value(visAnim->value());
        
        auto info = visAnim->info();
        if(info)
        {
            Ref<TRTrackFlagsInfo> vis_flag;
            vis_flag.instantiate();

            Ref<Resource> trackFlag = _LoadTrackFlag(info->values_type(), info->values());
            vis_flag->set_values(trackFlag);

            anim->set_info(vis_flag);
        }
    }

    return anim;
}

Ref<TRBlendShapeTimeline> TRAnimationChannelMeshes::_LoadBlendshapeAnims(const Titan::Animation::BlendShapeTimeline *blendAnim)
{
    Ref<TRBlendShapeTimeline> anim;
    anim.instantiate();

    if(blendAnim)
    {
        auto _inf = blendAnim->info();
        if(_inf)
        {
            Ref<TRAnimationInfo> _info;
            _info.instantiate();
            _info->set_does_loop(_inf->does_loop());
            _info->set_animation_count(_inf->animation_count());
            _info->set_animation_rate(_inf->animation_rate());

            anim->set_info(_info);
        }
        anim->set_res_1(blendAnim->res_1());
        auto blendTracks = blendAnim->blendshape_tracks();
        if(blendTracks)
        {
            Array bshapeArr;
            for(int i = 0; i < blendTracks->size(); i++)
            {
                auto trk = blendTracks->Get(i);
                Ref<TRTrackBlendShape> bshape;
                bshape.instantiate();
                bshape->set_Name(Utils::toGodotString(trk->name()));
                
                auto initVals = trk->init_values();
                Array initValArr;
                if(initVals)
                {
                    for(int j = 0; j < initVals->size(); j++)
                    {
                        auto iv = initVals->Get(j);
                        Ref<TRTrackBlendshapeInit> init_val;
                        init_val.instantiate();
                        init_val->set_Name(Utils::toGodotString(iv->mesh_name()));

                        auto list = iv->blendshape_list();
                        if(list)
                        {
                            Array bsArr;
                            for(int k = 0; k < list->size(); k++)
                            {
                                bsArr.push_back(Utils::toGodotString(list->Get(k)));
                            }
                            init_val->set_blendshape_list(bsArr);
                        }
                        initValArr.push_back(init_val);
                    }
                }
                bshape->set_init_values(initValArr);

                Ref<Resource> btrack = _LoadBlendTracks(trk->track_type(), trk->track());
                bshape->set_track(btrack);

                bshape->set_blend_list_idx(trk->blend_list_idx());
                bshapeArr.push_back(bshape);
            }
            anim->set_blendshape_tracks(bshapeArr);
        }
        anim->set_res_3(blendAnim->res_3());

        auto blist = blendAnim->blend_list();
        if(blist)
        {
            Array bshapeArr;
            for(int i = 0; i < blist->size(); i++)
            {
                auto trk = blist->Get(i);
                Ref<TRBlendTable> bshape;
                bshape.instantiate();
                bshape->set_Name(Utils::toGodotString(trk->name()));
                bshapeArr.push_back(bshape);
            }
            anim->set_blend_list(bshapeArr);
        }
    }

    return anim;
}

void TRAnimationChannelMeshes::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRACN file: %s", file));
    auto tracm = Titan::Animation::GetTRACM(buf.ptr());
    ERR_FAIL_COND_MSG(tracm == nullptr, "Couldn't load TRACM");

    auto _inf = tracm->info();
    if(_inf)
    {
        Ref<TRAnimationInfo> _info;
        _info.instantiate();
        _info->set_does_loop(_inf->does_loop());
        _info->set_animation_count(_inf->animation_count());
        _info->set_animation_rate(_inf->animation_rate());

        set_info(_info);
    }

    auto tracks = tracm->tracks();
    Array trackArr;
    for(int i = 0; i < tracks->size(); i++)
    {
        auto track = tracks->Get(i);

        Ref<TRMeshAnimeTrack> anim;
        anim.instantiate();
        anim->set_path(Utils::toGodotString(track->track_path()));
        anim->set_res_1(track->res_1());
        anim->set_res_2(track->res_2());
        anim->set_res_3(track->res_3());

        //Material anims
        Ref<TRTrackMaterialTimeline> _mat_anim = _LoadMaterialAnims(track->material_animation());
        anim->set_mat_anim(_mat_anim);

        //Visibility anims
        Ref<TRVisibilityShapeTimeline> _vis_anim = _LoadVisibilityAnims(track->visibility_animation());
        anim->set_vis_anim(_vis_anim);

        //Blendshape anims
        Ref<TRBlendShapeTimeline> _blendshape_anim = _LoadBlendshapeAnims(track->blendshape_animation());
        anim->set_blendshape_anim(_blendshape_anim);

        trackArr.push_back(anim);
    }
    set_tracks(trackArr);
}

Variant ResourceFormatLoaderTRACM::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRAnimationChannelMeshes> tracm;
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
    return p_type == String("TRAnimationChannelMeshes");
}