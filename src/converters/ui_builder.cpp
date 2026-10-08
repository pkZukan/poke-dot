#include "ui_builder.h"
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/hashing_context.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/math.hpp>
#include "middleware/bflyt.h"
#include "middleware/bntx.h"
#include "middleware/bfcpx.h"

using namespace godot;

namespace
{

constexpr int FLAG_VISIBLE = 1;
constexpr int FLAG_ALPHA_INFLUENCED = 2;

float origin_fraction(int value) {
    return value == 1 ? 0.0f : value == 2 ? 1.0f : 0.5f;
}
Vector2 pane_origin(int flags) {
    return Vector2(origin_fraction(flags & 3), origin_fraction((flags >> 2) & 3));
}

double effective_alpha_of(const Node *node) {
    return double(node->get_meta("effective_alpha", 1.0));
}

struct Insets {
    int left = 0, right = 0, top = 0, bottom = 0;
};

Insets read_insets(const PackedInt32Array &values, const Vector2 &size) {
    Insets r;
    const int w = int(size.x), h = int(size.y);
    if (values.size() > 0) r.left = CLAMP(values[0], 0, w);
    if (values.size() > 1) r.right = CLAMP(values[1], 0, w - r.left);
    if (values.size() > 2) r.top = CLAMP(values[2], 0, h);
    if (values.size() > 3) r.bottom = CLAMP(values[3], 0, h - r.top);
    return r;
}

PackedVector2Array default_uv() {
    PackedVector2Array uv;
    uv.push_back(Vector2(0, 0));
    uv.push_back(Vector2(1, 0));
    uv.push_back(Vector2(0, 1));
    uv.push_back(Vector2(1, 1));
    return uv;
}

}

struct UIBuilder::PaneInfo {
    enum Type { PAN, PIC, TXT, WND, BND, PRT, SCR, UNKNOWN };

    Type type = UNKNOWN;
    String type_name;
    String name;
    int parent = -1;
    Vector2 size;
    int origin = 0;
    int flags = 0;
    double alpha = 1.0;
    Vector3 translation;
    Vector3 rotation;
    Vector2 scale = Vector2(1, 1);

    static PaneInfo from(const Dictionary &pane) {
        PaneInfo info;
        info.type_name = String(pane["type"]);
        if (info.type_name == "pan1") info.type = PAN;
        else if (info.type_name == "pic1") info.type = PIC;
        else if (info.type_name == "txt1") info.type = TXT;
        else if (info.type_name == "wnd1") info.type = WND;
        else if (info.type_name == "bnd1") info.type = BND;
        else if (info.type_name == "prt1") info.type = PRT;
        else if (info.type_name == "scr1") info.type = SCR;
        info.name = String(pane["name"]);
        info.parent = int(pane["parent"]);
        info.size = pane["size"];
        info.origin = int(pane["origin"]);
        info.flags = int(pane["flags"]);
        info.alpha = double(pane["alpha"]) / 255.0;
        info.translation = pane["translation"];
        info.rotation = pane["rotation"];
        info.scale = pane["scale"];
        return info;
    }
};

void UIFontResolver::reset() {
    archive_fonts.clear();
}

Error UIFontResolver::index_archive(const Ref<SeadArchive> &archive, const PackedStringArray &files) {
    for (const String &file : files) {
        const String extension = file.get_extension();
        if (extension != "fcpx" && extension != "bfcpx") continue;
        const String name = file.get_file().get_basename() + ".bfcpx";
        ERR_FAIL_COND_V_MSG(archive_fonts.has(name), ERR_INVALID_DATA, "Ambiguous composite font name in ARC: " + name);
        PackedByteArray data = archive->get_file_data(file);
        Ref<HashingContext> hash;
        hash.instantiate();
        Error error = hash->start(HashingContext::HASH_SHA256);
        if (error != OK) return error;
        error = hash->update(data);
        if (error != OK) return error;
        Dictionary entry;
        entry["data"] = data;
        entry["resource_path"] = "res://.gflib-fonts/" + font_dir.simplify_path().sha256_text()
                + "/" + hash->finish().hex_encode() + ".bfcpx";
        archive_fonts[name] = entry;
    }
    return OK;
}

