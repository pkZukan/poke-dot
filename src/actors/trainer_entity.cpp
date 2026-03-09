#include "trainer_entity.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TrainerEntity::_bind_methods() {
    GETTER_SETTER_BIND(TrainerEntity, id, Variant::INT, PROPERTY_HINT_NONE)

    ClassDB::bind_method(D_METHOD("Initialize"), &TrainerEntity::Initialize);
    ClassDB::bind_method(D_METHOD("PlayAnim", "name"), &ActorObj::PlayAnim);
    ClassDB::bind_method(D_METHOD("GetAnimationList"), &ActorObj::GetAnimationList);
}

void TrainerEntity::Initialize()
{
    String base_path = "res://Assets/ik_chara";
    
    _trainer_path = base_path.path_join("model_cc_ir/tr0001_00_rival_f");

    LoadActor(_trainer_path.path_join("tr0001_00.trmdl"), "");
}