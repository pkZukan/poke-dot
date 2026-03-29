#include "animation_converter.h"
#include "animation_fbs/tracm.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TrinityAnimationConverter::_bind_methods() {
    ClassDB::bind_static_method("TrinityAnimationConverter",
        D_METHOD("convert_to_godot_animation", "anim_file", "skl", "skl_path"),
        &TrinityAnimationConverter::convert_to_godot_animation
    );
}

Ref<Animation> TrinityAnimationConverter::convert_to_godot_animation(
    const String& animFile, 
    Skeleton3D* skl, 
    const String& skl_path
) 
{
    Ref<TRAnimation> anim = ResourceLoader::get_singleton()->load(animFile);
    if (!anim.is_valid()) 
    {
        UtilityFunctions::push_error("Failed to load animation");
        return Ref<Animation>();
    }

    Ref<Animation> godot_anim;
    godot_anim.instantiate();
    
    float frame_rate = anim->get_Info()->get_animation_rate();
    int key_frames = anim->get_Info()->get_animation_count();
    godot_anim->set_length(key_frames / frame_rate);
    godot_anim->set_loop_mode(anim->get_Info()->get_does_loop() ? Animation::LOOP_LINEAR : Animation::LOOP_NONE);

    for (int bone_idx = 0; bone_idx < skl->get_bone_count(); bone_idx++) 
    {
        String bone_name = skl->get_bone_name(bone_idx);
        Ref<BoneTrack> bt = get_bone_track(anim, bone_name);

        // Get rest pose
        Transform3D rest = skl->get_bone_rest(bone_idx);
        Vector3 rest_loc = rest.origin;
        Quaternion rest_rot = rest.basis.get_rotation_quaternion();
        Vector3 rest_scale = rest.basis.get_scale();

        String bone_path = String(skl_path) + ":" + bone_name;

        // Create tracks
        int pos_track = godot_anim->add_track(Animation::TYPE_POSITION_3D);
        godot_anim->track_set_path(pos_track, bone_path);
        godot_anim->track_set_interpolation_type(pos_track, Animation::INTERPOLATION_CUBIC);

        int rot_track = godot_anim->add_track(Animation::TYPE_ROTATION_3D);
        godot_anim->track_set_path(rot_track, bone_path);
        godot_anim->track_set_interpolation_type(rot_track, Animation::INTERPOLATION_CUBIC);

        int scale_track = godot_anim->add_track(Animation::TYPE_SCALE_3D);
        godot_anim->track_set_path(scale_track, bone_path);
        godot_anim->track_set_interpolation_type(scale_track, Animation::INTERPOLATION_CUBIC);

        if (!bt.is_valid()) 
        {
            // No animation - use rest pose
            godot_anim->track_insert_key(pos_track, 0.0, rest_loc);
            godot_anim->track_insert_key(rot_track, 0.0, rest_rot);
            godot_anim->track_insert_key(scale_track, 0.0, rest_scale);
            continue;
        }

        // Has animation data
        if (!bt->get_translate().is_null())
            sample_vector_track(godot_anim, pos_track, bt->get_translate(), frame_rate, key_frames);
        else
            godot_anim->track_insert_key(pos_track, 0.0, rest_loc);

        if (!bt->get_rotate().is_null())
            sample_rotation_track(godot_anim, rot_track, bt->get_rotate(), frame_rate, key_frames);
        else
            godot_anim->track_insert_key(rot_track, 0.0, rest_rot);

        if (!bt->get_scale().is_null())
            sample_vector_track(godot_anim, scale_track, bt->get_scale(), frame_rate, key_frames);
        else
            godot_anim->track_insert_key(scale_track, 0.0, rest_scale);
    }

    return godot_anim;
}