Ref<Font> UIFontResolver::get(const String &p_name, PackedStringArray &warnings) {
    String name = p_name;
    if (name.ends_with(".fcpx"))
        name = name.get_basename() + ".bfcpx";
    ResourceLoader *loader = ResourceLoader::get_singleton();
    Ref<Font> font;
    if (archive_fonts.has(name)) {
        Dictionary entry = archive_fonts[name];
        String resource_path = entry["resource_path"];
        font = loader->get_cached_ref(resource_path);
        if (font.is_null()) {
            Ref<BinaryCompositeFont> composite;
            composite.instantiate();
            Error error = composite->LoadFromBuffer(entry["data"], font_dir);
            if (error != OK) {
                for (const String &warning : composite->get_warnings()) ui_warn_once(warnings, warning);
                ui_warn_once(warnings, vformat("Could not load UI font '%s' (error %d).", p_name, error));
                return Ref<Font>();
            }
            composite->set_path(resource_path);
            font = composite;
        }
    } else {
        font = loader->load(font_dir.path_join(name).simplify_path(), "Font", ResourceLoader::CACHE_MODE_REUSE);
    }
    if (font.is_null()) {
        ui_warn_once(warnings, vformat("Could not load UI font '%s'.", p_name));
        return font;
    }
    Ref<BinaryCompositeFont> composite = font;
    if (composite.is_valid()) {
        for (const String &warning : composite->get_warnings()) ui_warn_once(warnings, warning);
    }
    return font;
}

Error UIBuilder::build(const Ref<SeadArchive> &archive, const String &arc_path, TrinityPane *&r_root) {
    r_root = nullptr;
    const PackedStringArray files = archive->get_files();

    fonts.reset();
    Error error = fonts.index_archive(archive, files);
    if (error != OK) return error;
    error = parse_layouts(archive, files, arc_path);
    if (error != OK) return error;
    error = load_textures(archive, files);
    if (error != OK) return error;

    if (!build_layout(entry_layout, nullptr, PackedStringArray())) {
        if (root) memdelete(root);
        root = nullptr;
        nodes.clear();
        return ERR_INVALID_DATA;
    }
    r_root = root;
    return OK;
}

Error UIBuilder::parse_layouts(const Ref<SeadArchive> &archive, const PackedStringArray &files, const String &arc_path) {
    PackedStringArray bflyt_files;
    for (const String &file : files) {
        if (file.ends_with(".bflyt")) bflyt_files.push_back(file);
    }
    ERR_FAIL_COND_V_MSG(bflyt_files.is_empty(), ERR_FILE_NOT_FOUND, "BFLYT layout not found in ARC");

    PackedStringArray referenced;
    String selected;
    for (const String &file : bflyt_files) {
        Ref<BinaryLayout> parsed;
        parsed.instantiate();
        Error result = parsed->LoadFromBuffer(archive->get_file_data(file));
        ERR_FAIL_COND_V_MSG(result != OK, result, "Could not parse UI layout");
        const String name = file.get_file().get_basename();
        ERR_FAIL_COND_V_MSG(layouts.has(name), ERR_INVALID_DATA, "Ambiguous UI layout name");
        layouts[name] = parsed;
        if (name == arc_path.get_file().get_basename())
            selected = name;

        const Dictionary &data = parsed->get_layout_ref();
        const Array panes = data["panes"];
        for (int j = 0; j < panes.size(); ++j) {
            const Dictionary pane = panes[j];
            if (pane.has("parts_name"))
                referenced.push_back(String(pane["parts_name"]).get_file().get_basename());
        }
    }

    if (selected.is_empty()) {
        Array names = layouts.keys();
        for (int i = 0; i < names.size(); ++i) {
            if (referenced.has(String(names[i]))) continue;
            ERR_FAIL_COND_V_MSG(!selected.is_empty(), ERR_INVALID_DATA,
                    "Multiple UI entry layouts; archive name must identify the entry layout");
            selected = names[i];
        }
    }
    ERR_FAIL_COND_V_MSG(selected.is_empty(), ERR_INVALID_DATA, "No UI entry layout found");
    entry_layout = selected;
    return OK;
}

