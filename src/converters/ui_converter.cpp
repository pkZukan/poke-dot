#include "ui_converter.h"
#include "ui_fbs/truiv.h"
#include <godot_cpp/classes/label.hpp>
#include <functional>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>


using namespace godot;

float TrinityUI::origin_fraction(int value) {
    return value == 1 ? 0.0f : value == 2 ? 1.0f : 0.5f;
}
Vector2 TrinityUI::pane_origin(int flags) {
    return Vector2(origin_fraction(flags & 3), origin_fraction((flags >> 2) & 3));
}
void TrinityUI::warn_once(PackedStringArray &warnings, const String &message) {
    if (!warnings.has(message)) warnings.push_back(message);
}

void TrinityUI::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_ui", "truiv_path", "arc_path", "parent_path"), &TrinityUI::load_ui);
    ClassDB::bind_method(D_METHOD("apply_state", "component", "state", "frame"), &TrinityUI::apply_state, DEFVAL(0.0));
    ClassDB::bind_method(D_METHOD("get_warnings"), &TrinityUI::get_warnings);
}

void TrinityUI::_notification(int what) {
    if (what == NOTIFICATION_RESIZED || what == NOTIFICATION_READY) fit_layout();
}

void TrinityUI::fit_layout() {
    Control *layout = Object::cast_to<Control>(get_node_or_null(get_meta("layout_path", NodePath("Layout"))));
    if (!layout || !layout->has_meta("layout_size")) return;
    const Vector2 native_size = layout->get_meta("layout_size");
    if (native_size.x <= 0 || native_size.y <= 0) return;
    Vector2 available = get_size();
    if (available.x <= 0 || available.y <= 0) available = native_size;
    const real_t factor = MIN(available.x / native_size.x, available.y / native_size.y);
    layout->set_scale(Vector2(factor, factor));
    layout->set_position((available - native_size * factor) * 0.5);
}

Control *TrinityUI::scope_root() const {
    return Object::cast_to<Control>(get_node_or_null(get_meta("layout_path", NodePath("Layout"))));
}

Error TrinityUI::apply_state(const String &component, const String &state, double frame) {
    Control *layout = Object::cast_to<Control>(get_node_or_null(get_meta("layout_path", NodePath("Layout"))));
    ERR_FAIL_NULL_V(layout, ERR_UNCONFIGURED);
    return state_runtime.apply(this, layout, component, state, frame);
}

PackedStringArray TrinityUI::get_warnings() const {
    return get_meta("conversion_warnings", PackedStringArray());
}

Ref<Font> TrinityUI::get_font(const String &p_name) 
{
    // The layout calls composite fonts .fcpx, while loose files use .bfcpx.
    String name = p_name;
    if (name.ends_with(".fcpx")) name = name.get_basename() + ".bfcpx";
    if (font_cache.has(name)) return font_cache[name];

    Ref<Font> font;
    Error error;
    if (name.get_extension() == "bfcpx") {
        Ref<BinaryCompositeFont> composite;
        composite.instantiate();
        // Archive composites describe the layout's font; their members live in
        // the shared font directory, just as they do for loose composites.
        if (archive_fonts.has(name))
            error = composite->LoadFromBuffer(archive_fonts[name], font_dir);
        else
            error = composite->LoadFromFile(font_dir.path_join(name));
        if (error == OK) font = composite;
        for (const String &warning : composite->get_warnings()) warn_once(warnings, warning);
    } else {
        Ref<BinaryFont> bitmap;
        bitmap.instantiate();
        error = bitmap->LoadFromFile(font_dir.path_join(name));
        if (error == OK) font = bitmap;
    }
    if (error != OK) warn_once(warnings, vformat("Could not load UI font '%s' (error %d).", p_name, error));
    font_cache[name] = font;
    return font;
}

