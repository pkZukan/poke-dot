#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/math.hpp>
#include <vector>
#include "middleware/bflyt.h"
#include "middleware/bntx.h"
#include "middleware/sarc.h"

namespace godot {

class TrinityUI : public Control {
    GDCLASS(TrinityUI, Control)

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    Error load_ui(const String &truiv_path, const String &arc_path, const String &layout_file = "");
    Control *get_pane(const String &name) const;
    PackedStringArray get_warnings() const;

private:
    void fit_layout();
    static float origin_fraction(int value);
    static Vector2 pane_origin(int flags);
    static void warn_once(PackedStringArray &warnings, const String &message);

};

} // namespace godot