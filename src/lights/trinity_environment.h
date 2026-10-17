#pragma once

#include "fbs/gfx2/trlgt.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/environment.hpp>
#include <godot_cpp/classes/camera_attributes_practical.hpp>
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/classes/directional_light3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <vector>

namespace godot {

class TrinityEnvironment : public Node3D {
    GDCLASS(TrinityEnvironment, Node3D)
    struct Layer {
        StringName name;
        Ref<TRLGT> light;
        int priority = 0;
        double weight = 1.0;
        // Weather offset files (light/offset/offset_*) hold deltas that are
        // added onto the stack below instead of blended toward.
        bool additive = false;
    };
    Ref<TRLGT> master_light;
    std::vector<Layer> layers;
    Dictionary effective_state;
    Dictionary diffuse_probe_cache;
    Dictionary texture_cache;
    NodePath world_environment_path;
    NodePath camera_path;
    bool enabled = true;
    bool use_native_bloom = false;
    bool applying = false;
    int lighting_category = 0;
    double time_of_day = 12.0;
    uint64_t owned_world_id = 0;
    uint64_t owned_light_id = 0;
    uint64_t bound_world_id = 0;
    uint64_t bound_camera_id = 0;
    Ref<Environment> original_environment;
    Ref<CameraAttributes> original_world_attributes;
    Ref<CameraAttributes> original_camera_attributes;
    Ref<Environment> applied_environment;
    Ref<CameraAttributesPractical> applied_attributes;
    Ref<Environment> baseline_environment;
    Ref<CameraAttributesPractical> baseline_attributes;

    void resolve_resource(const Ref<TRLGT> &resource, double weight, bool additive);
    Dictionary find_type(const String &type) const;
    Dictionary find_category(int category) const;
    double number(const Dictionary &light, const String &name, double fallback) const;
    Color color(const Dictionary &light, const String &name, Color fallback, bool source_is_linear = true) const;
    void detach_targets();
    void apply_render_state();
    bool probe_average(const Dictionary &probe, Color &average);
    void apply_probe_ambient(const Ref<Environment> &environment);
    Ref<Texture2D> load_texture(const String &path);
    void update_sky(const Dictionary &sky_data);

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    void set_time_of_day(double hours);
    double get_time_of_day() const { return time_of_day; }
    void set_master_light(const Ref<TRLGT> &light);
    Ref<TRLGT> get_master_light() const { return master_light; }
    void set_enabled(bool value);
    bool is_enabled() const { return enabled; }
    void set_use_native_bloom(bool value);
    bool get_use_native_bloom() const { return use_native_bloom; }
    void set_lighting_category(int category);
    int get_lighting_category() const { return lighting_category; }
    void set_world_environment_path(const NodePath &path);
    NodePath get_world_environment_path() const { return world_environment_path; }
    void set_camera_path(const NodePath &path);
    NodePath get_camera_path() const { return camera_path; }

    void set_layer(const StringName &name, const Ref<TRLGT> &light, int priority = 0, double weight = 1.0, bool additive = false);
    void set_layer_weight(const StringName &name, double weight);
    void remove_layer(const StringName &name);
    void clear_layers();
    void apply();
    Dictionary get_effective_state() const { return effective_state.duplicate(true); }
    Ref<Environment> get_environment() const { return applied_environment; }
    Ref<CameraAttributesPractical> get_camera_attributes() const { return applied_attributes; }
};
}
