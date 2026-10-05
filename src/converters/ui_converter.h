#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/text_server.hpp>
#include <godot_cpp/classes/font_file.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/label.hpp>
#include "middleware/bflyt.h"
#include "middleware/bntx.h"
#include "middleware/bfcpx.h"
#include "middleware/sarc.h"
#include "ui/ui_state_runtime.h"

namespace godot 
{

class TrinityUI : public Control {
    GDCLASS(TrinityUI, Control)

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    // Bare names must be unique; paths are relative to this scope.
    Control *get_pane(const String &name_or_path) const;
    TrinityUI *get_scope(const String &name_or_path) const;
    Error _set_text(const String &name_or_path, const String &text);

    // Root-only API.
    // Attaches the layout to parent_path (relative to this node). Returns OK on success.
    Error load_ui(const String &truiv_path, const String &arc_path, const NodePath &parent_path);
    Ref<Font> get_font(const String &p_name);
    Error apply_state(const String &component, const String &state, double frame = 0.0);
    PackedStringArray get_warnings() const;

private:
    // Pane nodes and the entry layout node carry these metas; the UI root does not.
    bool is_scope_node() const;
    Control *scope_root() const;
    UIStateRuntime state_runtime;
    void fit_layout();
    bool build_layout(const String &layout_name, Control *container, PackedStringArray ancestry);
    static float origin_fraction(int value);
    static Vector2 pane_origin(int flags);
    static void warn_once(PackedStringArray &warnings, const String &message);

    String font_dir = "res://Assets/ui/font/bin/";
    Dictionary font_cache;
    Dictionary archive_fonts;
    Dictionary layouts;
    
    Control *layout = nullptr;
    PackedStringArray warnings;
    Vector<Control *> nodes;
    Dictionary textures;
    Dictionary texture_materials;
};

} // namespace godot