#include "pokemon_actor.h"
#include "gflib.h"

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
    ClassDB::bind_method(D_METHOD("GetPersonal"), &PokemonActor::GetPersonal);
    ClassDB::bind_method(D_METHOD("GetCatalogEntry"), &PokemonActor::GetCatalogEntry);
    ClassDB::bind_method(D_METHOD("GetEvolutions"), &PokemonActor::GetEvolutions);
    ClassDB::bind_method(D_METHOD("FindLevelUpEvolution", "level"), &PokemonActor::FindLevelUpEvolution);
    ClassDB::bind_method(D_METHOD("Evolve", "index"), &PokemonActor::Evolve);
    ADD_SIGNAL(MethodInfo("evolved", PropertyInfo(Variant::INT, "previous_species"),
                          PropertyInfo(Variant::INT, "previous_form")));
}

PokemonActor::PokemonActor()
{
    base_path = GFLib::romfs_path("ik_pokemon/data");
}

void PokemonActor::SetInfo(Ref<CatalogEntry> catalog, bool isShiny)
{
    _catalog = catalog;
    _isShiny = isShiny;
    Ref<SpeciesResourceInfo> info = _catalog.is_valid() ? _catalog->get_species_info() : Ref<SpeciesResourceInfo>();
    _personal = info.is_valid()
                    ? PokemonCatalog::get_singleton()->GetPersonalData(info->get_species(), info->get_form())
                    : Ref<TRPersonalData>();
}

void PokemonActor::SetShiny(bool isShiny)
{
    _isShiny = isShiny;
    if (TrinityModel* model = GetModel())
    {
        model->set_variation(_isShiny ? "rare" : "");
    }
}

void PokemonActor::Initialize()
{
    if (!_catalog.is_valid())
    {
        return;
    }

    String relMdlPath = _catalog->get_model_path();
    _species_path = base_path.path_join(relMdlPath.get_base_dir());
    String _species_mdl = relMdlPath.get_file();
    Ref<AnimationResourceInfo> animInfo = _catalog->get_animations()[0];

    String tracn_file = base_path.path_join(animInfo->get_path());
    // Pokemon animation files add _base to the catalog's .tracn path.
    tracn_file = tracn_file.replace(".tracn", "_base.tracn");
    LoadActor(_species_path.path_join(_species_mdl), tracn_file, _isShiny ? "rare" : "");
}

Array PokemonActor::GetEvolutions() const
{
    Array evolutions;
    if (_personal.is_null())
    {
        return evolutions;
    }
    const Array table = _personal->get_evo_table();
    for (int i = 0; i < table.size(); i++)
    {
        Ref<TREvoData> evo = table[i];
        if (evo.is_valid() && evo->get_species() != 0)
        {
            evolutions.push_back(evo);
        }
    }
    return evolutions;
}

int PokemonActor::FindLevelUpEvolution(int level) const
{
    const Array evolutions = GetEvolutions();
    for (int i = 0; i < evolutions.size(); i++)
    {
        Ref<TREvoData> evo = evolutions[i];
        if (evo->get_method() == 4 && level >= evo->get_level())
        {
            return i;
        }
    }
    return -1;
}

bool PokemonActor::Evolve(int index)
{
    const Array evolutions = GetEvolutions();
    ERR_FAIL_INDEX_V_MSG(index, evolutions.size(), false, "PokemonActor: no evolution at that index");
    Ref<TREvoData> evo = evolutions[index];
    Ref<SpeciesResourceInfo> current = _catalog->get_species_info();

    // Keep the gender model when the evolution has one; otherwise use its default model.
    PokemonCatalog* catalog = PokemonCatalog::get_singleton();
    uint8_t gender = current->get_gender();
    if (!catalog->HasCatalogEntry(evo->get_species(), evo->get_form(), gender))
    {
        gender = 0;
    }
    Ref<CatalogEntry> target = catalog->GetCatalogEntry(evo->get_species(), evo->get_form(), gender);
    if (target.is_null())
    {
        return false;
    }

    const int previous_species = current->get_species();
    const int previous_form = current->get_form();
    SetInfo(target, _isShiny);
    Initialize();
    emit_signal("evolved", previous_species, previous_form);
    return true;
}

String PokemonActor::GetIconPath()
{
    String origPath = base_path.path_join(_catalog->get_icon_path());
    String path = origPath.get_base_dir();
    String file = origPath.get_file().get_basename();
    String ext = "." + origPath.get_extension();
    return path.path_join("icon").path_join(file + (_isShiny ? "_1" : "_0") + ext);
}
