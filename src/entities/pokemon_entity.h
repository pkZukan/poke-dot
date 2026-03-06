#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/immediate_mesh.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include "converters/animation_converter.h"
#include "converters/model_converter.h"
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

    GETTER_SETTER_DEFINE(int, species)

private:
    int species;
    String _species_str;
    AnimationPlayer* _anim_player = nullptr;
    TrinityModel* _model = nullptr;
    Skeleton3D* _skeleton = nullptr;
    String _species_path;

    Skeleton3D* _find_skeleton(Node* node);
    void _setup_animation(Node* pkmn);

    //Debug
    bool debug_skel = true;
    MeshInstance3D* _debug_mesh = nullptr;
    Ref<ImmediateMesh> _imm_mesh;
};

} // namespace godot