void TrinityAnimationConverter::convert_tracm_to_godot_animation(
    const String& tracmFile,
    Ref<Animation> godot_anim
) 
{
    Ref<TRAnimationChannelMeshes> tracm = ResourceLoader::get_singleton()->load(tracmFile);
    if (!tracm.is_valid()) 
    {
        UtilityFunctions::push_error("Failed to load TRACM animation ", tracmFile);
        return;
    }

    float frame_rate = tracm->get_info()->get_animation_rate();
    int key_frames = tracm->get_info()->get_animation_count();

    float length = (float)key_frames / frame_rate;
    if (length > godot_anim->get_length()) {
        godot_anim->set_length(length);
    }

    Array tracks = tracm->get_tracks();
    for (int i = 0; i < tracks.size(); i++) 
    {
        Ref<TRMeshAnimeTrack> mat_trk = tracks[i];
        String mesh_path = mat_trk->get_path();

        // 1. Visibility
        Ref<TRVisibilityShapeTimeline> vis = mat_trk->get_vis_anim();
        if (vis.is_valid()) {
            int track_idx = godot_anim->add_track(Animation::TYPE_VALUE);
            godot_anim->track_set_path(track_idx, mesh_path + ":visible");
            sample_bool_track(godot_anim, track_idx, vis->get_info()->get_values(), frame_rate, key_frames);
        }

        // 2. Material Animations
        Ref<TRTrackMaterialTimeline> mat_anim = mat_trk->get_mat_anim();
        if (mat_anim.is_valid()) {
            Array mat_tracks = mat_anim->get_material_tracks();
            for (int j = 0; j < mat_tracks.size(); j++) {
                Ref<TRTrackMaterial> trk = mat_tracks[j];
                String mat_name = trk->get_Name();
                
                Array anim_values = trk->get_anim_values();
                for (int k = 0; k < anim_values.size(); k++) {
                    Ref<TRTrackMaterialAnim> m_anim = anim_values[k];
                    String param_name = m_anim->get_Name();
                    Ref<TRTrackMaterialChannelVec4> channels = m_anim->get_list();
                    
                    // We can sample each channel to its own property track if needed,
                    // but shader parameters are often Colors or Vector4s.
                    // For now, let's treat them as individual tracks for simplicity if they have data.
                    
                    if (channels->get_x().is_valid() && channels->get_x()->get_values().size() > 0) {
                        int trk_idx = godot_anim->add_track(Animation::TYPE_VALUE);
                        godot_anim->track_set_path(trk_idx, mesh_path + "_" + mat_name + ":material_override:shader_parameter/" + param_name + ":x");
                        sample_material_channel(godot_anim, trk_idx, channels->get_x(), frame_rate);
                    }
                    if (channels->get_y().is_valid() && channels->get_y()->get_values().size() > 0) {
                        int trk_idx = godot_anim->add_track(Animation::TYPE_VALUE);
                        godot_anim->track_set_path(trk_idx, mesh_path + "_" + mat_name + ":material_override:shader_parameter/" + param_name + ":y");
                        sample_material_channel(godot_anim, trk_idx, channels->get_y(), frame_rate);
                    }
                    if (channels->get_z().is_valid() && channels->get_z()->get_values().size() > 0) {
                        int trk_idx = godot_anim->add_track(Animation::TYPE_VALUE);
                        godot_anim->track_set_path(trk_idx, mesh_path + "_" + mat_name + ":material_override:shader_parameter/" + param_name + ":z");
                        sample_material_channel(godot_anim, trk_idx, channels->get_z(), frame_rate);
                    }
                    if (channels->get_w().is_valid() && channels->get_w()->get_values().size() > 0) {
                        int trk_idx = godot_anim->add_track(Animation::TYPE_VALUE);
                        godot_anim->track_set_path(trk_idx, mesh_path + "_" + mat_name + ":material_override:shader_parameter/" + param_name + ":w");
                        sample_material_channel(godot_anim, trk_idx, channels->get_w(), frame_rate);
                    }
                }
            }
        }
        
        // 3. Blendshapes
        Ref<TRBlendShapeTimeline> blend_anim = mat_trk->get_blendshape_anim();
        if (blend_anim.is_valid()) {
            Array btracks = blend_anim->get_blendshape_tracks();
            for (int j = 0; j < btracks.size(); j++) {
                Ref<TRTrackBlendShape> btrk = btracks[j];
                String bname = btrk->get_Name();
                int track_idx = godot_anim->add_track(Animation::TYPE_BLEND_SHAPE);
                godot_anim->track_set_path(track_idx, mesh_path + ":" + bname);
                sample_float_track(godot_anim, track_idx, btrk->get_track(), frame_rate, key_frames);
            }
        }
    }
}

void TrinityAnimationConverter::sample_float_track(
    Ref<Animation> anim, int track_idx,
    const Ref<Resource>& trk,
    float frame_rate, int key_frames
)
{
    float end_time = (float)key_frames / frame_rate;

    if (Ref<FixedFloatTrack> fixed = trk; fixed.is_valid()) 
    {
        float value = fixed->get_value();
        anim->track_insert_key(track_idx, 0.0f, value);
        anim->track_insert_key(track_idx, end_time, value);
    } 
    else if (Ref<DynamicFloatTrack> dynamic = trk; dynamic.is_valid()) 
    {
        Array co = dynamic->get_values();
        for (int frame = 0; frame < co.size(); frame++) 
        {
            float time = MIN((float)frame / frame_rate, end_time);
            anim->track_insert_key(track_idx, time, co[frame]);
        }
    } else if (Ref<Framed8FloatTrack> f8 = trk; f8.is_valid()) 
    {
        Array frames = f8->get_frames();
        Array co     = f8->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }
    } else if (Ref<Framed16FloatTrack> f16 = trk; f16.is_valid()) 
    {
        Array frames = f16->get_frames();
        Array co     = f16->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }
    }
}

