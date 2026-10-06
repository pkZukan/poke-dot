#include "ui_converter.h"
#include "ui_fbs/truiv.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TrinityUI::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("load_ui", "truiv_path", "arc_path", "parent_path"), &TrinityUI::load_ui);
    ClassDB::bind_method(D_METHOD("apply_state", "component", "state", "frame"), &TrinityUI::apply_state, DEFVAL(0.0));
    ClassDB::bind_method(D_METHOD("get_warnings"), &TrinityUI::get_warnings);
}

Control *TrinityUI::layout_node() const {
    return Object::cast_to<Control>(get_node_or_null(get_meta("layout_path", NodePath("Layout"))));
}

Control *TrinityUI::scope_root() const {
    return layout_node();
}

void TrinityUI::_notification(int what) {
    if (what == NOTIFICATION_RESIZED || what == NOTIFICATION_READY) fit_layout();
}

void TrinityUI::fit_layout() {
    Control *root = layout_node();
    if (!root || !root->has_meta("layout_size")) return;
    const Vector2 native_size = root->get_meta("layout_size");
    if (native_size.x <= 0 || native_size.y <= 0) return;
    Vector2 available = get_size();
    if (available.x <= 0 || available.y <= 0) available = native_size;
    const real_t factor = MIN(available.x / native_size.x, available.y / native_size.y);
    // Fit BFLYT's native layout coordinates into Godot's control area without changing their aspect ratio.
    root->set_scale(Vector2(factor, factor));
    root->set_position((available - native_size * factor) * 0.5);
}

Error TrinityUI::apply_state(const String &component, const String &state, double frame) {
    Control *root = layout_node();
    ERR_FAIL_NULL_V(root, ERR_UNCONFIGURED);
    return state_runtime.apply(this, root, component, state, frame);
}

PackedStringArray TrinityUI::get_warnings() const {
    return get_meta("conversion_warnings", PackedStringArray());
}

Ref<Font> TrinityUI::get_font(const String &p_name) {
    const int before = warnings.size();
    Ref<Font> font = fonts.get(p_name, warnings);
    if (warnings.size() != before) set_meta("conversion_warnings", warnings);
    return font;
}

void TrinityUI::clear_loaded_ui() {
    if (layout_id != 0 && UtilityFunctions::is_instance_id_valid(layout_id)) {
        if (Node *old_parent = layout->get_parent()) old_parent->remove_child(layout);
        layout->queue_free();
    }
    layout = nullptr;
    layout_id = 0;
}

Error TrinityUI::load_ui(const String &truiv_path, const String &arc_path, const NodePath &parent_path)
{
    Node *layout_parent = get_node_or_null(parent_path);
    ERR_FAIL_NULL_V_MSG(layout_parent, ERR_DOES_NOT_EXIST, "UI parent path does not exist");
    ERR_FAIL_COND_V_MSG(!FileAccess::file_exists(truiv_path) || !FileAccess::file_exists(arc_path),
            ERR_FILE_NOT_FOUND, "UI input file does not exist");

    Ref<TRUIV> view = ResourceLoader::get_singleton()->load(truiv_path);
    Ref<SeadArchive> archive = ResourceLoader::get_singleton()->load(arc_path);
    ERR_FAIL_COND_V_MSG(view.is_null() || archive.is_null() || view->get_Chunks().is_empty(),
            ERR_FILE_CORRUPT, "Could not load UI inputs");

    UIBuilder builder(fonts);
    TrinityPane *new_layout = nullptr;
    Error error = builder.build(archive, arc_path, new_layout);
    if (error != OK) return error;

    clear_loaded_ui();
    layout = new_layout;
    layout_id = layout->get_instance_id();
    warnings = builder.warnings;
    ui_warn_once(warnings, "Static layout only: BFLAN playback, UIKit actions, and game material shaders are not implemented");

    layout_parent->add_child(layout);
    Node *scene_owner = layout_parent == this || is_ancestor_of(layout_parent) ? this : layout_parent;
    layout->set_owner(scene_owner);
    set_meta("layout_path", get_path_to(layout));

    Dictionary paths;
    for (Control *node : builder.nodes)
    {
        node->set_owner(scene_owner);
        for (int j = 0; j < node->get_child_count(); ++j) node->get_child(j)->set_owner(scene_owner);
        paths[String(layout->get_path_to(node))] = get_path_to(node);
    }
    set_meta("pane_paths", paths);
    set_meta("truiv", view);
    set_meta("ui_archive", archive);
    set_meta("ui_textures", builder.textures);
    set_meta("ui_texture_materials", builder.texture_materials);
    set_meta("conversion_warnings", warnings);
    set_meta("layout_file", builder.entry_layout);
    set_mouse_filter(MOUSE_FILTER_IGNORE);
    fit_layout();
    state_runtime.reset(this, layout);
    return OK;
}
