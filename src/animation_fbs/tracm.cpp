#include "tracm.h"

using namespace godot;

void Framed8BoolTrack::_bind_methods()
{
    GETTER_SETTER_BIND(Framed8BoolTrack, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(Framed8BoolTrack, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "bool")
}

void Framed16BoolTrack::_bind_methods()
{
    GETTER_SETTER_BIND(Framed16BoolTrack, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(Framed16BoolTrack, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "bool")
}

void DynamicBoolTrack::_bind_methods()
{
    GETTER_SETTER_BIND(DynamicBoolTrack, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "bool")
}

void FixedBoolTrack::_bind_methods()
{
    GETTER_SETTER_BIND(FixedBoolTrack, value, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TRTrackFlagsInfo::_bind_methods()
{
    //Ref<Resource> values; //TRTrackFlag
}

void TRTrackMaterialValue::_bind_methods()
{
    //Ref<Resource> values; //TRTrackFlag
}


void TRTrackMaterialValueList::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialValueList, values, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTrackMaterialValue")
}

void TRTrackMaterialChannels::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackMaterialChannels, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRTrackBlendShape::_bind_methods()
{
    GETTER_SETTER_BIND(TRTrackBlendShape, Name, Variant::STRING, PROPERTY_HINT_NONE)
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
    //
}

void TRVisibilityShapeTimeline::_bind_methods()
{
    //
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
                        auto v = iv->list()->values();
                        if(v)
                        {
                            Array vs;
                            Ref<TRTrackMaterialValueList> initValList;
                            initValList.instantiate();
                            for(int k = 0; k < v->size(); k++)
                            {
                                auto val = v->Get(k);
                                Ref<TRTrackMaterialValue> mval;
                                mval.instantiate();
                                mval->set_Time(val->time());
                                mval->set_Value(val->value());
                                mval->set_config_0(val->config_0());
                                mval->set_config_1(val->config_1());
                                mval->set_config_2(val->config_2());
                                vs.push_back(mval);
                            }
                            initValList->set_values(vs);
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

                        //
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

Ref<TRVisibilityShapeTimeline> TRAnimationChannelMeshes::_LoadVisibilityAnims(const Titan::Animation::VisibilityShapeTimeline *visAnim)
{
    Ref<TRVisibilityShapeTimeline> anim;
    anim.instantiate();

    if(visAnim)
    {
        anim->set_time(visAnim->time());
        anim->set_value(visAnim->value());
        
        auto _v_info = visAnim->info();
        if(_v_info)
        {
            Ref<TRTrackFlagsInfo> vis_flag;
            vis_flag.instantiate();
            //TODO
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
                //TODO
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