void TrinityAnimationConverter::sample_bool_track(
    Ref<Animation> anim, int track_idx,
    const Ref<Resource>& trk,
    float frame_rate, int key_frames
)
{
    float end_time = (float)key_frames / frame_rate;

    if (Ref<FixedBoolTrack> fixed = trk; fixed.is_valid()) 
    {
        bool value = fixed->get_value();
        anim->track_insert_key(track_idx, 0.0f, value);
        anim->track_insert_key(track_idx, end_time, value);
    } 
    else if (Ref<DynamicBoolTrack> dynamic = trk; dynamic.is_valid()) 
    {
        Array co = dynamic->get_values();
        for (int frame = 0; frame < co.size(); frame++) 
        {
            float time = MIN((float)frame / frame_rate, end_time);
            anim->track_insert_key(track_idx, time, co[frame]);
        }
    } else if (Ref<Framed8BoolTrack> f8 = trk; f8.is_valid()) 
    {
        Array frames = f8->get_frames();
        Array co     = f8->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }
    } else if (Ref<Framed16BoolTrack> f16 = trk; f16.is_valid()) 
    {
        Array frames = f16->get_frames();
        Array co     = f16->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }
    }
}

void TrinityAnimationConverter::sample_material_channel(
    Ref<Animation> anim, int track_idx,
    Ref<TRTrackMaterialChannel> chan,
    float frame_rate
)
{
    Array values = chan->get_values();
    for (int i = 0; i < values.size(); i++) {
        Ref<TRTrackMaterialValue> mval = values[i];
        anim->track_insert_key(track_idx, mval->get_Time() / frame_rate, mval->get_Value());
    }
}

Ref<BoneTrack> TrinityAnimationConverter::get_bone_track(
    const Ref<TRAnimation>& anim, const String& bone_name)
{
    Array tracks = anim->get_Track()->get_tracks();
    for (int i = 0; i < tracks.size(); i++) 
    {
        Ref<BoneTrack> bt = tracks[i];
        if (bt.is_valid() && (bt->get_Name() == bone_name))
            return bt;
    }

    return nullptr;
}

void TrinityAnimationConverter::sample_vector_track(
    Ref<Animation> anim, int track_idx,
    const Ref<Resource>& trk,
    float frame_rate, int key_frames
)
{
    float end_time = (float)key_frames / frame_rate;

    if (Ref<FixedVectorTrack> fixed = trk; fixed.is_valid()) 
    {
        Vector3 value = fixed->get_value();
        anim->track_insert_key(track_idx, 0.0f, value);
        anim->track_insert_key(track_idx, end_time, value);

    } 
    else if (Ref<DynamicVectorTrack> dynamic = trk; dynamic.is_valid()) 
    {
        Array co = dynamic->get_values();
        for (int frame = 0; frame < co.size(); frame++) 
        {
            float time = MIN((float)frame / frame_rate, end_time);
            anim->track_insert_key(track_idx, time, co[frame]);
        }

    } else if (Ref<Framed8VectorTrack> f8 = trk; f8.is_valid()) 
    {
        Array frames = f8->get_frames();
        Array co     = f8->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }

    } else if (Ref<Framed16VectorTrack> f16 = trk; f16.is_valid()) 
    {
        Array frames = f16->get_frames();
        Array co     = f16->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }
    }
}

void TrinityAnimationConverter::sample_rotation_track(
    Ref<Animation> anim, int track_idx,
    const Ref<Resource>& trk,
    float frame_rate, int key_frames
)
{
    float end_time = (float)key_frames / frame_rate;

    if (Ref<FixedRotationTrack> fixed = trk; fixed.is_valid()) 
    {
        Quaternion quat = fixed->get_value();
        anim->track_insert_key(track_idx, 0.0f, quat);
        anim->track_insert_key(track_idx, end_time, quat);

    } 
    else if (Ref<DynamicRotationTrack> dynamic = trk; dynamic.is_valid()) 
    {
        Array co = dynamic->get_values();
        for (int frame = 0; frame < co.size(); frame++) 
        {
            float time = MIN((float)frame / frame_rate, end_time);
            anim->track_insert_key(track_idx, time, co[frame]);
        }

    } 
    else if (Ref<Framed8RotationTrack> f8 = trk; f8.is_valid()) 
    {
        Array frames = f8->get_frames();
        Array co     = f8->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }

    } 
    else if (Ref<Framed16RotationTrack> f16 = trk; f16.is_valid()) 
    {
        Array frames = f16->get_frames();
        Array co     = f16->get_values();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }
    }
}