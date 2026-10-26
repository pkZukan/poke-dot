#include "gflib.h"
#include "actors/pokemon_catalog.h"

#include <godot_cpp/classes/project_settings.hpp>

using namespace godot;

static const char* ROMFS_ROOT_SETTING = "gflib/romfs_root";

GFLib* GFLib::singleton = nullptr;

void GFLib::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_romfs_root", "path"), &GFLib::set_romfs_root);
    ClassDB::bind_method(D_METHOD("get_romfs_root"), &GFLib::get_romfs_root);
    ClassDB::bind_static_method("GFLib", D_METHOD("romfs_path", "relative"), &GFLib::romfs_path);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "romfs_root", PROPERTY_HINT_DIR), "set_romfs_root", "get_romfs_root");
}

GFLib::GFLib()
{
    if (singleton == nullptr)
    {
        singleton = this;
    }
}

GFLib::~GFLib()
{
    if (singleton == this)
    {
        singleton = nullptr;
    }
}

void GFLib::load_project_settings()
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    if (!settings->has_setting(ROMFS_ROOT_SETTING))
    {
        settings->set_setting(ROMFS_ROOT_SETTING, String());
    }
    settings->set_initial_value(ROMFS_ROOT_SETTING, String());
    settings->set_as_basic(ROMFS_ROOT_SETTING, true);

    Dictionary info;
    info["name"] = ROMFS_ROOT_SETTING;
    info["type"] = Variant::STRING;
    info["hint"] = PROPERTY_HINT_DIR;
    settings->add_property_info(info);

    romfs_root = settings->get_setting(ROMFS_ROOT_SETTING);
}

String GFLib::romfs_path(const String& relative)
{
    return singleton ? singleton->romfs_root.path_join(relative) : relative;
}

void GFLib::set_romfs_root(const String& path)
{
    if (romfs_root == path)
    {
        return;
    }
    romfs_root = path;
    // The catalog is read once at startup; refresh it from the new root.
    if (PokemonCatalog* catalog = PokemonCatalog::get_singleton())
    {
        catalog->unload_catalog();
        catalog->load_catalog();
    }
}
