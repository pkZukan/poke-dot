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
    GETTER_SETTER_BIND(PokemonEntity, form, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(PokemonEntity, gender, Variant::INT, PROPERTY_HINT_NONE)

    ClassDB::bind_method(D_METHOD("Initialize"), &PokemonEntity::Initialize);
}

PokemonEntity::PokemonEntity()
{
    base_path = "res://Assets/ik_pokemon/data";

}

void PokemonEntity::Initialize()
{
    Ref<CatalogEntry> catEnt = PokemonCatalog::get_singleton()->GetCatalogEntry(species, form, gender);

    String relMdlPath = catEnt->get_model_path();
    _species_path = base_path.path_join(relMdlPath.get_base_dir());
    String _species_mdl = relMdlPath.get_file();
    Ref<AnimationResourceInfo> animInfo = catEnt->get_animations()[0];
    
    LoadActor(_species_path.path_join(_species_mdl), base_path.path_join(animInfo->get_path()));
}