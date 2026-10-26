#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{

// The map replace table (map_replace_data_array.bin): which optional scenes
// load for a given story state. Scenes that the table does not mention
// always load.
// A story state is a Dictionary; missing keys mean a new game:
//   phase: int                     main story progress
//   flags: Dictionary or Array      flag name -> bool, or the set flag names
//   works: Dictionary               work name -> int
//   player_sex: int, another_dimension_rank: int, noon: bool, birthday: bool
class TrinityMapReplace : public RefCounted
{
    GDCLASS(TrinityMapReplace, RefCounted)
    Array entries;
    Dictionary scene_entries;

protected:
    static void _bind_methods();

public:
    static String default_path();

    Error load(const String& path);

    Array get_entries() const
    {
        return entries;
    }

    bool has_scene(const String& scene) const
    {
        return scene_entries.has(scene);
    }

    bool is_entry_active(const Dictionary& entry, const Dictionary& state) const;
    bool is_scene_active(const String& scene, const Dictionary& state) const;
};
}
