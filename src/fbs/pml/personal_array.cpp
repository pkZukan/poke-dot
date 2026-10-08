#include "personal_array.h"

using namespace godot;

void TRSpeciesInfo::_bind_methods()
{
    GETTER_SETTER_BIND(TRSpeciesInfo, species_index, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, species_form, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, species_model, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, color, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, bodytype, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, height, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, weight, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, res0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, res1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSpeciesInfo, res2, Variant::INT, PROPERTY_HINT_NONE)
}

void TRStatInfo::_bind_methods()
{
    GETTER_SETTER_BIND(TRStatInfo, HP, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRStatInfo, ATK, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRStatInfo, DEF, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRStatInfo, SPA, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRStatInfo, SPD, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRStatInfo, SPE, Variant::INT, PROPERTY_HINT_NONE)
}

void TRGenderData::_bind_methods()
{
    GETTER_SETTER_BIND(TRGenderData, group, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRGenderData, ratio, Variant::INT, PROPERTY_HINT_NONE)
}

void TREvoData::_bind_methods()
{
    GETTER_SETTER_BIND(TREvoData, level, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREvoData, method, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREvoData, arg, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREvoData, res0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREvoData, res1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREvoData, res2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREvoData, species, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TREvoData, form, Variant::INT, PROPERTY_HINT_NONE)
}

void TRHatch::_bind_methods()
{
    GETTER_SETTER_BIND(TRHatch, species, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRHatch, form, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRHatch, region, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRHatch, can_evolve, Variant::INT, PROPERTY_HINT_NONE)
}

void TRZukanData::_bind_methods()
{
    GETTER_SETTER_BIND(TRZukanData, index, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRZukanData, group, Variant::INT, PROPERTY_HINT_NONE)
}

void TRMove::_bind_methods()
{
    GETTER_SETTER_BIND(TRMove, move, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRMove, level, Variant::INT, PROPERTY_HINT_NONE)
}

