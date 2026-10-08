#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trpmcatalog_generated.h"
#include <utils.h>

namespace godot {

class LocatorResourceInfo : public Resource {
	GDCLASS(LocatorResourceInfo, Resource)
protected:
	static void _bind_methods();
public:
	LocatorResourceInfo(){}
	~LocatorResourceInfo(){}

	GETTER_SETTER_DEFINE(int16_t, form_number)
    GETTER_SETTER_DEFINE(uint8_t, loc_index)
    GETTER_SETTER_DEFINE(String, loc_path)

private:
	int16_t form_number;
    uint8_t loc_index;
    String loc_path;
};

class AnimationResourceInfo : public Resource {
	GDCLASS(AnimationResourceInfo, Resource)
protected:
	static void _bind_methods();
public:
	AnimationResourceInfo(){}
	~AnimationResourceInfo(){}

	GETTER_SETTER_DEFINE(int16_t, form_number)
    GETTER_SETTER_DEFINE(String, path)

private:
	int16_t form_number;
    String path;
};

class SpeciesResourceInfo : public Resource {
	GDCLASS(SpeciesResourceInfo, Resource)
protected:
	static void _bind_methods();
public:
	SpeciesResourceInfo(){}
	~SpeciesResourceInfo(){}

	GETTER_SETTER_DEFINE(uint16_t, species)
    GETTER_SETTER_DEFINE(uint16_t, form)
    GETTER_SETTER_DEFINE(uint8_t, gender)

private:
	uint16_t species;
    uint16_t form;
    uint8_t gender;
};

class CatalogEntry : public Resource {
	GDCLASS(CatalogEntry, Resource)
protected:
	static void _bind_methods();
public:
	CatalogEntry(){}
	~CatalogEntry(){}

	GETTER_SETTER_DEFINE(Ref<SpeciesResourceInfo>, species_info)
    GETTER_SETTER_DEFINE(String, model_path)
    GETTER_SETTER_DEFINE(String, material_table_path)
    GETTER_SETTER_DEFINE(String, config_path)
    GETTER_SETTER_DEFINE(Array, animations)
    GETTER_SETTER_DEFINE(Array, locators)
    GETTER_SETTER_DEFINE(String, icon_path)

private:
	Ref<SpeciesResourceInfo> species_info;
    String model_path;
    String material_table_path;
    String config_path;
    Array animations;
    Array locators;
    String icon_path;
};

class TRPpokemonCatalog : public Resource {
    GDCLASS(TRPpokemonCatalog, Resource)
protected:
	static void _bind_methods();
public:
	TRPpokemonCatalog(){}
	~TRPpokemonCatalog(){}

    void LoadFromFile(String file);
	
    GETTER_SETTER_DEFINE(int, version)
    GETTER_SETTER_DEFINE(Array, catalog_table)

private:
    int version;
    Array catalog_table;
};

class ResourceFormatLoaderTRPMCATALOG : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRPMCATALOG, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRPMCATALOG(){}
	~ResourceFormatLoaderTRPMCATALOG(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}