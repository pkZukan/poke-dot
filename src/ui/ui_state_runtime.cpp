#include "ui_state_runtime.h"
#include "middleware/bflan.h"
#include "middleware/sarc.h"
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/core/math.hpp>
#include <cmath>

using namespace godot;

void UIStateRuntime::index(Node *owner, Control *node, Instance *instance) {
    if (node->get_meta("layout_instance", false)) {
        instance = &instances[String(owner->get_path_to(node))];
        instance->layout_name = node->get_meta("layout_name");
    }
    if (instance && node->has_meta("bflyt_pane")) {
        Dictionary pane = node->get_meta("bflyt_pane");
        NodePath path = owner->get_path_to(node);
        instance->panes[String(pane["name"])].push_back(path);
        Node *picture = node->get_node_or_null("Picture");
        if (picture && picture->has_meta("material_name"))
            instance->materials[String(picture->get_meta("material_name"))].push_back(path);
    }
    for (int i = 0; i < node->get_child_count(); ++i) {
        Control *child = Object::cast_to<Control>(node->get_child(i));
        if (child) index(owner, child, instance);
    }
}

void UIStateRuntime::reset(Node *owner, Control *layout) {
    instances.clear();
    components.clear();
    animations.clear();
    animation_files.clear();
    initialized = true;
    if (!layout) return;
    index(owner, layout, nullptr);
    for (const auto &entry : instances) {
        NodePath root_path(entry.first);
        Node *root = owner->get_node_or_null(root_path);
        Node *component = root->get_parent() == layout ? root : root->get_parent();
        Dictionary pane = component->get_meta("bflyt_pane", Dictionary());
        components[String(pane["name"])].push_back(root_path);
        String path = layout->get_path_to(component);
        if (path != String(pane["name"])) components[path].push_back(root_path);
        if (component == root) components["."].push_back(root_path);
    }
    Ref<SeadArchive> archive = owner->get_meta("ui_archive", Variant());
    if (archive.is_null()) return;
    PackedStringArray files = archive->get_files();
    for (int i = 0; i < files.size(); ++i) {
        if (files[i].ends_with(".bflan"))
            animation_files[files[i].get_file().get_basename()] = files[i];
    }
}

double UIStateRuntime::sample(const Dictionary &track, double frame) {
    Array keys = track["keyframes"];
    Dictionary left = keys[0];
    for (int i = 0; i < keys.size(); ++i) {
        Dictionary right = keys[i];
        if (double(right["frame"]) > frame) {
            if (int(track["curve_type"]) == 1 || frame <= double(left["frame"])) return left["value"];
            double span = double(right["frame"]) - double(left["frame"]);
            double t = (frame - double(left["frame"])) / span;
            return (2*t*t*t-3*t*t+1)*double(left["value"]) + (t*t*t-2*t*t+t)*span*double(left["slope"])
                + (-2*t*t*t+3*t*t)*double(right["value"]) + (t*t*t-t*t)*span*double(right["slope"]);
        }
        left = right;
    }
    return left["value"];
}

Error UIStateRuntime::apply(Node *owner, Control *layout, const String &component, const String &state, double frame) {
    ERR_FAIL_COND_V(!std::isfinite(frame), ERR_INVALID_PARAMETER);
    if (!initialized) reset(owner, layout); // Rebuild bindings after PackedScene instantiation.
    auto scopes = components.find(component);
    ERR_FAIL_COND_V_MSG(scopes == components.end(), ERR_DOES_NOT_EXIST, "Unknown UI component: " + component);
    ERR_FAIL_COND_V_MSG(scopes->second.size() != 1, ERR_INVALID_PARAMETER, "Ambiguous UI component; use its path relative to Layout: " + component);
    const NodePath &root_path = scopes->second.front();
    ERR_FAIL_NULL_V(owner->get_node_or_null(root_path), ERR_DOES_NOT_EXIST);
    auto instance = instances.find(String(root_path));
    ERR_FAIL_COND_V(instance == instances.end(), ERR_DOES_NOT_EXIST);
    String name = instance->second.layout_name + "_" + state;
    ERR_FAIL_COND_V_MSG(!animation_files.has(name), ERR_FILE_NOT_FOUND, "Missing UI state: " + name);
    if (!animations.has(name)) {
        Ref<SeadArchive> archive = owner->get_meta("ui_archive");
        ERR_FAIL_COND_V(archive.is_null(), ERR_UNCONFIGURED);
        Ref<BinaryLayoutAnimation> parsed;
        parsed.instantiate();
        parsed->LoadFromBuffer(archive->get_file_data(animation_files[name]));
        Dictionary animation = parsed->get_animation();
        ERR_FAIL_COND_V(!animation.has("pai1"), ERR_FILE_CORRUPT);
        animations[name] = animation["pai1"];
    }
    Dictionary data = animations[name];
    Array entries = data["entries"];
    for (int i = 0; i < entries.size(); ++i) {
        Dictionary entry = entries[i];
        int type = entry["target_type"];
        if (type != 0 && type != 1) continue;
        const auto &bindings = type == 0 ? instance->second.panes : instance->second.materials;
        auto targets = bindings.find(String(entry["name"]));
        if (targets == bindings.end()) continue;
        for (const NodePath &path : targets->second) {
            Control *pane = Object::cast_to<Control>(owner->get_node_or_null(path));
            if (!pane) continue;
            Polygon2D *picture = Object::cast_to<Polygon2D>(pane->get_node_or_null("Picture"));
            Array tags = entry["tags"];
            for (int j = 0; j < tags.size(); ++j) {
                Dictionary tag = tags[j];
                Array tracks = tag["tracks"];
                for (int k = 0; k < tracks.size(); ++k) {
                    Dictionary track = tracks[k];
                    if (!bool(track.get("supported", false)) || Array(track.get("keyframes", Array())).is_empty()) continue;
                    apply_track(owner, pane, picture, tag["type"], track, sample(track, frame), data["textures"]);
                }
            }
        }
    }
    return OK;
}

