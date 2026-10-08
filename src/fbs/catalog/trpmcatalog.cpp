#include "trpmcatalog.h"

using namespace godot;

void LocatorResourceInfo::_bind_methods() 
{
    GETTER_SETTER_BIND(LocatorResourceInfo, form_number, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(LocatorResourceInfo, loc_index, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(LocatorResourceInfo, loc_path, Variant::STRING, PROPERTY_HINT_NONE)
}

void AnimationResourceInfo::_bind_methods() 
{
    GETTER_SETTER_BIND(AnimationResourceInfo, form_number, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationResourceInfo, path, Variant::STRING, PROPERTY_HINT_NONE)
}

void SpeciesResourceInfo::_bind_methods() 
{
    GETTER_SETTER_BIND(SpeciesResourceInfo, species, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SpeciesResourceInfo, form, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SpeciesResourceInfo, gender, Variant::INT, PROPERTY_HINT_NONE)
}

void CatalogEntry::_bind_methods() 
{
    GETTER_SETTER_BIND(CatalogEntry, species_info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "SpeciesResourceInfo")
    GETTER_SETTER_BIND(CatalogEntry, model_path, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(CatalogEntry, material_table_path, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(CatalogEntry, config_path, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(CatalogEntry, animations, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "AnimationResourceInfo")
    GETTER_SETTER_BIND(CatalogEntry, locators, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "LocatorResourceInfo")
    GETTER_SETTER_BIND(CatalogEntry, icon_path, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRPpokemonCatalog::_bind_methods() 
{
    GETTER_SETTER_BIND(TRPpokemonCatalog, version, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPpokemonCatalog, catalog_table, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "CatalogEntry")
}

void TRPpokemonCatalog::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRPMCATALOG file: %s", file));
    
    auto catalog = Titan::Resource::GetTRPmCatalog(buf.ptr());
    ERR_FAIL_COND_MSG(catalog == NULL, vformat("Couldn't load TRPMCATALOG"));

    set_version(catalog->version()->version());

    auto table = catalog->catalog_table();
    for(int i = 0; i < table->size(); i++)
    {
        auto ent = table->Get(i);

        Ref<CatalogEntry> catEnt;
        catEnt.instantiate();

        Ref<SpeciesResourceInfo> sinfo;
        sinfo.instantiate();
        sinfo->set_species(ent->species_info()->species());
        sinfo->set_form(ent->species_info()->form());
        sinfo->set_gender(ent->species_info()->gender());

        catEnt->set_species_info(sinfo);
        catEnt->set_model_path(Utils::toGodotString(ent->model_path()));
        catEnt->set_material_table_path(Utils::toGodotString(ent->material_table_path()));
        catEnt->set_config_path(Utils::toGodotString(ent->config_path()));

        Array animArr;
        if (ent->animations()) {
            auto anims = ent->animations();
            for (int j = 0; j < anims->size(); j++) {
                auto an = anims->Get(j);
                if (!an) continue;
                Ref<AnimationResourceInfo> anim;
                anim.instantiate();
                anim->set_form_number(an->form_number());
                anim->set_path(an->path() ? Utils::toGodotString(an->path()) : String());
                animArr.push_back(anim);
            }
        }
        catEnt->set_animations(animArr);

        Array locArr;
        if (ent->locators()) {
            auto locs = ent->locators();
            for (int j = 0; j < locs->size(); j++) {
                auto l = locs->Get(j);
                if (!l) continue;
                Ref<LocatorResourceInfo> loc;
                loc.instantiate();
                loc->set_form_number(l->form_number());
                loc->set_loc_index(l->loc_index());
                loc->set_loc_path(l->loc_path() ? Utils::toGodotString(l->loc_path()) : String());
                locArr.push_back(loc);
            }
        }
        catEnt->set_locators(locArr);

        catEnt->set_icon_path(Utils::toGodotString(ent->icon_path()));

        catalog_table.push_back(catEnt);
    }
}

Variant ResourceFormatLoaderTRPMCATALOG::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRPpokemonCatalog> trmbf;
    trmbf.instantiate();
    trmbf->LoadFromFile(p_path);
    return trmbf;
}

PackedStringArray ResourceFormatLoaderTRPMCATALOG::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trpmcatalog");

    return exts;
}

bool ResourceFormatLoaderTRPMCATALOG::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRPpokemonCatalog");
}