Error UIBuilder::load_textures(const Ref<SeadArchive> &archive, const PackedStringArray &files) {
    picture_shader = ResourceLoader::get_singleton()->load("res://gflib/shaders/ui_picture.gdshader");
    ERR_FAIL_COND_V_MSG(picture_shader.is_null(), ERR_FILE_NOT_FOUND, "UI picture shader not found");

    for (const String &file : files) {
        if (!file.ends_with(".bntx")) continue;
        Ref<BinaryTextureArchive> texture_archive;
        texture_archive.instantiate();
        Error error = texture_archive->LoadFromBuffer(archive->get_file_data(file));
        if (error != OK) {
            ERR_PRINT("Failed to load BNTX archive: " + file);
            return error;
        }
        Dictionary images = texture_archive->get_textures();
        Array names = images.keys();
        for (int j = 0; j < names.size(); ++j) {
            ERR_FAIL_COND_V_MSG(textures.has(names[j]), ERR_INVALID_DATA, "Ambiguous texture name across BNTX archives");
            Ref<BinaryTexture> texture_array = images[names[j]];
            if (texture_array.is_null() || texture_array->get_layers() == 0) return ERR_FILE_CORRUPT;
            Ref<Image> image = texture_array->get_layer_image(0);
            if (image.is_null()) return ERR_FILE_CORRUPT;
            textures[names[j]] = ImageTexture::create_from_image(image);
            Ref<ShaderMaterial> shader_material;
            shader_material.instantiate();
            shader_material->set_shader(picture_shader);
            shader_material->set_shader_parameter("channel_sources", texture_array->get_channel_sources());
            texture_materials[names[j]] = shader_material;
        }
    }
    return OK;
}

bool UIBuilder::build_layout(const String &layout_name, Control *container, PackedStringArray ancestry) {
    if (!layouts.has(layout_name) || ancestry.has(layout_name) || ancestry.size() >= 32) {
        ERR_PRINT("Missing or cyclic UI part: " + layout_name);
        return false;
    }
    ancestry.push_back(layout_name);

    Ref<BinaryLayout> binary_layout = layouts[layout_name];
    const Dictionary &data = binary_layout->get_layout_ref();
    const Vector2 native_size = data["size"];
    if (native_size.x <= 0 || native_size.y <= 0) {
        ERR_PRINT("Invalid layout size in: " + layout_name);
        return false;
    }

    const bool is_entry = container == nullptr;
    if (is_entry) {
        root = memnew(TrinityPane);
        root->set_name("Layout");
        root->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
        root->set_meta("layout_size", native_size);
        root->set_meta("effective_alpha", 1.0);
        root->set_size(native_size);
        root->set_meta("bflyt", binary_layout);
        container = root;
    }

    const Array panes = data["panes"];
    const Array materials = data["materials"];
    const PackedStringArray texture_names = data["textures"];
    Vector<Control *> layout_nodes;
    Vector<Vector2> layout_sizes;
    Vector<Vector2> layout_pivots;

    for (int i = 0; i < panes.size(); ++i) {
        const Dictionary pane = panes[i];
        const PaneInfo info = PaneInfo::from(pane);
        if (info.parent < -1 || info.parent >= i) {
            ERR_PRINT("Invalid pane parent index in: " + layout_name);
            return false;
        }

        Control *parent = info.parent < 0 ? container : layout_nodes[info.parent];
        Control *node = create_pane_node(info, layout_name, parent);
        layout_nodes.push_back(node);
        layout_sizes.push_back(info.size);
        layout_pivots.push_back(info.size * pane_origin(info.origin));

        node->set_meta("bflyt_pane", pane);
        node->set_meta("layout_name", layout_name);
        if (info.parent < 0) node->set_meta("layout_instance", true);

        Vector2 anchor;
        if (info.parent < 0)
            anchor = bool(data.get("draw_from_center", true))
                    ? (is_entry ? native_size : container->get_size()) * 0.5 : Vector2();
        else
            anchor = layout_pivots[info.parent]
                    + layout_sizes[info.parent] * (pane_origin(info.origin >> 4) - Vector2(0.5, 0.5));
        apply_transform(node, info, anchor);

        const double parent_alpha = effective_alpha_of(parent);
        const double alpha = apply_alpha_and_visibility(node, info, parent_alpha);

        switch (info.type) {
            case PaneInfo::PRT:
                if (!build_part(node, pane, ancestry)) return false;
                break;
            case PaneInfo::SCR:
                node->set_clip_contents(true);
                break;
            case PaneInfo::TXT:
                build_text(static_cast<Label *>(node), info, pane, data, layout_name);
                break;
            case PaneInfo::WND:
                build_window(node, info, pane, materials, texture_names, alpha);
                break;
            case PaneInfo::PIC:
                build_picture(node, info, pane, materials, texture_names, alpha);
                break;
            case PaneInfo::PAN:
            case PaneInfo::BND:
                break;
            default:
                ui_warn_once(warnings, "Unrendered pane type: " + info.type_name);
                break;
        }
    }
    return true;
}

