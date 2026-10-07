#include "trainer_actor.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TrainerActor::_bind_methods() {
    GETTER_SETTER_BIND(TrainerActor, id, Variant::INT, PROPERTY_HINT_NONE)

    ClassDB::bind_method(D_METHOD("Initialize"), &TrainerActor::Initialize);
}

void TrainerActor::Initialize()
{
    String base_path = "res://Assets/ik_chara";
    
    _trainer_mdl_path = base_path.path_join("model_cc_ir/tr0001_00_rival_f");
    _trainer_mot_path = base_path.path_join("motion_cc_base/rv/tr0001_00_rival_f");

    LoadActor(_trainer_mdl_path.path_join("tr0001_00.trmdl"), _trainer_mot_path.path_join("tr0001_00_base.tracn"));

    //Quick hack to steal jump anims from player since npcs/rival dont have them
    const String player_motion_path = base_path.path_join("motion_pc/base");
    _add_animation(player_motion_path.path_join("p0_00_00210_jumpup01_start.tranm"), "00210_jumpup01_start");
    _add_animation(player_motion_path.path_join("p0_00_00211_jumpup01_loop.tranm"), "00211_jumpup01_loop");
    _add_animation(player_motion_path.path_join("p0_00_00212_jumpdown01_start.tranm"), "00212_jumpdown01_start");
    _add_animation(player_motion_path.path_join("p0_00_00213_jumpdown01_loop.tranm"), "00213_jumpdown01_loop");
    _add_animation(player_motion_path.path_join("p0_00_00215_land02.tranm"), "00215_land02");
}