bool TrinityUI::build_layout(const String &layout_name, Control *container, PackedStringArray ancestry)
{
    if (!layouts.has(layout_name) || ancestry.has(layout_name) || ancestry.size() >= 32) 
    {
        ERR_PRINT("Missing or cyclic UI part: " + layout_name);
        return false;
    }
    ancestry.push_back(layout_name);

    Ref<BinaryLayout> binary_layout = layouts[layout_name];
    Dictionary data = binary_layout->get_layout();
    Vector2 native_size = data["size"];
    if (native_size.x <= 0 || native_size.y <= 0) return false;

    const bool is_entry = container == nullptr;
    if (is_entry) 
    {
        layout = memnew(TrinityPane);
        layout->set_name("Layout");
        layout->set_mouse_filter(MOUSE_FILTER_IGNORE);
        layout->set_size(native_size);
        layout->set_meta("layout_size", native_size);
        layout->set_meta("bflyt", binary_layout);
        container = layout;
    }

    Array panes = data["panes"];
    Array materials = data["materials"];
    PackedStringArray texture_names = data["textures"];
    Vector<Control *> layout_nodes;

    for (int i = 0; i < panes.size(); ++i) 
    {
        Dictionary pane = panes[i];
        int parent_index = pane["parent"];
        if (parent_index < -1 || parent_index >= i) 
        {
            ERR_PRINT("Invalid pane parent index in: " + layout_name);
            return false;
        }
        Control *parent = parent_index < 0 ? container : layout_nodes[parent_index];
        Control *node = String(pane["type"]) == "txt1" ? static_cast<Control *>(memnew(Label)) : memnew(TrinityPane);
        node->set_name(parent_index < 0 ? layout_name : String(pane["name"]));
        node->set_mouse_filter(MOUSE_FILTER_IGNORE);
        parent->add_child(node);
        layout_nodes.push_back(node);
        nodes.push_back(node);
        node->set_meta("bflyt_pane", pane);
        node->set_meta("layout_name", layout_name);
        if (parent_index < 0) node->set_meta("layout_instance", true);
        const Vector2 size = pane["size"];
        const int origin = pane["origin"];
        const Vector2 pivot = size * pane_origin(origin);
        const Vector3 translation = pane["translation"];
        const Vector3 rotation = pane["rotation"];
        Vector2 base;
        if (parent_index < 0) 
        {
            if (is_entry) 
                base = native_size * 0.5;
            else 
                base = container->get_size() * 0.5;
        } 
        else 
        {
            base = parent->get_pivot_offset()
                + parent->get_size() * (pane_origin(origin >> 4) - Vector2(0.5, 0.5));
        }
        node->set_size(size);
        node->set_pivot_offset(pivot);
        node->set_position(base + Vector2(translation.x, -translation.y) - pivot);
        node->set_scale(pane["scale"]);
        node->set_rotation(-Math::deg_to_rad(rotation.z));
        if (rotation.x != 0 || rotation.y != 0) warn_once(warnings, "3D pane rotations are not rendered");
        const int flags = pane["flags"];
        node->set_visible(flags & 1);
        const Color alpha(1, 1, 1, double(pane["alpha"]) / 255.0);
        if ((flags & 2) || String(pane["type"]) == "prt1") node->set_modulate(alpha);
        else node->set_self_modulate(alpha);
        const String type = pane["type"];
        if (type == "prt1") {
            String part_name = String(pane["parts_name"]).get_file().get_basename();
            if (!build_layout(part_name, node, ancestry)) return false;
            Control *part_root = Object::cast_to<Control>(node->get_child(0));
            part_root->set_scale(part_root->get_scale() * Vector2(pane["parts_scale"]));
            if (int(pane["parts_override_count"]) > 0) warn_once(warnings, "Parts overrides are not yet applied: " + part_name);
            continue;
        }
        if (type == "scr1") node->set_clip_contents(true);
        if (type == "txt1") 
        {
            Label *label = Object::cast_to<Label>(node);
            if (!label) continue;

            label->set_clip_text(true);
            label->set_text(pane["text"]);

            Vector2 font_size = pane["font_size"];
            label->add_theme_font_size_override("font_size", MAX(1, int(font_size.y)));
            label->add_theme_color_override("font_color", pane["text_color"]);

            int align = pane["text_alignment"];
            int horizontal = align & 3, vertical = (align >> 2) & 3;
            label->set_horizontal_alignment(HorizontalAlignment(horizontal == 0 ? 1 : horizontal == 1 ? 0 : 2));
            label->set_vertical_alignment(VerticalAlignment(vertical == 0 ? 1 : vertical == 1 ? 0 : 2));
            PackedStringArray font_list = data["fonts"];
            const int font_index = pane["font_index"];
            if (font_index < 0 || font_index >= font_list.size()) {
                warn_once(warnings, vformat("Invalid font index %d in layout '%s', pane '%s'.", font_index, layout_name, pane["name"]));
            } else {
                Ref<Font> font = get_font(font_list[font_index]);
                if (font.is_valid()) {
                    label->add_theme_font_override("font", font);
                    // BFLYT specifies independent glyph width and height. Godot's font
                    // size is uniform, so compensate the label's local geometry before
                    // applying horizontal text scaling. Its visual bounds and pivot stay
                    // at the authored pane bounds (including alignment and clipping).
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
            }
            continue;
        }
        if (type != "pic1") 
        {
            if (type != "pan1" && type != "bnd1" && type != "scr1") warn_once(warnings, "Unrendered pane type: " + type);
            continue;
        }
        Dictionary material = materials[int(pane["material_index"])];
        Array maps = material["texture_maps"];
        if (maps.is_empty()) {
            Polygon2D *solid = memnew(Polygon2D);
            solid->set_name("Picture");
            PackedVector2Array corners;
            corners.push_back(Vector2()); corners.push_back(Vector2(size.x, 0));
            corners.push_back(size); corners.push_back(Vector2(0, size.y));
            solid->set_polygon(corners);
            solid->set_color(material["white_color"]);
            if (!(flags & 2)) solid->set_self_modulate(alpha);
            node->add_child(solid);
            continue;
        }
        Dictionary map = maps[0];
        String texture_name = texture_names[int(map["texture_index"])];
        if (!textures.has(texture_name)) {
            warn_once(warnings, "Missing texture: " + texture_name);
            continue;
        }
        if (maps.size() > 1) warn_once(warnings, "Multi-texture materials render their first texture only; game shaders are not converted");
        Array uv_sets = pane["uv_sets"];
        if (uv_sets.is_empty()) {
            warn_once(warnings, "Picture without UV coordinates: " + String(pane["name"]));
            continue;
        }
        Ref<Texture2D> texture = textures[texture_name];
        PackedVector2Array source_uv = uv_sets[0];
        PackedColorArray source_colors = pane["colors"];
        const int order[] = {0, 1, 3, 2};
        const Vector2 corners[] = {Vector2(), Vector2(size.x, 0), Vector2(0, size.y), size};
        PackedVector2Array vertices, uv;
        PackedColorArray colors;
        for (int k : order) {
            vertices.push_back(corners[k]);
            uv.push_back(source_uv[k] * texture->get_size());
            colors.push_back(source_colors[k]);
        }
        Polygon2D *picture = memnew(Polygon2D);
        picture->set_name("Picture");
        picture->set_polygon(vertices);
        picture->set_uv(uv);
        picture->set_vertex_colors(colors);
        picture->set_texture(texture);
        Ref<ShaderMaterial> source_material = texture_materials[texture_name];
        Ref<ShaderMaterial> pane_material = source_material->duplicate();
        pane_material->set_local_to_scene(true);
        pane_material->set_shader_parameter("black_color", material["black_color"]);
        pane_material->set_shader_parameter("white_color", material["white_color"]);
        Array transforms = material["texture_transforms"];
        if (!transforms.is_empty()) {
            Dictionary transform = transforms[0];
            pane_material->set_shader_parameter("uv_translation", transform["translation"]);
            pane_material->set_shader_parameter("uv_scale", transform["scale"]);
            pane_material->set_shader_parameter("uv_rotation", Math::deg_to_rad(double(transform["rotation"])));
        }
        picture->set_material(pane_material);
        picture->set_meta("material_name", material["name"]);
        if (!(flags & 2)) picture->set_self_modulate(alpha);
        int wrap_s = map["wrap_s"], wrap_t = map["wrap_t"];
        picture->set_texture_filter(wrap_s < 3 && wrap_t < 3 ? TEXTURE_FILTER_NEAREST : TEXTURE_FILTER_LINEAR);
        if (wrap_s == wrap_t && (wrap_s == 1 || wrap_s == 5)) picture->set_texture_repeat(TEXTURE_REPEAT_ENABLED);
        else if (wrap_s == wrap_t && (wrap_s == 2 || wrap_s == 6)) picture->set_texture_repeat(TEXTURE_REPEAT_MIRROR);
        else {
            picture->set_texture_repeat(TEXTURE_REPEAT_DISABLED);
            if (wrap_s != wrap_t || wrap_s == 3 || wrap_s == 7) warn_once(warnings, "Unsupported sampler wrap combination uses clamp");
        }
        node->add_child(picture);
    }
    return true;
}

Error TrinityUI::load_ui(const String &truiv_path, const String &arc_path, const NodePath &parent_path) 
{
    Node *layout_parent = get_node_or_null(parent_path);

    ERR_FAIL_NULL_V_MSG(layout_parent, ERR_DOES_NOT_EXIST, "UI parent path does not exist");
    ERR_FAIL_COND_V_MSG(!FileAccess::file_exists(truiv_path) || !FileAccess::file_exists(arc_path), ERR_FILE_NOT_FOUND, "UI input file does not exist");

    Ref<TRUIV> view = ResourceLoader::get_singleton()->load(truiv_path);
    Ref<SeadArchive> archive = ResourceLoader::get_singleton()->load(arc_path);
    ERR_FAIL_COND_V_MSG(view.is_null() || archive.is_null() || view->get_Chunks().is_empty(), ERR_FILE_CORRUPT, "Could not load UI inputs");

    font_cache.clear();
    archive_fonts.clear();
    PackedStringArray files = archive->get_files();
    for (const String &file : files) {
        const String extension = file.get_extension();
        if (extension != "fcpx" && extension != "bfcpx") continue;
        const String name = file.get_file().get_basename() + ".bfcpx";
        ERR_FAIL_COND_V_MSG(archive_fonts.has(name), ERR_INVALID_DATA, "Ambiguous composite font name in ARC: " + name);
        archive_fonts[name] = archive->get_file_data(file);
    }
    PackedStringArray bflyt_files;
    for (int i = 0; i < files.size(); ++i) 
    {
        if (files[i].ends_with(".bflyt")) 
            bflyt_files.push_back(files[i]);
    }
    ERR_FAIL_COND_V_MSG(bflyt_files.is_empty(), ERR_FILE_NOT_FOUND, "BFLYT layout not found in ARC");
    
    // Load all layouts from the archive
    PackedStringArray referenced;
    String selected;
    for (int i = 0; i < bflyt_files.size(); ++i) 
    {
        Ref<BinaryLayout> parsed;
        parsed.instantiate();
        Error result = parsed->LoadFromBuffer(archive->get_file_data(bflyt_files[i]));
        ERR_FAIL_COND_V_MSG(result != OK, result, "Could not parse UI layout");
        String name = bflyt_files[i].get_file().get_basename();
        ERR_FAIL_COND_V_MSG(layouts.has(name), ERR_INVALID_DATA, "Ambiguous UI layout name");
        layouts[name] = parsed;
        if (name == arc_path.get_file().get_basename()) 
            selected = name;
        
        Array panes = parsed->get_layout()["panes"];
        for (int j = 0; j < panes.size(); ++j) 
        {
            Dictionary pane = panes[j];
            if (pane.has("parts_name")) 
                referenced.push_back(String(pane["parts_name"]).get_file().get_basename());
        }
    }
    if (selected.is_empty()) 
    {
        Array names = layouts.keys();
        for (int i = 0; i < names.size(); ++i) 
        {
            if (referenced.has(names[i])) continue;
            ERR_FAIL_COND_V_MSG(!selected.is_empty(), ERR_INVALID_DATA, "Multiple UI entry layouts; archive name must identify the entry layout");
            selected = names[i];
        }
    }
    ERR_FAIL_COND_V_MSG(selected.is_empty(), ERR_INVALID_DATA, "No UI entry layout found");
    Error error;

    // Setup custom ui shader
    Ref<Shader> picture_shader = ResourceLoader::get_singleton()->load("res://gflib/shaders/ui_picture.gdshader");
    ERR_FAIL_COND_V_MSG(picture_shader.is_null(), ERR_FILE_NOT_FOUND, "UI picture shader not found");
    
    // Load all textures from the archive
    for (int i = 0; i < files.size(); ++i) 
    {
        if (!files[i].ends_with(".bntx")) continue;
        Ref<BinaryTextureArchive> texture_archive;
        texture_archive.instantiate();
        error = texture_archive->LoadFromBuffer(archive->get_file_data(files[i]));
        if (error != OK) 
        {
            ERR_PRINT("Failed to load BNTX archive: " + files[i]);
            return error;
        }
        Dictionary images = texture_archive->get_textures();
        Array names = images.keys();
        for (int j = 0; j < names.size(); ++j) 
        {
            ERR_FAIL_COND_V_MSG(textures.has(names[j]), ERR_INVALID_DATA, "Ambiguous texture name across BNTX archives");
            Ref<BinaryTexture> image = images[names[j]];
            textures[names[j]] = ImageTexture::create_from_image(image);
            Ref<ShaderMaterial> shader_material;
            shader_material.instantiate();
            shader_material->set_shader(picture_shader);
            shader_material->set_shader_parameter("channel_sources", image->get_channel_sources());
            texture_materials[names[j]] = shader_material;
        }
    }

    if (!build_layout(selected, nullptr, PackedStringArray())) 
    {
        if (layout) 
            memdelete(layout);
        layout = nullptr;
        return ERR_INVALID_DATA;
    }
    warn_once(warnings, "Static layout only: BFLAN playback, UIKit actions, and game material shaders are not implemented");

    layout_parent->add_child(layout);
    Node *scene_owner = layout_parent == this || is_ancestor_of(layout_parent) ? this : layout_parent;
    layout->set_owner(scene_owner);
    set_meta("layout_path", get_path_to(layout));

    Dictionary paths;
    for (Control *node : nodes) 
    {
        node->set_owner(scene_owner);
        for (int j = 0; j < node->get_child_count(); ++j) node->get_child(j)->set_owner(scene_owner);
        paths[String(layout->get_path_to(node))] = get_path_to(node);
    }
    set_meta("pane_paths", paths);
    set_meta("truiv", view);
    set_meta("ui_archive", archive);
    set_meta("ui_textures", textures);
    set_meta("ui_texture_materials", texture_materials);
    set_meta("conversion_warnings", warnings);
    set_meta("layout_file", selected);
    set_mouse_filter(MOUSE_FILTER_IGNORE);
    fit_layout();
    state_runtime.reset(this, layout);
    return OK;
}
