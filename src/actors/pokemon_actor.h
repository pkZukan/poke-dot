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
#include "actor.h"
#include <utils.h>

namespace godot {

class PokemonActor : public ActorObj {
    GDCLASS(PokemonActor, ActorObj)
protected:
    static void _bind_methods();

public:
    PokemonActor();
    ~PokemonActor() = default;

    void SetInfo(Ref<CatalogEntry> catalog, bool isShiny);

    void Initialize() override;

    String GetIconPath();

private:
    bool _isShiny = false;
    String _species_path, _species_str;
    Ref<CatalogEntry> _catalog;
};

} // namespace godot