Control *UIBuilder::create_pane_node(const PaneInfo &info, const String &layout_name, Control *parent) {
    Control *node = info.type == PaneInfo::TXT
            ? static_cast<Control *>(memnew(Label))
            : static_cast<Control *>(memnew(TrinityPane));
    node->set_name(info.parent < 0 ? layout_name : info.name);
    node->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
    parent->add_child(node);
    nodes.push_back(node);
    return node;
}

void UIBuilder::apply_transform(Control *node, const PaneInfo &info, const Vector2 &anchor) {
    const Vector2 pivot = info.size * pane_origin(info.origin);
    node->set_size(info.size);
    node->set_pivot_offset(pivot);
    // Negate BFLYT's vertical translation and Z rotation when mapping pane transforms to Godot's 2D space.
    node->set_position(anchor + Vector2(info.translation.x, -info.translation.y) - pivot);
    node->set_scale(info.scale);
    node->set_rotation(-Math::deg_to_rad(info.rotation.z));
    if (info.rotation.x != 0 || info.rotation.y != 0) ui_warn_once(warnings, "3D pane rotations are not rendered");
}

double UIBuilder::apply_alpha_and_visibility(Control *node, const PaneInfo &info, double parent_alpha) {
    node->set_visible(info.flags & FLAG_VISIBLE);
    // Godot's modulate cascades to every descendant, unlike BFLYT's per-pane parent-alpha flag.
    // Resolve inheritance here and apply self_modulate so uninfluenced panes stay independent.
    const bool influenced = (info.flags & FLAG_ALPHA_INFLUENCED) || info.parent < 0;
    const double alpha = info.alpha * (influenced ? parent_alpha : 1.0);
    node->set_meta("effective_alpha", alpha);
    node->set_self_modulate(Color(1, 1, 1, alpha));
    return alpha;
}

bool UIBuilder::build_part(Control *node, const Dictionary &pane, const PackedStringArray &ancestry) {
    const String part_name = String(pane["parts_name"]).get_file().get_basename();
    if (!layouts.has(part_name)) {
        ui_warn_once(warnings, "Missing UI part (placeholder left empty): " + part_name);
        return true;
    }
    if (!build_layout(part_name, node, ancestry)) return false;

    Control *part_root = node->get_child_count() > 0 ? Object::cast_to<Control>(node->get_child(0)) : nullptr;
    if (part_root) {
        const Vector2 parts_scale = pane["parts_scale"];
        part_root->set_scale(part_root->get_scale() * parts_scale);
    }
    if (int(pane["parts_override_count"]) > 0) ui_warn_once(warnings, "Parts overrides are not yet applied: " + part_name);
    return true;
}