void TRPersonalData::_bind_methods()
{
    GETTER_SETTER_BIND(TRPersonalData, species, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRSpeciesInfo")
    GETTER_SETTER_BIND(TRPersonalData, is_present, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, dex, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRZukanData")
    GETTER_SETTER_BIND(TRPersonalData, type1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, type2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, ability1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, ability2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, ability3, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, growth, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, rate, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, gender, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRGenderData")
    GETTER_SETTER_BIND(TRPersonalData, group_1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, group_2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, hatch, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRHatch")
    GETTER_SETTER_BIND(TRPersonalData, hatch_steps, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, base_friendship, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, base_exp, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, evo_stage, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, unk_flag, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRPersonalData, yield_info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRStatInfo")
    GETTER_SETTER_BIND(TRPersonalData, stats, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRStatInfo")
    GETTER_SETTER_BIND(TRPersonalData, evo_table, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TREvoData")
    GETTER_SETTER_BIND(TRPersonalData, technical_machines, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(TRPersonalData, egg_moves, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(TRPersonalData, reminder_moves, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(TRPersonalData, learnset, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRMove")
}

void TRPersonalArray::_bind_methods() 
{
    GETTER_SETTER_BIND(TRPersonalArray, entries, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRPersonalData")
}

Ref<TRPersonalData> TRPersonalArray::_LoadEntry(const PersonalData *ent)
{
    Ref<TRPersonalData> pdata;
    pdata.instantiate();

    // SpeciesInfo
    auto sp = ent->species();
    if(sp)
    {
        Ref<TRSpeciesInfo> species;
        species.instantiate();
        species->set_species_index(sp->species_index());
        species->set_species_form(sp->species_form());
        species->set_species_model(sp->species_model());
        species->set_color(sp->color());
        species->set_bodytype(sp->bodytype());
        species->set_height(sp->height());
        species->set_weight(sp->weight());
        species->set_res0(sp->res0());
        species->set_res1(sp->res1());
        species->set_res2(sp->res2());
        pdata->set_species(species);
    }

    pdata->set_is_present(ent->is_present());

    // ZukanData
    auto dz = ent->dex();
    if(dz)
    {
        Ref<TRZukanData> dex;
        dex.instantiate();
        dex->set_index(dz->index());
        dex->set_group(dz->group());
        pdata->set_dex(dex);
    }

    pdata->set_type1(ent->type1());
    pdata->set_type2(ent->type2());
    pdata->set_ability1(ent->ability1());
    pdata->set_ability2(ent->ability2());
    pdata->set_ability3(ent->ability3());
    pdata->set_growth(ent->growth());
    pdata->set_rate(ent->rate());

    // GenderData
    auto gd = ent->gender();
    if(gd)
    {
        Ref<TRGenderData> gender;
        gender.instantiate();
        gender->set_group(gd->group());
        gender->set_ratio(gd->ratio());
        pdata->set_gender(gender);
    }

    pdata->set_group_1(ent->group_1());
    pdata->set_group_2(ent->group_2());

    // Hatch
    auto hd = ent->hatch();
    if(hd)
    {
        Ref<TRHatch> hatch;
        hatch.instantiate();
        hatch->set_species(hd->species());
        hatch->set_form(hd->form());
        hatch->set_region(hd->region());
        hatch->set_can_evolve(hd->can_evolve());
        pdata->set_hatch(hatch);
    }

    pdata->set_hatch_steps(ent->hatch_steps());
    pdata->set_base_friendship(ent->base_friendship());
    pdata->set_base_exp(ent->base_exp());
    pdata->set_evo_stage(ent->evo_stage());
    pdata->set_unk_flag(ent->unk_flag());

    // Yield StatInfo
    auto yi = ent->yield_info();
    if(yi)
    {
        Ref<TRStatInfo> yield_info;
        yield_info.instantiate();
        yield_info->set_HP(yi->HP());
        yield_info->set_ATK(yi->ATK());
        yield_info->set_DEF(yi->DEF());
        yield_info->set_SPA(yi->SPA());
        yield_info->set_SPD(yi->SPD());
        yield_info->set_SPE(yi->SPE());
        pdata->set_yield_info(yield_info);
    }

    // Base StatInfo
    auto bs = ent->stats();
    if(bs)
    {
        Ref<TRStatInfo> stats;
        stats.instantiate();
        stats->set_HP(bs->HP());
        stats->set_ATK(bs->ATK());
        stats->set_DEF(bs->DEF());
        stats->set_SPA(bs->SPA());
        stats->set_SPD(bs->SPD());
        stats->set_SPE(bs->SPE());
        pdata->set_stats(stats);
    }

    // EvoData
    auto ed = ent->evo_table();
    if(ed)
    {
        Ref<TREvoData> evo_table;
        evo_table.instantiate();
        evo_table->set_level(ed->level());
        evo_table->set_method(ed->method());
        evo_table->set_arg(ed->arg());
        evo_table->set_res0(ed->res0());
        evo_table->set_res1(ed->res1());
        evo_table->set_res2(ed->res2());
        evo_table->set_species(ed->species());
        evo_table->set_form(ed->form());
        pdata->set_evo_table(evo_table);
    }

    // Technical Machines
    auto tms = ent->technical_machines();
    if(tms)
    {
        Array tmArr;
        for(int j = 0; j < tms->size(); j++)
            tmArr.push_back((int)tms->Get(j));
        pdata->set_technical_machines(tmArr);
    }

    // Egg Moves
    auto ems = ent->egg_moves();
    if(ems)
    {
        Array emArr;
        for(int j = 0; j < ems->size(); j++)
            emArr.push_back((int)ems->Get(j));
        pdata->set_egg_moves(emArr);
    }

    // Reminder Moves
    auto rms = ent->reminder_moves();
    if(rms)
    {
        Array rmArr;
        for(int j = 0; j < rms->size(); j++)
            rmArr.push_back((int)rms->Get(j));
        pdata->set_reminder_moves(rmArr);
    }

    // Learnset
    auto ls = ent->learnset();
    if(ls)
    {
        Array lsArr;
        for(int j = 0; j < ls->size(); j++)
        {
            auto mv = ls->Get(j);
            Ref<TRMove> move;
            move.instantiate();
            move->set_move(mv->move());
            move->set_level(mv->level());
            lsArr.push_back(move);
        }
        pdata->set_learnset(lsArr);
    }

    return pdata;
}

void TRPersonalArray::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load personal array file: %s", file));
    auto personal = GetPersonalTable(buf.ptr());
    ERR_FAIL_COND_MSG(personal == NULL, vformat("Couldn't load personal"));

    Array entryArr;
    auto entries = personal->entries();
    if(entries)
    {
        for(int i = 0; i < entries->size(); i++)
        {
            auto ent = entries->Get(i);
            auto pdata = _LoadEntry(ent);
            entryArr.push_back(pdata);
        }
    }
    set_entries(entryArr);
}

Ref<TRPersonalData> TRPersonalArray::LazyLoadFromFile(String file, int idx)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_V_MSG(buf.is_empty(), Ref<TRPersonalData>(), vformat("Couldn't load personal array file: %s", file));
    auto personal = GetPersonalTable(buf.ptr());
    ERR_FAIL_COND_V_MSG(personal == NULL, Ref<TRPersonalData>(), vformat("Couldn't load personal"));

    auto entries = personal->entries();
    if(idx > entries->size())
        return Ref<TRPersonalData>();

    return _LoadEntry(entries->Get(idx));
}

int TRPersonalArray::FindIndex(String file, uint16_t species, uint8_t form)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_V_MSG(buf.is_empty(), -1, vformat("Couldn't load personal array file: %s", file));
    auto personal = GetPersonalTable(buf.ptr());
    ERR_FAIL_COND_V_MSG(personal == NULL, -1, vformat("Couldn't load personal"));

    auto entries = personal->entries();
    int idx = -1;
    if(entries)
    {
        for(int i = 0; i < entries->size(); i++)
        {
            auto ent = entries->Get(i)->species();
            if(ent->species_index() == species && ent->species_form() == form)
            {
                idx = i;
                break;
            }
        }
    }

    return idx;
}

Variant ResourceFormatLoaderTRPERSONAL::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRPersonalArray> personal;
    personal.instantiate();
    personal->LoadFromFile(p_path);
    return personal;
}

bool ResourceFormatLoaderTRPERSONAL::_recognize_path(const String &p_path, const StringName &p_type) const
{
    String filename = p_path.get_file();
    return filename == "personal_array.bin";
}

PackedStringArray ResourceFormatLoaderTRPERSONAL::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("bin");

    return exts;
}

bool ResourceFormatLoaderTRPERSONAL::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRPersonalArray");
}