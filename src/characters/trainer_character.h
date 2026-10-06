#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include "actors/trainer_actor.h"
#include "actors/actor.h"
#include "middleware/bntx.h"
#include <utils.h>

namespace godot {

class TrainerCharacter : public CharacterBody3D {
    GDCLASS(TrainerCharacter, CharacterBody3D)
protected:
    static void _bind_methods();

public:
    TrainerCharacter();
    // Godot deletes child nodes before destroying the extension instance.
    ~TrainerCharacter() = default;

    void _enter_tree() override;

    GETTER_SETTER_DEFINE(float, step_height)
    GETTER_SETTER_DEFINE(float, max_root_motion_speed)

    void setterCallback(String setterName)
    {
        if (is_inside_tree())
            _initialize();
    }

    Vector3 GetRootMotionPos();

    Node3D *get_model() const;
    AnimationPlayer *get_animation_player() const;
    void configure_animation_tree(AnimationTree *tree);
    void apply_movement(double delta, const Vector3 &direction);

private:
    float step_height = 0.3f;
    float max_root_motion_speed = 30.0f;
    Ref<BinaryTexture> icon;

    TrainerActor *_actor = nullptr;
    AnimationTree *_anim_tree = nullptr;
    
    CollisionShape3D *_col = nullptr;
    Ref<BoxShape3D> _col_shape;

    void _initialize();
    void _cleanup();
    void _try_step_up(const Vector3& motion);
};

} // namespace godot