void UIBuilder::build_text(Label *label, const PaneInfo &info, const Dictionary &pane,
        const Dictionary &layout_data, const String &layout_name) {
    const Vector2 size = info.size;
    const Vector2 pivot = size * pane_origin(info.origin);

    label->set_clip_text(true);
    label->set_text(pane["text"]);

    const Vector2 font_size = pane["font_size"];
    label->add_theme_font_size_override("font_size", MAX(1, int(font_size.y)));
    label->add_theme_color_override("font_color", pane["text_color"]);

    const int align = pane["text_alignment"];
    const int horizontal = align & 3, vertical = (align >> 2) & 3;
    label->set_horizontal_alignment(HorizontalAlignment(horizontal == 0 ? 1 : horizontal == 1 ? 0 : 2));
    label->set_vertical_alignment(VerticalAlignment(vertical == 0 ? 1 : vertical == 1 ? 0 : 2));

    const PackedStringArray font_list = layout_data["fonts"];
    const int font_index = pane["font_index"];
    if (font_index < 0 || font_index >= font_list.size()) {
        ui_warn_once(warnings, vformat("Invalid font index %d in layout '%s', pane '%s'.",
                font_index, layout_name, info.name));
        return;
    }
    Ref<Font> font = fonts.get(font_list[font_index], warnings);
    if (font.is_null()) return;
    label->add_theme_font_override("font", font);

    // BFLYT specifies independent glyph width and height; Godot uses a uniform font size.
    // Scale the label geometry to preserve the authored text proportions and pane bounds.
    if (font_size.x > 0 && font_size.y > 0 && Math::is_finite(font_size.x) && Math::is_finite(font_size.y)) {
        const Vector2 nominal = font->get_meta("nominal_font_size", Vector2(1, 1));
        const real_t native_aspect = nominal.x > 0 && nominal.y > 0 ? nominal.y / nominal.x : 1;
        const real_t text_scale = font_size.x / font_size.y * native_aspect;
        const Vector2 text_pivot(pivot.x / text_scale, pivot.y);
        label->set_meta("text_scale_x", text_scale);
        label->set_size(Vector2(size.x / text_scale, size.y));
        label->set_pivot_offset(text_pivot);
        label->set_position(label->get_position() + pivot - text_pivot);
        label->set_scale(label->get_scale() * Vector2(text_scale, 1));
    }
}

void UIBuilder::build_picture(Control *node, const PaneInfo &info, const Dictionary &pane,
        const Array &materials, const PackedStringArray &texture_names, double alpha) {
    Dictionary material;
    if (!get_material(materials, int(pane["material_index"]), material)) return;

    Array uv_sets = pane["uv_sets"];
    PackedVector2Array uv;
    if (!uv_sets.is_empty()) uv = uv_sets[0];
    const PackedColorArray colors = pane["colors"];

    add_quad(node, "Picture", Rect2(Vector2(), info.size), material, texture_names, uv, colors, 0, alpha);
}

