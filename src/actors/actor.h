#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/immediate_mesh.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include "converters/animation_converter.h"
#include "converters/model_converter.h"
#include "animation_fbs/tracn.h"
#include "animation_fbs/tracr.h"
#include "pokemon_catalog.h"
#include <utils.h>

namespace godot {

class ActorObj: public Node3D {
    GDCLASS(ActorObj, Node3D)
protected:
    static void _bind_methods();

public:
    ActorObj() = default;
    ~ActorObj() = default;

    virtual void Initialize();
    void _cleanup();
    virtual void LoadActor(String mdlFile, String animFile);

    virtual AnimationPlayer* GetAnimationPlayer() { return _anim_player; }
    virtual Skeleton3D* GetSkeleton() { return _skeleton; }

    virtual TypedArray<StringName> GetAnimationList();
    virtual AABB GetBBox();

protected:
    bool debug_skel = false;
    String base_path;

    void DebugDrawSkeleton();

private:
    AnimationPlayer* _anim_player = nullptr;
    Ref<AnimationLibrary> _anim_lib;
    TrinityModel* _model = nullptr;
    Skeleton3D* _skeleton = nullptr;
    String _skl_path;

    Skeleton3D* _find_skeleton(Node* node);
    void _setup_animation();
    void _add_animation(String anim_file, String name);
    void _add_mesh_animation(String tracm_file, String name);
    void _load_animation_parameter(String filepath);
    void _load_animation_resource(String filepath);
    void _load_animation_layer(String filepath);
    void _load_animation_state(String filepath);
    void _load_animation_look_at(String filepath);
    void _load_animation_slope_orientor(String filepath);
    void _load_animation_motion_detector(String filepath);
    void _load_animations(String tracn_file);

    //Debug
    MeshInstance3D* _debug_mesh = nullptr;
    Ref<ImmediateMesh> _imm_mesh;
};

} // namespace godot