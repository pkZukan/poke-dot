#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/immediate_mesh.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include "converters/animation_converter.h"
#include "converters/model_converter.h"
#include "animation_fbs/tracn.h"
#include "animation_fbs/tracr.h"
#include "pokemon_catalog.h"
#include <utils.h>

namespace godot {

class PokemonEntity : public Node {
    GDCLASS(PokemonEntity, Node)
protected:
    static void _bind_methods();

public:
    PokemonEntity() = default;
    ~PokemonEntity() = default;

    void _ready() override;
    void _process(double delta) override;

    void PlayAnim(String name);
    TypedArray<StringName> GetAnimationList();

    GETTER_SETTER_DEFINE(int, species)

private:
    int species = 2;
    uint8_t form = 0;
    uint8_t gender = 0;

    String _species_str;
    AnimationPlayer* _anim_player = nullptr;
    Ref<AnimationLibrary> _anim_lib;
    TrinityModel* _model = nullptr;
    Skeleton3D* _skeleton = nullptr;
    String _skl_path;
    String _species_path;

    Skeleton3D* _find_skeleton(Node* node);
    void _setup_animation();
    void _add_animation(String anim_file, String name);
    void _load_animation_parameter(String filepath);
    void _load_animation_resource(String filepath);
    void _load_animation_layer(String filepath);
    void _load_animation_state(String filepath);
    void _load_animation_look_at(String filepath);
    void _load_animation_slope_orientor(String filepath);
    void _load_animation_motion_detector(String filepath);
    void _load_animations(Ref<AnimationResourceInfo> animInfo);

    //Debug
    bool debug_skel = false;
    MeshInstance3D* _debug_mesh = nullptr;
    Ref<ImmediateMesh> _imm_mesh;
};

} // namespace godot