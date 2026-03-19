#include "pokemon_character.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void PokemonCharacter::_bind_methods() 
{
    GETTER_SETTER_BIND(PokemonCharacter, icon, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "BinaryTexture")

    GETTER_SETTER_BIND(PokemonCharacter, species, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PokemonCharacter, form, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PokemonCharacter, gender, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PokemonCharacter, is_shiny, Variant::BOOL, PROPERTY_HINT_NONE)

    ClassDB::bind_method(D_METHOD("GetRootMotionPos"), &PokemonCharacter::GetRootMotionPos);

    ClassDB::bind_method(D_METHOD("Idle"), &PokemonCharacter::Idle);
    ClassDB::bind_method(D_METHOD("Walk", "dir"), &PokemonCharacter::Walk);
    ClassDB::bind_method(D_METHOD("Run"), &PokemonCharacter::Run);
    ClassDB::bind_method(D_METHOD("Roar"), &PokemonCharacter::Roar);
    ClassDB::bind_method(D_METHOD("Attack"), &PokemonCharacter::Attack);
    ClassDB::bind_method(D_METHOD("apply_movement", "delta"), &PokemonCharacter::apply_movement);
}

PokemonCharacter::~PokemonCharacter()
{
    _cleanup();
}

void PokemonCharacter::_enter_tree()
{
    _initialize();
}

void PokemonCharacter::_ready()
{
    //
}

void PokemonCharacter::_process(double delta)
{
    //
}

void PokemonCharacter::_travel(const String& state)
{
    Ref<AnimationNodeStateMachine> sm = _anim_tree->get_tree_root();
    if (sm.is_valid()) {
        AnimationNodeStateMachinePlayback* pb = Object::cast_to<AnimationNodeStateMachinePlayback>(
            _anim_tree->get("parameters/playback")
        );
        if (pb) pb->travel(state);
    }
}

void PokemonCharacter::_initialize()
{
    _cleanup();
    
    Ref<CatalogEntry> catEnt = PokemonCatalog::get_singleton()->GetCatalogEntry(species, form, gender);
    if (!catEnt.is_valid())
    {
        UtilityFunctions::printerr("Failed to get CatalogEntry for species ", species);
        return;
    }

    _actor = memnew(PokemonActor);
    _actor->SetInfo(catEnt, is_shiny);
    icon = ResourceLoader::get_singleton()->load(_actor->GetIconPath());

    _col = memnew(CollisionShape3D);
    _col_shape.instantiate();
    _col->set_shape(_col_shape);

    _anim_tree = memnew(AnimationTree);
    _anim_sm.instantiate();
    _anim_tree->set_tree_root(_anim_sm);
    _anim_tree->set_advance_expression_base_node(NodePath("."));

    add_child(_actor);
    add_child(_col);
    add_child(_anim_tree);

    _actor->Initialize();

    //Going to set the collision box as the bounding box for convenience
    _col_shape->set_size(_actor->GetBBox().get_size());

    Skeleton3D *_skel = _actor->GetSkeleton();

    String origin_bone = _skel->get_bone_name(1);
    String bone0 = _skel->get_bone_name(0);
    NodePath rm_path = NodePath(bone0 + "/" + bone0 + ":" + origin_bone);
    _anim_tree->set_root_motion_local(true);
    _anim_tree->set_root_motion_track(rm_path);
    _anim_tree->set_process_callback(AnimationTree::ANIMATION_PROCESS_PHYSICS);

    AnimationPlayer *player = _actor->GetAnimationPlayer();
    if(!player)
    {
        UtilityFunctions::printerr("AnimationPlayer null!");
        return;
    }

    _anim_tree->set_animation_player(player->get_path());
    _anim_tree->set_active(true);

    for (StringName anim_name : player->get_animation_list()) 
    {
        Ref<AnimationNodeAnimation> anim_node;
        anim_node.instantiate();
        anim_node->set_animation(anim_name);
        _anim_sm->add_node(anim_name, anim_node);
    }
}

void PokemonCharacter::_cleanup()
{
    if (_actor)
    {
        _actor->_cleanup();
        _actor->queue_free();
        _actor = nullptr;
    }

    if(_anim_tree)
    {
        _anim_tree->queue_free();
        _anim_tree = nullptr;
    }
}

Vector3 PokemonCharacter::GetRootMotionPos()
{
    return _anim_tree->get_root_motion_position();
}

void PokemonCharacter::Idle()
{
    _travel("00000_defaultwait01_loop");
}

void PokemonCharacter::Walk(float dir)
{
    _travel("00030_walk01_loop");
    
    Ref<AnimationNodeAnimation> anim_node = _anim_sm->get_node("00030_walk01_loop");
    if (anim_node.is_valid()) {
        anim_node->set_play_mode(dir >= 0 
            ? AnimationNodeAnimation::PLAY_MODE_FORWARD 
            : AnimationNodeAnimation::PLAY_MODE_BACKWARD);
    }
}

void PokemonCharacter::Run()
{
    _travel("00100_run01_loop");
}

void PokemonCharacter::Roar()
{
    _travel("00300_roar01");
}

void PokemonCharacter::Attack()
{
    _travel("00400_attack01");
}

void PokemonCharacter::apply_movement(double delta)
{
    Vector3 root_motion = GetRootMotionPos();

    float max_delta = 0.1f;
    if (root_motion.length() > max_delta)
    {
        root_motion = Vector3();
    }

    Vector3 vel = get_velocity();
    if (is_on_floor())
    {
        Vector3 motion = get_global_transform().basis.xform(root_motion);
        vel.x = motion.x / delta;
        vel.z = motion.z / delta;
        if (vel.y < 0)
        {
            vel.y = 0;
        }
    }
    else
    {
        vel.y -= 9.8 * delta;
    }
    set_velocity(vel);
    move_and_slide();
}