#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "catalog_fbs/trpmcatalog.h"

#define CATALOG_PATH "res://Assets/ik_pokemon/catalog/catalog/poke_resource_table.trpmcatalog"

namespace godot
{
class PokemonCatalog : public Object {
    GDCLASS(PokemonCatalog, Object)
    static PokemonCatalog* _singleton;

public:
    PokemonCatalog();
    ~PokemonCatalog();

    static PokemonCatalog *get_singleton() 
    {
        return _singleton;
    }

    void load_catalog();
    void unload_catalog();
    Ref<CatalogEntry> GetCatalogEntry(int species, uint8_t form, uint8_t gender);

protected:
    static void _bind_methods();

private:
    Ref<TRPpokemonCatalog> _catalog;
    HashMap<uint64_t, int> _catalog_map;
};
} //namespace godot