void UIBuilder::build_window(Control *node, const PaneInfo &info, const Dictionary &pane,
        const Array &materials, const PackedStringArray &texture_names, double alpha) {
    const Vector2 size = info.size;
    const Insets stretch = read_insets(pane["window_stretch"], size);
    const Insets fallback = read_insets(pane["window_custom_insets"], size);
    const Array frames = pane["window_frames"];
    const int count = frames.size();
    const int flags = pane["window_flags"];
    const int kind = (flags >> 2) & 3;
    const bool use_one_material = flags & 1;
    const bool use_vertex_colors = flags & 2;
    const bool draw_content = !(flags & 0x10) && kind != 2;
    const PackedColorArray colors = pane["colors"];

    // The custom frame sizes are fallbacks for frames without textures.
    // Stretch values expand the content into the border; they are not border sizes.
    auto frame_size = [&](int index) -> Vector2 {
        if (index >= count) return Vector2();
        const Dictionary frame = frames[index];
        const Dictionary material_frame = frames[use_one_material ? 0 : index];
        const Dictionary material = materials[int(material_frame["material_index"])];
        const Array maps = material["texture_maps"];
        if (maps.is_empty()) return Vector2();
        const Dictionary map = maps[0];
        const String name = texture_names[int(map["texture_index"])];
        if (!textures.has(name)) return Vector2();
        const Ref<Texture2D> texture = textures[name];
        Vector2 dimensions = texture->get_size();
        const int flip = frame["texture_flip"];
        if (flip == 3 || flip == 5) dimensions = Vector2(dimensions.y, dimensions.x);
        return dimensions;
    };
    const Vector2 first_size = frame_size(0);
    const Vector2 last_size = frame_size(count >= 4 ? 3 : count == 2 ? 1 : 0);
    const real_t left = CLAMP(first_size.x > 0 ? first_size.x : real_t(fallback.left), real_t(0), size.x);
    const real_t right = CLAMP(last_size.x > 0 ? last_size.x : real_t(fallback.right), real_t(0), size.x - left);
    const real_t top = CLAMP(first_size.y > 0 ? first_size.y : real_t(fallback.top), real_t(0), size.y);
    const real_t bottom = CLAMP(last_size.y > 0 ? last_size.y : real_t(fallback.bottom), real_t(0), size.y - top);
    const real_t inner_width = size.x - left - right;
    const real_t inner_height = size.y - top - bottom;

    Dictionary content_material;
    if (draw_content && get_material(materials, int(pane["material_index"]), content_material)) {
        const Array uv_sets = pane["uv_sets"];
        PackedVector2Array uv;
        if (!uv_sets.is_empty()) uv = uv_sets[0];
        const bool horizontal = kind == 1;
        add_quad(node, "WindowContent",
                Rect2(Vector2(left - stretch.left, horizontal ? 0 : top - stretch.top),
                        Vector2(inner_width + stretch.left + stretch.right,
                                horizontal ? size.y : inner_height + stretch.top + stretch.bottom)),
                content_material, texture_names, uv, colors, 0, alpha);
    }

    // Each frame owns one region. UV extents repeat edge textures at their
    // authored pixel size rather than stretching the entire window texture.
    auto draw_frame = [&](int index, const Rect2 &rect, const Vector2 &uv_start,
            const Vector2 &uv_end) {
        if (rect.size.x <= 0 || rect.size.y <= 0) return;
        const Dictionary frame = frames[index];
        const Dictionary material_frame = frames[use_one_material ? 0 : index];
        Dictionary material;
        if (!get_material(materials, int(material_frame["material_index"]), material)) return;
        PackedVector2Array uv;
        uv.push_back(uv_start);
        uv.push_back(Vector2(uv_end.x, uv_start.y));
        uv.push_back(Vector2(uv_start.x, uv_end.y));
        uv.push_back(uv_end);
        PackedColorArray frame_colors;
        if (use_vertex_colors && colors.size() == 4) {
            const Vector2 corners[4] = {rect.position, rect.position + Vector2(rect.size.x, 0),
                    rect.position + Vector2(0, rect.size.y), rect.position + rect.size};
            for (const Vector2 &corner : corners) {
                const real_t x = size.x > 0 ? corner.x / size.x : 0;
                const real_t y = size.y > 0 ? corner.y / size.y : 0;
                frame_colors.push_back(colors[0].lerp(colors[1], x).lerp(colors[2].lerp(colors[3], x), y));
            }
        }
        add_quad(node, vformat("WindowFrame%d", index), rect, material, texture_names,
                uv, frame_colors, int(frame["texture_flip"]), alpha);
    };
    auto ratio = [](real_t length, real_t tile) { return tile > 0 ? length / tile : real_t(1); };

    if (count == 0) return;
    if (kind == 1 || kind == 2) {
        if (count != 1 && count != 2) {
            ui_warn_once(warnings, "Horizontal window requires one or two frames");
            return;
        }
        if (kind == 1) {
            draw_frame(0, Rect2(0, 0, left, size.y), Vector2(), Vector2(1, 1));
            draw_frame(count - 1, Rect2(size.x - right, 0, right, size.y),
                    Vector2(count == 1 ? 1 : 0, 0), Vector2(count == 1 ? 0 : 1, 1));
        } else {
            // No-content windows use two overlapping horizontal strips.
            draw_frame(0, Rect2(left, 0, size.x - left, size.y),
                    Vector2(ratio(size.x - left, right), 0), Vector2(0, 1));
            draw_frame(count - 1, Rect2(0, 0, size.x - right, size.y),
                    Vector2(), Vector2(ratio(size.x - right, left), 1));
        }
    } else if (kind == 0 && (count == 1 || count == 4)) {
        // Four border strips, each including one corner.
        draw_frame(0, Rect2(0, 0, size.x - right, top),
                Vector2(), Vector2(ratio(size.x - right, left), 1));
        draw_frame(count == 1 ? 0 : 1, Rect2(size.x - right, 0, right, size.y - bottom),
                Vector2(count == 1 ? 1 : 0, 0), Vector2(count == 1 ? 0 : 1, ratio(size.y - bottom, top)));
        draw_frame(count == 1 ? 0 : 2, Rect2(0, top, left, size.y - top),
                Vector2(0, count == 1 ? ratio(size.y - top, bottom) : 1 - ratio(size.y - top, bottom)),
                Vector2(1, count == 1 ? 0 : 1));
        draw_frame(count == 1 ? 0 : 3, Rect2(left, size.y - bottom, size.x - left, bottom),
                Vector2(count == 1 ? ratio(size.x - left, right) : 1 - ratio(size.x - left, right), count == 1 ? 1 : 0),
                Vector2(count == 1 ? 0 : 1, count == 1 ? 0 : 1));
    } else if (kind == 0 && count == 8) {
        draw_frame(0, Rect2(0, 0, left, top), Vector2(), Vector2(1, 1));
        draw_frame(1, Rect2(size.x - right, 0, right, top), Vector2(), Vector2(1, 1));
        draw_frame(2, Rect2(0, size.y - bottom, left, bottom), Vector2(), Vector2(1, 1));
        draw_frame(3, Rect2(size.x - right, size.y - bottom, right, bottom), Vector2(), Vector2(1, 1));
        draw_frame(4, Rect2(0, top, left, inner_height),
                Vector2(0, 1 - ratio(inner_height, top)), Vector2(1, 1));
        draw_frame(5, Rect2(size.x - right, top, right, inner_height),
                Vector2(), Vector2(1, ratio(inner_height, bottom)));
        draw_frame(6, Rect2(left, 0, inner_width, top),
                Vector2(), Vector2(ratio(inner_width, left), 1));
        draw_frame(7, Rect2(left, size.y - bottom, inner_width, bottom),
                Vector2(1 - ratio(inner_width, right), 0), Vector2(1, 1));
    } else {
        ui_warn_once(warnings, "Unsupported window kind or frame count");
    }
}

