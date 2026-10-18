#include "trinity_map_replace.h"
#include "fbs/bfbs.h"
#include "gflib.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TrinityMapReplace::_bind_methods()
{
    ClassDB::bind_static_method("TrinityMapReplace", D_METHOD("default_path"), &TrinityMapReplace::default_path);
    ClassDB::bind_method(D_METHOD("load", "path"), &TrinityMapReplace::load);
    ClassDB::bind_method(D_METHOD("get_entries"), &TrinityMapReplace::get_entries);
    ClassDB::bind_method(D_METHOD("has_scene", "scene"), &TrinityMapReplace::has_scene);
    ClassDB::bind_method(D_METHOD("is_entry_active", "entry", "state"), &TrinityMapReplace::is_entry_active);
    ClassDB::bind_method(D_METHOD("is_scene_active", "scene", "state"), &TrinityMapReplace::is_scene_active);
}

namespace
{
int64_t arg(const Array &args, int index)
{
    return index < args.size() ? String(args[index]).to_int() : 0;
}

// Comparison ops, inferred from how the table's entries chain together.
bool compare(int64_t value, int op, const Array &args, int first)
{
    const int64_t a = arg(args, first);
    const int64_t b = arg(args, first + 1);
    switch (op)
    {
        case 1: return value == a;
        case 2: return value != a;
        case 3: return value > a;
        case 4: return value < a;
        case 5: return value >= a;
        case 6: return value <= a;
        case 8: return value >= a && value <= b;
        case 100: return value >= a && value < b;
        default:
            UtilityFunctions::push_warning("Map replace: unknown compare op ", op);
            return false;
    }
}

bool flag_set(const Dictionary &state, const String &name)
{
    const Variant flags = state.get("flags", Variant());
    if (flags.get_type() == Variant::DICTIONARY)
        return bool(Dictionary(flags).get(name, false));
    if (flags.get_type() == Variant::ARRAY)
        return Array(flags).has(name);
    return false;
}

bool condition_met(const Dictionary &param, const Dictionary &state)
{
    const String condition = param.get("condition", "");
    const int op = param.get("op", 0);
    const Variant raw_args = param.get("param", Variant());
    const Array args = raw_args.get_type() == Variant::ARRAY ? Array(raw_args) : Array();

    if (condition == "phase_condition")
        return compare(state.get("phase", 0), op, args, 0);
    if (condition == "flag_condition")
        return flag_set(state, args.is_empty() ? String() : String(args[0])) == (op == 1);
    if (condition == "work_condition")
    {
        const Variant works = state.get("works", Dictionary());
        const int64_t value = works.get_type() == Variant::DICTIONARY && !args.is_empty() ? int64_t(Dictionary(works).get(args[0], 0)) : 0;
        return compare(value, op, args, 1);
    }
    if (condition == "player_sex_condition")
        return compare(state.get("player_sex", 0), op, args, 0);
    if (condition == "another_dimension_rank_condition")
        return compare(state.get("another_dimension_rank", 0), op, args, 0);
    if (condition == "birthday_condition")
        return bool(state.get("birthday", false)) == (op == 1);
    if (condition == "time_noon_condition")
        return state.get("noon", true);
    if (condition == "force_condition")
        return true;
    UtilityFunctions::push_warning("Map replace: unknown condition ", condition);
    return false;
}
}

String TrinityMapReplace::default_path()
{
    return GFLib::romfs_path("world/ik_data/field/map_replace/map_replace_data/map_replace_data_array.bin");
}

Error TrinityMapReplace::load(const String &path)
{
    entries.clear();
    scene_entries.clear();
    Dictionary table = FlatbufferSchema::decode_table(path);
    ERR_FAIL_COND_V_MSG(table.is_empty(), ERR_FILE_CORRUPT, "Couldn't decode map replace table: " + path);

    Array values = table.get("values", Array());
    for (int v = 0; v < values.size(); v++)
    {
        Array root = Dictionary(values[v]).get("root", Array());
        for (int i = 0; i < root.size(); i++)
        {
            Dictionary entry = root[i];
            entries.push_back(entry);
            Array scenes = entry.get("loadSceneNames", Array());
            for (int s = 0; s < scenes.size(); s++)
            {
                if (!scene_entries.has(scenes[s]))
                    scene_entries[scenes[s]] = Array();
                Array(scene_entries[scenes[s]]).push_back(entry);
            }
        }
    }
    return OK;
}

bool TrinityMapReplace::is_entry_active(const Dictionary &entry, const Dictionary &state) const
{
    // Groups are alternatives; every element of a group must hold.
    Array groups = entry.get("activationCondition", Array());
    if (groups.is_empty())
        return entry.get("isDefaultLoad", false);
    for (int g = 0; g < groups.size(); g++)
    {
        Array elements = Dictionary(groups[g]).get("element", Array());
        bool all = true;
        for (int e = 0; e < elements.size() && all; e++)
        {
            Array params = Dictionary(elements[e]).get("param", Array());
            bool any = false;
            for (int p = 0; p < params.size() && !any; p++)
                any = condition_met(params[p], state);
            all = any;
        }
        if (all)
            return true;
    }
    return false;
}

bool TrinityMapReplace::is_scene_active(const String &scene, const Dictionary &state) const
{
    if (!scene_entries.has(scene))
        return true;
    Array list = scene_entries[scene];
    for (int i = 0; i < list.size(); i++)
        if (is_entry_active(list[i], state))
            return true;
    return false;
}
