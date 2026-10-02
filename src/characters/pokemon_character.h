#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include "actors/pokemon_actor.h"
#include "actors/actor.h"
#include "middleware/bntx.h"
#include <utils.h>

namespace godot {

class PokemonCharacter : public CharacterBody3D {
    GDCLASS(PokemonCharacter, CharacterBody3D)
protected:
    static void _bind_methods();

public:
    PokemonCharacter();
    // Godot deletes child nodes before destroying the extension instance.
    ~PokemonCharacter() = default;

    void _enter_tree() override;

    GETTER_SETTER_DEFINE(Ref<BinaryTexture>, icon)
    GETTER_SETTER_CALLBACK_DEFINE(uint16_t, species)
    GETTER_SETTER_CALLBACK_DEFINE(uint8_t, form)
    GETTER_SETTER_CALLBACK_DEFINE(uint8_t, gender)
    GETTER_SETTER_CALLBACK_DEFINE(bool, is_shiny)
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
    uint16_t species = 0;
    uint8_t form = 0;
    uint8_t gender = 0;
    bool is_shiny = false;
    float step_height = 0.3f;
    float max_root_motion_speed = 30.0f;
    Ref<BinaryTexture> icon;

    PokemonActor *_actor = nullptr;
    AnimationTree *_anim_tree = nullptr;
    
    CollisionShape3D *_col = nullptr;
    Ref<BoxShape3D> _col_shape;

    void _initialize();
    void _cleanup();
    void _try_step_up(const Vector3& motion);
};

} // namespace godot
