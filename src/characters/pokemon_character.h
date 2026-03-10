#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/box_shape3d.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <godot_cpp/classes/animation_node_state_machine.hpp>
#include <godot_cpp/classes/animation_node_state_machine_playback.hpp>
#include <godot_cpp/classes/animation_node_animation.hpp>
#include <godot_cpp/classes/animation_node_time_scale.hpp>
#include "actors/pokemon_actor.h"
#include "actors/actor.h"
#include <utils.h>

namespace godot {

class PokemonCharacter : public CharacterBody3D {
    GDCLASS(PokemonCharacter, CharacterBody3D)
protected:
    static void _bind_methods();

public:
    PokemonCharacter() = default;
    ~PokemonCharacter() = default;

    void _enter_tree() override;
    void _ready() override;
    void _process(double delta) override;

    GETTER_SETTER_DEFINE(uint16_t, species)
    GETTER_SETTER_DEFINE(uint8_t, form)
    GETTER_SETTER_DEFINE(uint8_t, gender)

    void Initialize();

    void Idle();
    void Walk(float dir);
    void Run();
    void Roar();

private:
    uint16_t species = 0;
    uint8_t form = 0;
    uint8_t gender = 0;

    PokemonActor *_actor = nullptr;
    AnimationTree *_anim_tree = nullptr;
    Ref<AnimationNodeStateMachine> _anim_sm;
    CollisionShape3D *_col = nullptr;
    Ref<BoxShape3D> _col_shape;

    void _travel(const String& state);
};

} // namespace godot