bool UIBuilder::get_material(const Array &materials, int index, Dictionary &r_material) {
    if (index < 0 || index >= materials.size()) {
        ui_warn_once(warnings, vformat("Invalid material index %d.", index));
        return false;
    }
    r_material = materials[index];
    return true;
}

Polygon2D *UIBuilder::add_quad(Control *parent, const String &name, const Rect2 &rect, const Dictionary &material,
        const PackedStringArray &texture_names, PackedVector2Array uv, const PackedColorArray &colors,
        int texture_flip, double alpha) {
    // Reorder BFLYT's TL, TR, BL, BR corners to the polygon winding expected by Godot.
    static const int order[4] = { 0, 1, 3, 2 };
    const Vector2 corners[4] = {
        rect.position,
        rect.position + Vector2(rect.size.x, 0),
        rect.position + Vector2(0, rect.size.y),
        rect.position + rect.size
    };
    PackedVector2Array vertices;
    for (int k : order) vertices.push_back(corners[k]);

    Polygon2D *quad = memnew(Polygon2D);
    quad->set_name(name);
    quad->set_polygon(vertices);

    const Array maps = material["texture_maps"];
    if (maps.is_empty()) {
        const Color white = material["white_color"];
        PackedColorArray vertex_colors;
        for (int k : order) vertex_colors.push_back(colors.size() == 4 ? white * colors[k] : white);
        quad->set_vertex_colors(vertex_colors);
    } else {
        const Dictionary map = maps[0];
        const int texture_index = int(map["texture_index"]);
        if (texture_index < 0 || texture_index >= texture_names.size()) {
            ui_warn_once(warnings, vformat("Invalid texture index %d.", texture_index));
            memdelete(quad);
            return nullptr;
        }
        const String texture_name = texture_names[texture_index];
        if (!textures.has(texture_name)) {
            ui_warn_once(warnings, "Missing texture: " + texture_name);
            memdelete(quad);
            return nullptr;
        }
        if (maps.size() > 1)
            ui_warn_once(warnings, "Additional UI material texture maps are not converted");
        if (uv.size() != 4) {
            ui_warn_once(warnings, "Picture without usable UV coordinates; using the full texture");
            uv = default_uv();
        }

        Ref<Texture2D> texture = textures[texture_name];
        PackedVector2Array quad_uv;
        PackedColorArray vertex_colors;
        // Window texture orientation is an enum, not a bitmask. Transform UV
        // values around (0.5, 0.5), including coordinates outside the unit tile.
        if (texture_flip < 0 || texture_flip >= 6) {
            ui_warn_once(warnings, "Unknown window texture orientation uses no rotation");
            texture_flip = 0;
        }
        for (int k : order) {
            Vector2 coord = uv[k];
            switch (texture_flip) {
                case 1: coord.x = 1 - coord.x; break;
                case 2: coord.y = 1 - coord.y; break;
                case 3: coord = Vector2(coord.y, 1 - coord.x); break;
                case 4: coord = Vector2(1 - coord.x, 1 - coord.y); break;
                case 5: coord = Vector2(1 - coord.y, coord.x); break;
            }
            quad_uv.push_back(coord * texture->get_size());
            vertex_colors.push_back(colors.size() == 4 ? colors[k] : Color(1, 1, 1, 1));
        }
        quad->set_uv(quad_uv);
        quad->set_vertex_colors(vertex_colors);
        quad->set_texture(texture);
        quad->set_material(make_picture_material(texture_name, material));
        apply_sampler(quad, map);
    }

    if (material.has("name")) quad->set_meta("material_name", material["name"]);
    quad->set_self_modulate(Color(1, 1, 1, alpha));
    parent->add_child(quad);
    return quad;
}

