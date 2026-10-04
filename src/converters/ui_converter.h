#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/text_server.hpp>
#include <godot_cpp/classes/font_file.hpp>
#include <godot_cpp/core/math.hpp>
#include <vector>
#include "middleware/bflyt.h"
#include "middleware/bntx.h"
#include "middleware/bfcpx.h"
#include "middleware/sarc.h"
#include "ui/ui_state_runtime.h"

namespace godot {

class TrinityUI : public Control {
    GDCLASS(TrinityUI, Control)

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    // Attaches the layout to parent_path (relative to this node). Returns OK on success.
    Error load_ui(const String &truiv_path, const String &arc_path, const NodePath &parent_path);
    Ref<Font> get_font(const String &p_name);
    Error apply_state(const String &component, const String &state, double frame = 0.0);
    Control *get_pane(const String &name) const;
    PackedStringArray get_warnings() const;

private:
    UIStateRuntime state_runtime;
    void fit_layout();
    static float origin_fraction(int value);
    static Vector2 pane_origin(int flags);
    static void warn_once(PackedStringArray &warnings, const String &message);

    String font_dir = "res://Assets/ui/font/bin/";
    HashMap<String, Ref<Font>> font_cache;
};

} // namespace godot