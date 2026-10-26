#include "pokemon_catalog.h"
#include "gflib.h"
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

PokemonCatalog* PokemonCatalog::_singleton = nullptr;

void PokemonCatalog::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetCatalogEntry"), &PokemonCatalog::GetCatalogEntry);
    ClassDB::bind_method(D_METHOD("GetPersonalData", "species", "form"), &PokemonCatalog::GetPersonalData);
}

PokemonCatalog::PokemonCatalog()
{
    if (_singleton == nullptr)
    {
        _singleton = this;
    }
}

PokemonCatalog::~PokemonCatalog()
{
    if (_singleton == this)
    {
        _singleton = nullptr;
    }
    _catalog_map.clear();
}

void PokemonCatalog::load_catalog()
{
    _load_personal();

    const String path = GFLib::romfs_path(CATALOG_PATH);
    if (!ResourceLoader::get_singleton()->exists(path))
    {
        UtilityFunctions::push_error("PokemonCatalog: file not found: ", path);
        return;
    }

    _catalog = ResourceLoader::get_singleton()->load(path);
    auto ent = _catalog->get_catalog_table();
    for (int i = 0; i < ent.size(); i++)
    {
        Ref<CatalogEntry> ce = ent[i];
        Ref<SpeciesResourceInfo> sri = ce->get_species_info();
        uint32_t species = sri->get_species();
        uint32_t form = sri->get_form();
        uint32_t gender = sri->get_gender();
        uint64_t key = ((uint64_t)species << 16) | ((uint64_t)form << 8) | (uint64_t)gender;
        _catalog_map[key] = i;
    }
}

void PokemonCatalog::unload_catalog()
{
    _catalog_map.clear();
    _catalog.unref();
    _personal_map.clear();
    _personal.unref();
}

Ref<CatalogEntry> PokemonCatalog::GetCatalogEntry(int species, uint8_t form, uint8_t gender)
{
    uint64_t key = ((uint64_t)species << 16) | ((uint64_t)form << 8) | (uint64_t)gender;
    if (!_catalog_map.has(key))
    {
        UtilityFunctions::push_error("PokemonCatalog: no entry for species ", species);
        return Ref<CatalogEntry>();
    }
    return _catalog->get_catalog_table()[_catalog_map[key]];
}

bool PokemonCatalog::HasCatalogEntry(int species, uint8_t form, uint8_t gender) const
{
    return _catalog_map.has(((uint64_t)species << 16) | ((uint64_t)form << 8) | (uint64_t)gender);
}

Ref<TRPersonalData> PokemonCatalog::GetPersonalData(int species, int form)
{
    const uint32_t key = (uint32_t(species) << 16) | uint32_t(form);
    if (_personal.is_null() || !_personal_map.has(key))
    {
        UtilityFunctions::push_error("PokemonCatalog: no personal data for species ", species, " form ", form);
        return Ref<TRPersonalData>();
    }
    return _personal->get_entries()[_personal_map[key]];
}

void PokemonCatalog::_load_personal()
{
    const String path = GFLib::romfs_path(PERSONAL_PATH);
    if (!FileAccess::file_exists(path))
    {
        UtilityFunctions::push_error("PokemonCatalog: file not found: ", path);
        return;
    }

    _personal = ResourceLoader::get_singleton()->load(path);
    if (_personal.is_null())
    {
        return;
    }
    // Forms are separate entries, so index by species and form together.
    Array entries = _personal->get_entries();
    for (int i = 0; i < entries.size(); i++)
    {
        Ref<TRPersonalData> entry = entries[i];
        Ref<TRSpeciesInfo> info = entry.is_valid() ? entry->get_species() : Ref<TRSpeciesInfo>();
        if (info.is_valid())
        {
            _personal_map[(uint32_t(info->get_species_index()) << 16) | uint32_t(info->get_species_form())] = i;
        }
    }
}