Ref<ShaderMaterial> UIBuilder::make_picture_material(const String &texture_name, const Dictionary &material)
{
    Ref<ShaderMaterial> source_material = texture_materials[texture_name];
    ERR_FAIL_COND_V_MSG(source_material.is_null(), Ref<ShaderMaterial>(), vformat("Missing picture material for texture: %s", texture_name));

    Ref<ShaderMaterial> pane_material = source_material->duplicate();
    pane_material->set_local_to_scene(true);
    pane_material->set_shader_parameter("black_color", material.get("black_color", Color(0.0, 0.0, 0.0, 0.0)));
    pane_material->set_shader_parameter("white_color", material.get("white_color", Color(1.0, 1.0, 1.0, 1.0)));

    const Dictionary blend_mode = material.get("color_blend_mode", Dictionary());

    const bool additive_blend = 
        !blend_mode.is_empty() &&
        int(blend_mode.get("equation", 0)) == 1 &&
        int(blend_mode.get("source", 0)) == 4 &&
        int(blend_mode.get("destination", 0)) == 1 &&
        int(blend_mode.get("logic_operation", 0)) == 0;

    // xy = translation
    // z  = rotation in radians
    // w  = additive blend flag
    Vector4 picture_params(0.0, 0.0, 0.0, additive_blend ? 1.0 : 0.0);
    Vector2 uv_scale(1.0, 1.0);

    const Array transforms = material.get("texture_transforms", Array());
    if (!transforms.is_empty()) {
        const Dictionary transform = transforms[0];
        const Vector2 translation = transform.get("translation", Vector2(0.0, 0.0));
        uv_scale = transform.get("scale", Vector2(1.0, 1.0));
        const double rotation = transform.get("rotation", 0.0);

        picture_params.x = translation.x;
        picture_params.y = translation.y;
        picture_params.z = Math::deg_to_rad(rotation);
    }

    pane_material->set_shader_parameter("picture_params", picture_params);
    pane_material->set_shader_parameter("uv_scale", uv_scale);

    return pane_material;
}

void UIBuilder::apply_sampler(Polygon2D *quad, const Dictionary &map) {
    const int flags_s = map["wrap_s"], flags_t = map["wrap_t"];
    const int wrap_s = flags_s & 3, wrap_t = flags_t & 3;
    const int min_filter = (flags_s >> 2) & 3, mag_filter = (flags_t >> 2) & 3;
    // CanvasItem has one filter setting for both minification and magnification.
    quad->set_texture_filter(mag_filter == 0 ? CanvasItem::TEXTURE_FILTER_NEAREST : CanvasItem::TEXTURE_FILTER_LINEAR);
    if (min_filter != mag_filter)
        ui_warn_once(warnings, "Different UI min/mag filters use the magnification filter");
    if (wrap_s == wrap_t && wrap_s == 1) {
        quad->set_texture_repeat(CanvasItem::TEXTURE_REPEAT_ENABLED);
    } else if (wrap_s == wrap_t && wrap_s == 2) {
        quad->set_texture_repeat(CanvasItem::TEXTURE_REPEAT_MIRROR);
    } else {
        quad->set_texture_repeat(CanvasItem::TEXTURE_REPEAT_DISABLED);
        if (wrap_s != wrap_t || wrap_s == 3)
            ui_warn_once(warnings, "Unsupported sampler wrap combination uses clamp");
    }
}
