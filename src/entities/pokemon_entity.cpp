#include "pokemon_entity.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void PokemonEntity::_bind_methods() {
    GETTER_SETTER_BIND(PokemonEntity, species, Variant::INT, PROPERTY_HINT_NONE)
}

void PokemonEntity::_ready() {
    _species_str = String::num_int64(species).pad_zeros(4);

    _species_path = vformat("res://Assets/ik_pokemon/data/pm%s/pm%s_00_00/", _species_str, _species_str);

    //Create and add TrinityModel
    _model = memnew(TrinityModel);
    _model->load_model(_species_path, vformat("pm%s_00_00.trmdl", _species_str));
    add_child(_model);

    // Create and add AnimationPlayer
    _anim_player = memnew(AnimationPlayer);
    _anim_player->set_name("AnimationPlayer");
    add_child(_anim_player);

    _setup_animation(_model);
    _anim_player->play("default");
}

void PokemonEntity::_setup_animation(Node* pkmn) {
    _skeleton = _find_skeleton(pkmn);
    if (!_skeleton) {
        UtilityFunctions::push_error("No Skeleton3D found in model");
        return;
    }

    _anim_player->set_root_node(_anim_player->get_path_to(this));
    String skl_path = String(get_path_to(_skeleton));

    //Build animation filename
    String anim_file = vformat("pm%s_00_00_00000_defaultwait01_loop.tranm", _species_str);

    //Convert animation files
    Ref<Animation> godot_anim = TrinityAnimationConverter::convert_to_godot_animation(
        _species_path, anim_file, _skeleton, skl_path
    );

    Ref<AnimationLibrary> anim_lib;
    anim_lib.instantiate();
    anim_lib->add_animation("default", godot_anim);
    _anim_player->add_animation_library("", anim_lib);
}

Skeleton3D* PokemonEntity::_find_skeleton(Node* node) {
    if (!node) return nullptr;
    if (Skeleton3D* skel = Object::cast_to<Skeleton3D>(node)) {
        return skel;
    }
    for (int i = 0; i < node->get_child_count(); i++) {
        Skeleton3D* result = _find_skeleton(node->get_child(i));
        if (result) return result;
    }
    return nullptr;
}