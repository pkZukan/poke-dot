#include "pokemon_actor.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void PokemonActor::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("Initialize"), &PokemonActor::Initialize);
}

PokemonActor::PokemonActor()
{
    base_path = "res://Assets/ik_pokemon/data";
}

void PokemonActor::SetInfo(Ref<CatalogEntry> catalog, bool isShiny)
{
    _catalog = catalog;
    _isShiny = isShiny;
}

void PokemonActor::Initialize()
{
    if(!_catalog.is_valid()) return;

    String relMdlPath = _catalog->get_model_path();
    _species_path = base_path.path_join(relMdlPath.get_base_dir());
    String _species_mdl = relMdlPath.get_file();
    Ref<AnimationResourceInfo> animInfo = _catalog->get_animations()[0];
    
    LoadActor(_species_path.path_join(_species_mdl), base_path.path_join(animInfo->get_path()));
}

String PokemonActor::GetIconPath()
{
    String origPath = base_path.path_join(_catalog->get_icon_path());
    String path = origPath.get_base_dir();
    String file = origPath.get_file().get_basename();
    String ext = "." + origPath.get_extension();
    return path.path_join("icon").path_join(file + (_isShiny ? "_1" : "_0") + ext);
}