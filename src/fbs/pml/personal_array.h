#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/personal_array_generated.h"
#include <utils.h>

namespace godot {

class TRSpeciesInfo : public Resource {
    GDCLASS(TRSpeciesInfo, Resource)
protected:
    static void _bind_methods();
public:
    TRSpeciesInfo(){}
    ~TRSpeciesInfo(){}
    GETTER_SETTER_DEFINE(int, species_index)
    GETTER_SETTER_DEFINE(int, species_form)
    GETTER_SETTER_DEFINE(int, species_model)
    GETTER_SETTER_DEFINE(int, color)
    GETTER_SETTER_DEFINE(int, bodytype)
    GETTER_SETTER_DEFINE(int, height)
    GETTER_SETTER_DEFINE(int, weight)
    GETTER_SETTER_DEFINE(int, res0)
    GETTER_SETTER_DEFINE(int, res1)
    GETTER_SETTER_DEFINE(int, res2)

private:
    int species_index;
    int species_form;
    int species_model;
    int color;
    int bodytype;
    int height;
    int weight;
    int res0;
    int res1;
    int res2;
};

class TRStatInfo : public Resource {
    GDCLASS(TRStatInfo, Resource)
protected:
    static void _bind_methods();
public:
    TRStatInfo(){}
    ~TRStatInfo(){}
    GETTER_SETTER_DEFINE(int, HP)
    GETTER_SETTER_DEFINE(int, ATK)
    GETTER_SETTER_DEFINE(int, DEF)
    GETTER_SETTER_DEFINE(int, SPA)
    GETTER_SETTER_DEFINE(int, SPD)
    GETTER_SETTER_DEFINE(int, SPE)

private:
    int HP;
    int ATK;
    int DEF;
    int SPA;
    int SPD;
    int SPE;
};

class TRGenderData : public Resource {
    GDCLASS(TRGenderData, Resource)
protected:
    static void _bind_methods();
public:
    TRGenderData(){}
    ~TRGenderData(){}
    GETTER_SETTER_DEFINE(int, group)
    GETTER_SETTER_DEFINE(int, ratio)

private:
    int group;
    int ratio;
};

class TREvoData : public Resource {
    GDCLASS(TREvoData, Resource)
protected:
    static void _bind_methods();
public:
    TREvoData(){}
    ~TREvoData(){}
    GETTER_SETTER_DEFINE(int, level)
    GETTER_SETTER_DEFINE(int, method)
    GETTER_SETTER_DEFINE(int, arg)
    GETTER_SETTER_DEFINE(int, res0)
    GETTER_SETTER_DEFINE(int, res1)
    GETTER_SETTER_DEFINE(int, res2)
    GETTER_SETTER_DEFINE(int, species)
    GETTER_SETTER_DEFINE(int, form)

private:
    int level;
    int method;
    int arg;
    int res0;
    int res1;
    int res2;
    int species;
    int form;
};

class TRHatch : public Resource {
    GDCLASS(TRHatch, Resource)
protected:
    static void _bind_methods();
public:
    TRHatch(){}
    ~TRHatch(){}
    GETTER_SETTER_DEFINE(int, species)
    GETTER_SETTER_DEFINE(int, form)
    GETTER_SETTER_DEFINE(int, region)
    GETTER_SETTER_DEFINE(int, can_evolve)

private:
    int species;
    int form;
    int region;
    int can_evolve;
};

class TRZukanData : public Resource {
    GDCLASS(TRZukanData, Resource)
protected:
    static void _bind_methods();
public:
    TRZukanData(){}
    ~TRZukanData(){}
    GETTER_SETTER_DEFINE(int, index)
    GETTER_SETTER_DEFINE(int, group)

private:
    int index;
    int group;
};

class TRMove : public Resource {
    GDCLASS(TRMove, Resource)
protected:
    static void _bind_methods();
public:
    TRMove(){}
    ~TRMove(){}
    GETTER_SETTER_DEFINE(int, move)
    GETTER_SETTER_DEFINE(int, level)
private:
    int move;
    int level;
};

class TRPersonalData : public Resource {
    GDCLASS(TRPersonalData, Resource)
protected:
    static void _bind_methods();
public:
    TRPersonalData(){}
    ~TRPersonalData(){}
    GETTER_SETTER_DEFINE(Ref<TRSpeciesInfo>, species)
    GETTER_SETTER_DEFINE(bool, is_present)
    GETTER_SETTER_DEFINE(Ref<TRZukanData>, dex)
    GETTER_SETTER_DEFINE(int, type1)
    GETTER_SETTER_DEFINE(int, type2)
    GETTER_SETTER_DEFINE(int, ability1)
    GETTER_SETTER_DEFINE(int, ability2)
    GETTER_SETTER_DEFINE(int, ability3)
    GETTER_SETTER_DEFINE(int, growth)
    GETTER_SETTER_DEFINE(int, rate)
    GETTER_SETTER_DEFINE(Ref<TRGenderData>, gender)
    GETTER_SETTER_DEFINE(int, group_1)
    GETTER_SETTER_DEFINE(int, group_2)
    GETTER_SETTER_DEFINE(Ref<TRHatch>, hatch)
    GETTER_SETTER_DEFINE(int, hatch_steps)
    GETTER_SETTER_DEFINE(int, base_friendship)
    GETTER_SETTER_DEFINE(int, base_exp)
    GETTER_SETTER_DEFINE(int, evo_stage)
    GETTER_SETTER_DEFINE(int, unk_flag)
    GETTER_SETTER_DEFINE(Ref<TRStatInfo>, yield_info)
    GETTER_SETTER_DEFINE(Ref<TRStatInfo>, stats)
    GETTER_SETTER_DEFINE(Ref<TREvoData>, evo_table)
    GETTER_SETTER_DEFINE(Array, technical_machines)
    GETTER_SETTER_DEFINE(Array, egg_moves)
    GETTER_SETTER_DEFINE(Array, reminder_moves)
    GETTER_SETTER_DEFINE(Array, learnset)
private:
    Ref<TRSpeciesInfo> species;
    bool is_present;
    Ref<TRZukanData> dex;
    int type1;
    int type2;
    int ability1;
    int ability2;
    int ability3;
    int growth;
    int rate;
    Ref<TRGenderData> gender;
    int group_1;
    int group_2;
    Ref<TRHatch> hatch;
    int hatch_steps;
    int base_friendship;
    int base_exp;
    int evo_stage;
    int unk_flag;
    Ref<TRStatInfo> yield_info;
    Ref<TRStatInfo> stats;
    Ref<TREvoData> evo_table;
    Array technical_machines;
    Array egg_moves;
    Array reminder_moves;
    Array learnset;
};

class TRPersonalArray : public Resource {
    GDCLASS(TRPersonalArray, Resource)
protected:
	static void _bind_methods();
public:
	TRPersonalArray(){}
	~TRPersonalArray(){}

    void LoadFromFile(String file);
    Ref<TRPersonalData> LazyLoadFromFile(String file, int idx);
    int FindIndex(String file, uint16_t species, uint8_t form);
	
    GETTER_SETTER_DEFINE(Array, entries)

private:
    Ref<TRPersonalData> _LoadEntry(const PersonalData *ent);

    Array entries;
};

class ResourceFormatLoaderTRPERSONAL : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderTRPERSONAL, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderTRPERSONAL(){}
	~ResourceFormatLoaderTRPERSONAL(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
    virtual bool _recognize_path(const String &p_path, const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}