void UIStateRuntime::apply_track(Node *owner, Control *pane, Polygon2D *picture, const String &kind, const Dictionary &track, double value, const PackedStringArray &textures) 
{
    int target = track["target"];
    Dictionary metadata = pane->get_meta("bflyt_pane", Dictionary());
    Ref<ShaderMaterial> material = picture ? Ref<ShaderMaterial>(picture->get_material()) : Ref<ShaderMaterial>();
    if (kind == "FLVI") 
    {
        pane->set_visible(value != 0);
    } 
    else if (kind == "FLVC") 
    {
        if (target == 16) 
        {
            if ((int(metadata.get("flags", 0)) & 2) || String(metadata.get("type", "")) == "prt1") {
                Color color = pane->get_modulate();
                color.a = value / 255.0;
                pane->set_modulate(color);
            } else if (picture) {
                Color color = picture->get_self_modulate();
                color.a = value / 255.0;
                picture->set_self_modulate(color);
            } else {
                Color color = pane->get_self_modulate();
                color.a = value / 255.0;
                pane->set_self_modulate(color);
            }
        } 
        else if (Object::cast_to<Label>(pane) && target < 4) 
        {
            Label *label = Object::cast_to<Label>(pane);
            Color color = label->get_theme_color("font_color"); color[target] = value / 255.0;
            label->add_theme_color_override("font_color", color);
        } 
        else if (picture && target < 16 && picture->get_vertex_colors().size() == 4) 
        {
            const int order[] = {0, 1, 3, 2};
            PackedColorArray colors = picture->get_vertex_colors();
            Color color = colors[order[target / 4]]; color[target % 4] = value / 255.0;
            colors.set(order[target / 4], color); picture->set_vertex_colors(colors);
        }
    } 
    else if (kind == "FLPA") 
    {
        if (target < 2) 
        {
            if (!pane->has_meta("animation_position_base")) 
            {
                Vector3 translation = metadata["translation"];
                pane->set_meta("animation_position_base", pane->get_position() - Vector2(translation.x, -translation.y));
            }
            Vector2 base = pane->get_meta("animation_position_base"), position = pane->get_position();
            position[target] = base[target] + (target == 0 ? value : -value); pane->set_position(position);
        }
        else if (target == 5) pane->set_rotation(-Math::deg_to_rad(value));
        else if (target == 6 || target == 7) 
        {
            Vector2 scale = pane->get_scale(); scale[target - 6] = value; pane->set_scale(scale);
        }
    } 
    else if (kind == "FLMC" && material.is_valid() && target < 8) {
        String parameter = target < 4 ? "black_color" : "white_color";
        Color color = material->get_shader_parameter(parameter); color[target % 4] = value / 255.0;
        material->set_shader_parameter(parameter, color);
    } 
    else if (kind == "FLTS" && material.is_valid() && int(track["index"]) == 0) 
    {
        if (target < 2) 
        {
            Vector2 offset = material->get_shader_parameter("uv_translation"); offset[target] = value;
            material->set_shader_parameter("uv_translation", offset);
        } 
        else if (target == 2) material->set_shader_parameter("uv_rotation", Math::deg_to_rad(value));
        else if (target < 5) 
        {
            Vector2 scale = material->get_shader_parameter("uv_scale"); scale[target - 3] = value;
            material->set_shader_parameter("uv_scale", scale);
        }
    } 
    else if (kind == "FLTP" && material.is_valid() && int(track["index"]) == 0) 
    {
        if (value < 0 || value >= textures.size() || picture->get_texture().is_null()) return;
        String name = textures[int(value)];
        Dictionary available = owner->get_meta("ui_textures"), materials = owner->get_meta("ui_texture_materials");
        if (!available.has(name) || !materials.has(name)) return;
        Vector2 old_size = picture->get_texture()->get_size();
        if (old_size.x <= 0 || old_size.y <= 0) return;
        Ref<Texture2D> texture = available[name];
        PackedVector2Array uv = picture->get_uv();
        for (int i = 0; i < uv.size(); ++i) uv.set(i, uv[i] / old_size * texture->get_size());
        picture->set_texture(texture); picture->set_uv(uv);
        Ref<ShaderMaterial> source = materials[name];
        material->set_shader_parameter("channel_sources", source->get_shader_parameter("channel_sources"));
    }
}
