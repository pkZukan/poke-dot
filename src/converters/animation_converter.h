#pragma once
#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include "fbs/animation/tranm.h"
#include "fbs/animation/tracm.h"

namespace godot {

class TrinityAnimationConverter : public Object {
    GDCLASS(TrinityAnimationConverter, Object)

protected:
    static void _bind_methods();

public:
    static Ref<Animation> convert_to_godot_animation(
        const String& animFile, 
        Skeleton3D* skl, 
        const String& skl_path
    );

private:
    static Ref<BoneTrack> get_bone_track(const Ref<TRAnimation>& anim, const String& bone_name);

    static void sample_vector_track(
        Ref<Animation> anim, int track_idx,
        const Ref<Resource>& trk,
        float frame_rate, int key_frames
    );

    static void sample_rotation_track(
        Ref<Animation> anim, int track_idx,
        const Ref<Resource>& trk,
        float frame_rate, int key_frames
    );

public:
    static void sample_float_track(
        Ref<Animation> anim, int track_idx,
        const Ref<Resource>& trk,
        float frame_rate, int key_frames
    );

    static void sample_bool_track(
        Ref<Animation> anim, int track_idx,
        const Ref<Resource>& trk,
        float frame_rate, int key_frames
    );

    static void add_material_tracks(
        Ref<Animation> anim, Ref<TRTrackMaterial> track,
        const String& parameter_path, const Dictionary& parameter_types, float frame_rate
    );

private:
    static void sample_material_channel(
        Ref<Animation> anim, int track_idx,
        Ref<TRTrackMaterialChannel> chan,
        float frame_rate
    );
};

} // namespace godot