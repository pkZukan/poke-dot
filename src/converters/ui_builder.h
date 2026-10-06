#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/polygon2d.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include "middleware/sarc.h"
#include "ui_pane.h"

namespace godot
{

inline void ui_warn_once(PackedStringArray &warnings, const String &message) {
    if (!warnings.has(message)) warnings.push_back(message);
}

class UIFontCache {
public:
    String font_dir = "res://Assets/ui/font/bin/";

    void reset();
    Error index_archive(const Ref<SeadArchive> &archive, const PackedStringArray &files);
    Ref<Font> get(const String &name, PackedStringArray &warnings);

private:
    Dictionary cache;
    Dictionary archive_fonts;
};

class UIBuilder {
public:
    explicit UIBuilder(UIFontCache &p_fonts) : fonts(p_fonts) {}

    Error build(const Ref<SeadArchive> &archive, const String &arc_path, TrinityPane *&r_root);

    Dictionary textures;
    Dictionary texture_materials;
    PackedStringArray warnings;
    Vector<Control *> nodes;
    String entry_layout;

private:
    struct PaneInfo;

    Error parse_layouts(const Ref<SeadArchive> &archive, const PackedStringArray &files, const String &arc_path);
    Error load_textures(const Ref<SeadArchive> &archive, const PackedStringArray &files);
    bool build_layout(const String &layout_name, Control *container, PackedStringArray ancestry);

    Control *create_pane_node(const PaneInfo &info, const String &layout_name, Control *parent);
    void apply_transform(Control *node, const PaneInfo &info, const Vector2 &anchor);
    double apply_alpha_and_visibility(Control *node, const PaneInfo &info, double parent_alpha);

    bool build_part(Control *node, const Dictionary &pane, const PackedStringArray &ancestry);
    void build_text(Label *label, const PaneInfo &info, const Dictionary &pane,
            const Dictionary &layout_data, const String &layout_name);
    void build_window(Control *node, const PaneInfo &info, const Dictionary &pane,
            const Array &materials, const PackedStringArray &texture_names, double alpha);
    void build_picture(Control *node, const PaneInfo &info, const Dictionary &pane,
            const Array &materials, const PackedStringArray &texture_names, double alpha);

    Polygon2D *add_quad(Control *parent, const String &name, const Rect2 &rect, Dictionary material,
            const PackedStringArray &texture_names, PackedVector2Array uv, const PackedColorArray &colors,
            int texture_flip, double alpha);
    Ref<ShaderMaterial> make_picture_material(const String &texture_name, const Dictionary &material);
    void apply_sampler(Polygon2D *quad, const Dictionary &map);
    bool get_material(const Array &materials, int index, Dictionary &r_material);

    UIFontCache &fonts;
    Ref<Shader> picture_shader;
    Dictionary layouts;
    TrinityPane *root = nullptr;
};

}
