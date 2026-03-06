#include "pokemon_entity.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void PokemonEntity::_bind_methods() {
    GETTER_SETTER_BIND(PokemonEntity, species, Variant::INT, PROPERTY_HINT_NONE)

    ClassDB::bind_method(D_METHOD("PlayAnim", "name"), &PokemonEntity::PlayAnim);
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
    _anim_player->set_process_callback(AnimationPlayer::ANIMATION_PROCESS_IDLE);

    //Debug skeleton
    if(debug_skel)
    {
        _debug_mesh = memnew(MeshInstance3D);
        _imm_mesh.instantiate();
        _debug_mesh->set_mesh(_imm_mesh);
        add_child(_debug_mesh);

        Ref<StandardMaterial3D> mat;
        mat.instantiate();
        mat->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
        mat->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
        _debug_mesh->set_material_override(mat);
    }
}

void PokemonEntity::_process(double delta) {
    //Debug skeleton
    if(debug_skel)
    {
        if (!_skeleton || !_imm_mesh.is_valid()) return;
        
        _imm_mesh->clear_surfaces();
        _imm_mesh->surface_begin(Mesh::PRIMITIVE_LINES);
        
        for (int i = 0; i < _skeleton->get_bone_count(); i++) {
            int parent = _skeleton->get_bone_parent(i);
            if (parent < 0) continue;
            
            Transform3D bone_global  = _skeleton->get_bone_global_pose(i);
            Transform3D parent_global = _skeleton->get_bone_global_pose(parent);
            
            Vector3 bone_pos   = _skeleton->get_global_transform().xform(bone_global.origin);
            Vector3 parent_pos = _skeleton->get_global_transform().xform(parent_global.origin);
            
            _imm_mesh->surface_set_color(Color(1, 1, 0));
            _imm_mesh->surface_add_vertex(parent_pos);
            _imm_mesh->surface_add_vertex(bone_pos);
        }
        
        _imm_mesh->surface_end();
    }
}
void PokemonEntity::PlayAnim(String name)
{
    _anim_player->play(name);
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

    if (!godot_anim.is_valid()) {
        UtilityFunctions::push_error("Animation conversion failed");
        return;
    }

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