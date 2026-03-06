#include "pokemon_catalog.h"

using namespace godot;

PokemonCatalog* PokemonCatalog::_singleton = nullptr;

void PokemonCatalog::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("GetCatalogEntry"), &PokemonCatalog::GetCatalogEntry);
}

PokemonCatalog::PokemonCatalog() {
    if (_singleton == nullptr)
        _singleton = this;
}

PokemonCatalog::~PokemonCatalog() {
    if (_singleton == this)
        _singleton = nullptr;
    _catalog_map.clear();
}

void PokemonCatalog::load_catalog() 
{
    if (!ResourceLoader::get_singleton()->exists(CATALOG_PATH))
        UtilityFunctions::push_error("PokemonCatalog: file not found: ", CATALOG_PATH);

    _catalog = ResourceLoader::get_singleton()->load(CATALOG_PATH);
    auto ent = _catalog->get_catalog_table();
    for(int i = 0; i < ent.size(); i++)
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
}

Ref<CatalogEntry> PokemonCatalog::GetCatalogEntry(int species, uint8_t form, uint8_t gender)
{
    uint64_t key = ((uint64_t)species << 16) | ((uint64_t)form << 8) | (uint64_t)gender;
    if (!_catalog_map.has(key)) {
        UtilityFunctions::push_error("PokemonCatalog: no entry for species ", species);
        return Ref<CatalogEntry>();
    }
    return _catalog->get_catalog_table()[_catalog_map[key]];
}