#pragma once

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{

class GFLib : public Object
{
    GDCLASS(GFLib, Object)
    static GFLib* singleton;
    String romfs_root;

protected:
    static void _bind_methods();

public:
    GFLib();
    ~GFLib();

    static GFLib* get_singleton()
    {
        return singleton;
    }

    // Reads the gflib/romfs_root project setting; call before anything loads game files.
    void load_project_settings();

    static String romfs_path(const String& relative);

    void set_romfs_root(const String& path);

    String get_romfs_root() const
    {
        return romfs_root;
    }
};
}
