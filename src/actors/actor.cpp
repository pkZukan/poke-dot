#include "actor.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void ActorObj::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("GetAnimationPlayer"), &ActorObj::GetAnimationPlayer);
    ClassDB::bind_method(D_METHOD("GetAnimationList"), &ActorObj::GetAnimationList);
}

void ActorObj::Initialize()
{
    //
}

void ActorObj::LoadActor(String mdlFile, String animFile)
{
    _cleanup();
    
    String base_path = mdlFile.get_base_dir();
    String filename = mdlFile.get_file();  

    //Create and add TrinityModel
    _model = memnew(TrinityModel);
    _model->load_model(base_path, filename);
    add_child(_model);

    //Create and add AnimationPlayer/AnimationLibrary
    _setup_animation();
    _load_animations(animFile);

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

TypedArray<StringName> ActorObj::GetAnimationList()
{
    return _anim_lib->get_animation_list();
}

AABB ActorObj::GetBBox()
{
    return _model->BBox;
}

void ActorObj::_cleanup() 
{
    if (_model) 
    {
        _model->queue_free();
        _model = nullptr;
    }
    if (_anim_player) 
    {
        _anim_player->queue_free();
        _anim_player = nullptr;
    }
    if (_debug_mesh) 
    {
        _debug_mesh->queue_free();
        _debug_mesh = nullptr;
    }
}

void ActorObj::_setup_animation() {
    //Setup animation player
    _anim_player = memnew(AnimationPlayer);
    add_child(_anim_player);

    _anim_player->set_name("AnimationPlayer");
    _anim_player->set_process_callback(AnimationPlayer::ANIMATION_PROCESS_IDLE);

    //init animation library
    _anim_lib.instantiate();
    _anim_player->add_animation_library("", _anim_lib);

    _skeleton = _find_skeleton(_model);
    if (!_skeleton) {
        UtilityFunctions::push_error("No Skeleton3D found in model");
        return;
    }
    _anim_player->set_root_node(_anim_player->get_path_to(this));

    //Get skeleton path
    _skl_path = String(get_path_to(_skeleton));
}

void ActorObj::_load_animation_parameter(String filepath)
{
    //
}

void ActorObj::_load_animation_resource(String filepath)
{
    String base_path = filepath.get_base_dir();
    Ref<TRAnimationChannelResource> tracr = ResourceLoader::get_singleton()->load(filepath);
    Ref<TRAnimationTrackListTable> table = tracr->get_table();
    auto list = table->get_list();
    for(int i = 0; i < list.size(); i++)
    {
        Ref<TRAnimationTrack> track = list[i];
        String name = track->get_Name();
        
        Ref<TRAnimationTrackResourceTable> res = track->get_Resources();
        Ref<TRAnimationTrackResource> anim = res->get_animation();
        if(anim.is_valid())
        {
            String file = base_path.path_join(anim->get_FileName());
            _add_animation(file, name);
        }
        Ref<TRAnimationTrackResource> mat = res->get_material();
        if(mat.is_valid())
        {
            String file = base_path.path_join(anim->get_FileName());
            //TODO: TRACM
        }
        Ref<TRAnimationTrackResource> eff = res->get_effect();
        if(eff.is_valid())
        {
            String file = base_path.path_join(anim->get_FileName());
            //TODO
        }
    }
}

void ActorObj::_load_animation_layer(String filepath)
{
    //
}

void ActorObj::_load_animation_state(String filepath)
{
    //
}

void ActorObj::_load_animation_look_at(String filepath)
{
    //
}

void ActorObj::_load_animation_slope_orientor(String filepath)
{
    //
}

void ActorObj::_load_animation_motion_detector(String filepath)
{
    //
}

void ActorObj::_load_animations(String tracn_file)
{
    if(tracn_file.is_empty()) return;
    
    //Load TRACN
    tracn_file = tracn_file.replace(".tracn", "_base.tracn"); //why, gamefreak?
    String pokeBase = tracn_file.get_base_dir();
    Ref<TRAnimationChannelNames> tracn = ResourceLoader::get_singleton()->load(tracn_file);

    //Load channels
    auto chanList = tracn->get_list();
    for(int i = 0; i < chanList.size(); i++)
    {
        Ref<AnimationChannelNameEntry> nameEnt = chanList[i];
        String aname = nameEnt->get_animation_name();
        String filepath = pokeBase.path_join(nameEnt->get_filename());
        if(aname == "parameter")
        {
            _load_animation_parameter(filepath);
        }
        else if(aname == "resource")
        {
            _load_animation_resource(filepath);
        }
        else if(aname == "layer")
        {
            _load_animation_layer(filepath);
        }
        else if(aname == "state")
        {
            _load_animation_state(filepath);
        }
        else if(aname == "look_at")
        {
            _load_animation_look_at(filepath);
        }
        else if(aname == "slope_orientor")
        {
            _load_animation_slope_orientor(filepath);
        }
        else if(aname == "motion_detector")
        {
            _load_animation_motion_detector(filepath);
        }
    }
}

void ActorObj::_add_animation(String anim_file, String name)
{
    Ref<Animation> godot_anim = TrinityAnimationConverter::convert_to_godot_animation(anim_file, _skeleton, _skl_path);
    if (!godot_anim.is_valid()) {
        UtilityFunctions::push_error("Animation conversion failed");
        return;
    }

    _anim_lib->add_animation(name, godot_anim);
}

Skeleton3D* ActorObj::_find_skeleton(Node* node) {
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

/*
*   Debug 
*/

void ActorObj::DebugDrawSkeleton() 
{
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