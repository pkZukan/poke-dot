#include "trainer_character.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/kinematic_collision3d.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

TrainerCharacter::TrainerCharacter()
{
    set_floor_snap_length(0.35f);
}

void TrainerCharacter::_bind_methods() 
{
    GETTER_SETTER_BIND(TrainerCharacter, step_height, Variant::FLOAT, PROPERTY_HINT_RANGE, "0,1,0.01,or_greater,suffix:m")
    GETTER_SETTER_BIND(TrainerCharacter, max_root_motion_speed, Variant::FLOAT, PROPERTY_HINT_RANGE, "0,100,0.1,or_greater,suffix:m/s")

    ClassDB::bind_method(D_METHOD("GetRootMotionPos"), &TrainerCharacter::GetRootMotionPos);
    ClassDB::bind_method(D_METHOD("get_model"), &TrainerCharacter::get_model);
    ClassDB::bind_method(D_METHOD("get_animation_player"), &TrainerCharacter::get_animation_player);
    ClassDB::bind_method(D_METHOD("configure_animation_tree", "tree"), &TrainerCharacter::configure_animation_tree);
    ClassDB::bind_method(D_METHOD("apply_movement", "delta", "direction"), &TrainerCharacter::apply_movement, DEFVAL(Vector3(0, 0, 1)));
    ADD_SIGNAL(MethodInfo("character_rebuilt"));
}

void TrainerCharacter::_enter_tree()
{
    _initialize();
}

void TrainerCharacter::_initialize()
{
    _cleanup();

    _actor = memnew(TrainerActor);
    
    _col = memnew(CollisionShape3D);
    _col_shape.instantiate();
    _col->set_shape(_col_shape);

    _actor->set_name("Model");
    add_child(_actor);
    add_child(_col);

    _actor->Initialize();

    const AABB bounds = _actor->GetBBox();
    _col_shape->set_size(bounds.get_size());
    _col->set_position(bounds.get_center());

    emit_signal("character_rebuilt");
}

Node3D *TrainerCharacter::get_model() const
{
    return _actor;
}

AnimationPlayer *TrainerCharacter::get_animation_player() const
{
    return _actor ? _actor->GetAnimationPlayer() : nullptr;
}

void TrainerCharacter::configure_animation_tree(AnimationTree *tree)
{
    if (_anim_tree) _anim_tree->set_active(false);
    _anim_tree = tree;
    if (!_anim_tree) return;

    _anim_tree->set_active(false);

    AnimationPlayer *player = get_animation_player();
    if (!_actor || !player) return;

    Skeleton3D *skeleton = _actor->GetSkeleton();
    if (!skeleton || skeleton->get_bone_count() < 2) return;

    const String bone0 = skeleton->get_bone_name(0);
    const NodePath root_path(vformat("%s/%s:%s", bone0, bone0, skeleton->get_bone_name(1)));
    _anim_tree->set_root_node(_anim_tree->get_path_to(_actor));
    _anim_tree->set_animation_player(_anim_tree->get_path_to(player));
    _anim_tree->set_root_motion_local(true);
    _anim_tree->set_root_motion_track(root_path);
    _anim_tree->set_process_callback(AnimationTree::ANIMATION_PROCESS_MANUAL);
    for (StringName name : player->get_animation_list()) {
        Ref<Animation> animation = player->get_animation(name);
        const int track = animation->find_track(root_path, Animation::TYPE_POSITION_3D);
        // Accumulated root translation must not interpolate back to its origin.
        if (track >= 0) 
            animation->track_set_interpolation_loop_wrap(track, false);
    }
    _anim_tree->set_active(!Engine::get_singleton()->is_editor_hint());
}

void TrainerCharacter::_cleanup()
{
    // This is for rebuilding a live character, never for destruction. Stop
    // animation evaluation before detaching the nodes its tracks target.
    if (_anim_tree)
    {
        _anim_tree->set_active(false);
        _anim_tree = nullptr;
    }

    if (_actor)
    {
        remove_child(_actor);
        _actor->queue_free();
        _actor = nullptr;
    }

    if (_col)
    {
        remove_child(_col);
        _col->queue_free();
        _col = nullptr;
    }

    _col_shape.unref();
    icon.unref();

}

Vector3 TrainerCharacter::GetRootMotionPos()
{
    return _anim_tree ? _anim_tree->get_root_motion_position() : Vector3();
}

void TrainerCharacter::apply_movement(double delta, const Vector3 &direction)
{
    if (delta <= 0.0) return;
    Vector3 root_motion = GetRootMotionPos();
    
    if (!root_motion.is_finite() || (max_root_motion_speed > 0.0f && root_motion.length() > max_root_motion_speed * delta))
        root_motion = Vector3();

    Vector3 vel = get_velocity();
    if (is_on_floor() && vel.y <= 0.0f)
    {
        //Grounded motion
        Vector3 horizontal_direction(direction.x, 0, direction.z);
        if (!horizontal_direction.is_finite()) horizontal_direction = Vector3();
        Vector3 motion = get_global_transform().basis.xform(horizontal_direction.limit_length() * root_motion.z);
        vel.x = motion.x / delta;
        vel.z = motion.z / delta;
        if (vel.y < 0)
        {
            vel.y = 0;
        }

        //Curb/stair stepup
        _try_step_up(Vector3(vel.x, 0, vel.z) * delta);
    }
    else if (!is_on_floor())
    {
        vel.y -= 9.8 * delta;
    }
    
    set_velocity(vel);

    move_and_slide();
}

void TrainerCharacter::_try_step_up(const Vector3& motion)
{
    if (step_height <= 0.0f || motion.is_zero_approx()) 
        return;

    const float margin = get_safe_margin();
    const Vector3 lift(0, step_height + margin * 2.0f, 0);
    Transform3D probe = get_global_transform();
    
    Ref<KinematicCollision3D> hit;
    hit.instantiate();

    if (!test_move(probe, motion, hit, margin)) 
        return;

    if (hit->get_normal().y >= Math::cos(get_floor_max_angle())) 
        return;

    if (test_move(probe, lift, hit, margin)) 
        return;

    probe.origin += lift;
    if (test_move(probe, motion, hit, margin))
        return;

    probe.origin += motion;
    if (!test_move(probe, -lift, hit, margin)) 
        return;

    if (hit->get_normal().y < Math::cos(get_floor_max_angle())) 
        return;

    const float rise = lift.y + hit->get_travel().y;
    if (rise <= margin || rise > step_height + margin * 2.0f) 
        return;

    set_global_position(get_global_position() + Vector3(0, rise, 0));
}
