#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "fbs/catalog/trpmcatalog.h"
#include "fbs/pml/personal_array.h"

#define CATALOG_PATH "ik_pokemon/catalog/catalog/poke_resource_table.trpmcatalog"
#define PERSONAL_PATH "avalon/data/personal_array.bin"

namespace godot
{
class PokemonCatalog : public Object
{
    GDCLASS(PokemonCatalog, Object)
    static PokemonCatalog* _singleton;

public:
    PokemonCatalog();
    ~PokemonCatalog();

    static PokemonCatalog* get_singleton()
    {
        return _singleton;
    }

    void load_catalog();
    void unload_catalog();
    Ref<CatalogEntry> GetCatalogEntry(int species, uint8_t form, uint8_t gender);
    bool HasCatalogEntry(int species, uint8_t form, uint8_t gender) const;
    Ref<TRPersonalData> GetPersonalData(int species, int form);

protected:
    static void _bind_methods();

private:
    Ref<TRPpokemonCatalog> _catalog;
    HashMap<uint64_t, int> _catalog_map;
    Ref<TRPersonalArray> _personal;
    HashMap<uint32_t, int> _personal_map;

    void _load_personal();
};
}
