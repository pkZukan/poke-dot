#include "ui_converter.h"
#include "ui_fbs/truiv.h"
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
    ClassDB::bind_method(D_METHOD("load_ui", "truiv_path", "arc_path", "layout_file"), &TrinityUI::load_ui, DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("get_pane", "name"), &TrinityUI::get_pane);
    ClassDB::bind_method(D_METHOD("get_warnings"), &TrinityUI::get_warnings);
}

void TrinityUI::_notification(int what) {
    if (what == NOTIFICATION_RESIZED || what == NOTIFICATION_READY) fit_layout();
}

void TrinityUI::fit_layout() {
    Control *layout = Object::cast_to<Control>(get_node_or_null(NodePath("Layout")));
    if (!layout || !layout->has_meta("layout_size")) return;
    const Vector2 native_size = layout->get_meta("layout_size");
    if (native_size.x <= 0 || native_size.y <= 0) return;
    Vector2 available = get_size();
    if (available.x <= 0 || available.y <= 0) available = native_size;
    const real_t factor = MIN(available.x / native_size.x, available.y / native_size.y);
    layout->set_scale(Vector2(factor, factor));
    layout->set_position((available - native_size * factor) * 0.5);
}

Control *TrinityUI::get_pane(const String &name) const {
    // Paths are saved as metadata so lookups also work after PackedScene loading.
    Dictionary paths = get_meta("pane_paths", Dictionary());
    if (!paths.has(name)) return nullptr;
    return Object::cast_to<Control>(get_node_or_null(paths[name]));
}

PackedStringArray TrinityUI::get_warnings() const {
    return get_meta("conversion_warnings", PackedStringArray());
}

Error TrinityUI::load_ui(const String &truiv_path, const String &arc_path, const String &layout_file) {
    ERR_FAIL_COND_V_MSG(!FileAccess::file_exists(truiv_path) || !FileAccess::file_exists(arc_path), ERR_FILE_NOT_FOUND, "UI input file does not exist");
    Ref<TRUIV> view = ResourceLoader::get_singleton()->load(truiv_path);
    Ref<SeadArchive> archive = ResourceLoader::get_singleton()->load(arc_path);
    ERR_FAIL_COND_V_MSG(view.is_null() || archive.is_null() || view->get_Chunks().is_empty(), ERR_FILE_CORRUPT, "Could not load UI inputs");
    String selected = layout_file;
    PackedStringArray files = archive->get_files();
    if (selected.is_empty()) {
        for (int i = 0; i < files.size(); ++i) {
            if (!files[i].ends_with(".bflyt")) continue;
            ERR_FAIL_COND_V_MSG(!selected.is_empty(), ERR_INVALID_PARAMETER, "ARC contains multiple layouts; specify layout_file");
            selected = files[i];
        }
    }
    ERR_FAIL_COND_V_MSG(selected.is_empty() || !archive->has_file(selected), ERR_FILE_NOT_FOUND, "BFLYT layout not found in ARC");
    Ref<BinaryLayout> binary_layout;
    binary_layout.instantiate();
    Error error = binary_layout->LoadFromBuffer(archive->get_file_data(selected));
    if (error != OK) return error;
    Dictionary data = binary_layout->get_layout();
    Vector2 native_size = data["size"];
    ERR_FAIL_COND_V(native_size.x <= 0 || native_size.y <= 0, ERR_FILE_CORRUPT);

    Ref<Shader> picture_shader = ResourceLoader::get_singleton()->load("res://gflib/shaders/ui_picture.gdshader");
    ERR_FAIL_COND_V_MSG(picture_shader.is_null(), ERR_FILE_NOT_FOUND, "UI picture shader not found");
    Dictionary textures;
    Dictionary texture_materials;
    for (int i = 0; i < files.size(); ++i) {
        if (!files[i].ends_with(".bntx")) continue;
        Ref<BinaryTextureArchive> texture_archive;
        texture_archive.instantiate();
        error = texture_archive->LoadFromBuffer(archive->get_file_data(files[i]));
        if (error != OK) return error;
        Dictionary images = texture_archive->get_textures();
        Array names = images.keys();
        for (int j = 0; j < names.size(); ++j) {
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

    // Build off-tree, then replace only the subtree owned by the converter.
    Control *layout = memnew(Control);
    layout->set_name("Layout");
    layout->set_mouse_filter(MOUSE_FILTER_IGNORE);
    layout->set_size(native_size);
    layout->set_meta("layout_size", native_size);
    layout->set_meta("bflyt", binary_layout);
    Array panes = data["panes"];
    Array materials = data["materials"];
    PackedStringArray texture_names = data["textures"];
    PackedStringArray warnings;
    std::vector<Control *> nodes;
    Dictionary paths;
    for (int i = 0; i < panes.size(); ++i) {
        Dictionary pane = panes[i];
        int parent_index = pane["parent"];
        if (parent_index < -1 || parent_index >= i) {
            memdelete(layout);
            return ERR_FILE_CORRUPT;
        }
        Control *parent = parent_index < 0 ? layout : nodes[parent_index];
        Control *node = memnew(Control);
        node->set_name(pane["name"]);
        node->set_mouse_filter(MOUSE_FILTER_IGNORE);
        parent->add_child(node);
        nodes.push_back(node);
        node->set_meta("bflyt_pane", pane);
        const Vector2 size = pane["size"];
        const int origin = pane["origin"];
        const Vector2 pivot = size * pane_origin(origin);
        const Vector3 translation = pane["translation"];
        const Vector3 rotation = pane["rotation"];
        Vector2 base;
        if (parent_index < 0) {
            if (bool(data["draw_from_center"])) base = native_size * 0.5;
        } else {
            base = parent->get_size() * pane_origin(origin >> 4);
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
        if (flags & 2) node->set_modulate(alpha);
        const String type = pane["type"];
        if (type != "pic1") {
            if (type != "pan1" && type != "bnd1") warn_once(warnings, "Unrendered pane type: " + type);
            continue;
        }
        Dictionary material = materials[int(pane["material_index"])];
        Array maps = material["texture_maps"];
        if (maps.is_empty()) {
            warn_once(warnings, "Picture material without a texture: " + String(material["name"]));
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
        picture->set_material(texture_materials[texture_name]);
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
    warn_once(warnings, "Static layout only: BFLAN playback, UIKit actions, and game material shaders are not implemented");
    Node *old = get_node_or_null(NodePath("Layout"));
    if (old) { remove_child(old); memdelete(old); }
    add_child(layout);
    layout->set_owner(this);
    for (Control *node : nodes) {
        node->set_owner(this);
        for (int j = 0; j < node->get_child_count(); ++j) node->get_child(j)->set_owner(this);
        String original_name = Dictionary(node->get_meta("bflyt_pane"))["name"];
        if (paths.has(original_name)) warn_once(warnings, "Duplicate pane name: " + original_name);
        else paths[original_name] = get_path_to(node);
    }
    set_meta("pane_paths", paths);
    set_meta("truiv", view);
    set_meta("conversion_warnings", warnings);
    set_meta("layout_file", selected);
    set_mouse_filter(MOUSE_FILTER_IGNORE);
    fit_layout();
    return OK;
}
