#include "animation_converter.h"

using namespace godot;

void TrinityAnimationConverter::_bind_methods() {
    ClassDB::bind_static_method("TrinityAnimationConverter",
        D_METHOD("convert_to_godot_animation", "path", "anim_file", "skl", "skl_path"),
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

        int rot_track = godot_anim->add_track(Animation::TYPE_ROTATION_3D);
        godot_anim->track_set_path(rot_track, bone_path);

        int scale_track = godot_anim->add_track(Animation::TYPE_SCALE_3D);
        godot_anim->track_set_path(scale_track, bone_path);

        if (bt == Ref<BoneTrack>()) 
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

Ref<BoneTrack> TrinityAnimationConverter::get_bone_track(
    const Ref<TRAnimation>& anim, const String& bone_name)
{
    Array tracks = anim->get_Track()->get_tracks();
    for (int i = 0; i < tracks.size(); i++) 
    {
        Ref<BoneTrack> bt = tracks[i];
        if (bt.is_valid() && bt->get_Name() == bone_name)
            return bt;
    }
    return Ref<BoneTrack>();
}

void TrinityAnimationConverter::sample_vector_track(
    Ref<Animation> anim, int track_idx,
    const Ref<Resource>& trk,
    float frame_rate, int key_frames)
{
    float end_time = (float)key_frames / frame_rate;

    if (Ref<FixedVectorTrack> fixed = trk; fixed.is_valid()) 
    {
        Vector3 value = fixed->get_co();
        anim->track_insert_key(track_idx, 0.0f,    value);
        anim->track_insert_key(track_idx, end_time, value);

    } 
    else if (Ref<DynamicVectorTrack> dynamic = trk; dynamic.is_valid()) 
    {
        Array co = dynamic->get_co();
        for (int frame = 0; frame < co.size(); frame++) 
        {
            float time = MIN((float)frame / frame_rate, end_time);
            anim->track_insert_key(track_idx, time, co[frame]);
        }

    } else if (Ref<Framed8VectorTrack> f8 = trk; f8.is_valid()) 
    {
        Array frames = f8->get_frames();
        Array co     = f8->get_co();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }

    } else if (Ref<Framed16VectorTrack> f16 = trk; f16.is_valid()) 
    {
        Array frames = f16->get_frames();
        Array co     = f16->get_co();
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
    float frame_rate, int key_frames)
{
    float end_time = (float)key_frames / frame_rate;

    if (Ref<FixedRotationTrack> fixed = trk; fixed.is_valid()) 
    {
        Quaternion quat = fixed->get_co();
        anim->track_insert_key(track_idx, 0.0f,    quat);
        anim->track_insert_key(track_idx, end_time, quat);

    } else if (Ref<DynamicRotationTrack> dynamic = trk; dynamic.is_valid()) 
    {
        Array co = dynamic->get_co();
        for (int frame = 0; frame < co.size(); frame++) 
        {
            float time = MIN((float)frame / frame_rate, end_time);
            anim->track_insert_key(track_idx, time, co[frame]);
        }

    } else if (Ref<Framed8RotationTrack> f8 = trk; f8.is_valid()) 
    {
        Array frames = f8->get_frames();
        Array co     = f8->get_co();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }

    } else if (Ref<Framed16RotationTrack> f16 = trk; f16.is_valid()) 
    {
        Array frames = f16->get_frames();
        Array co     = f16->get_co();
        for (int i = 0; i < frames.size(); i++) 
        {
            float time = (float)(int)frames[i] / frame_rate;
            anim->track_insert_key(track_idx, time, co[i]);
        }
    }
}