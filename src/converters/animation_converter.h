#pragma once
#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include "animation_fbs/tranm.h"

namespace godot {

class TrinityAnimationConverter : public Object {
    GDCLASS(TrinityAnimationConverter, Object)

protected:
    static void _bind_methods();

public:
    static Ref<Animation> convert_to_godot_animation(const String& path, const String& animFile, Skeleton3D* skl, const String& skl_path);

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
};

} // namespace godot