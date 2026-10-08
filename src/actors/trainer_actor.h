#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/immediate_mesh.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include "converters/animation_converter.h"
#include "converters/model_converter.h"
#include "fbs/animation/tracn.h"
#include "fbs/animation/tracr.h"
#include "pokemon_catalog.h"
#include "actor.h"
#include <utils.h>

namespace godot {

class TrainerActor : public ActorObj {
    GDCLASS(TrainerActor, ActorObj)
protected:
    static void _bind_methods();

public:
    TrainerActor() = default;
    ~TrainerActor() = default;

    GETTER_SETTER_DEFINE(int, id)

    void Initialize() override;

private:
    int id = 0;

    String _trainer_mdl_path;
    String _trainer_mot_path;
};

} // namespace godot