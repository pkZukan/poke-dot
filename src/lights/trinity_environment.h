#pragma once

#include "fbs/gfx2/trlgt.h"
#include "lights/retail_tonemap.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/environment.hpp>
#include <godot_cpp/classes/camera_attributes_practical.hpp>
#include <godot_cpp/classes/world_environment.hpp>
#include <godot_cpp/classes/directional_light3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/cubemap.hpp>
#include <godot_cpp/classes/sky.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <vector>

namespace godot
{

class TrinityEnvironment : public Node3D
{
    GDCLASS(TrinityEnvironment, Node3D)
public:
    // Scene-owned layers retain the source resource's path for relative textures.
    struct SceneLayer
    {
        Ref<TRLGT> light;
        int priority = 0;
        double weight = 1.0;
        double sample_time = 0.0;
        Transform3D transform;
        String name_prefix;
    };

private:
    struct Layer
    {
        StringName name;
        Ref<TRLGT> light;
        int priority = 0;
        double weight = 1.0;
        // Weather offset files (light/offset/offset_*) hold deltas that are
        // added onto the stack below instead of blended toward.
        bool additive = false;
        uint64_t scene_owner = 0;
        double sample_time = -1.0;
        Transform3D transform;
        String name_prefix;
    };

    Ref<TRLGT> master_light;
    std::vector<Layer> layers;
    Dictionary effective_state;
    std::vector<size_t> ordered_layer_indices;
    bool state_dirty = true;
    bool render_dirty = true;
    bool layer_order_dirty = true;
    bool update_queued = false;
    Dictionary diffuse_probe_cache;
    Dictionary texture_cache;
    Dictionary global_textures;
    // Scene-linear range kept by the retail tonemap LUT; retail only gains ~1/255 above it.
    static constexpr double RETAIL_TONEMAP_HEADROOM = 8.0;
    Ref<ImageTexture3D> tonemap_lut;
    PackedFloat64Array tonemap_lut_key;
    Ref<Sky> probe_sky;
    Ref<ShaderMaterial> probe_sky_material;
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

    void resolve_resource(const Ref<TRLGT>& resource, double weight, bool additive, double sample_time = -1.0,
                          const Transform3D& transform = Transform3D(), const String& name_prefix = String());
    Dictionary find_type(const String& type) const;
    Dictionary find_category(int category) const;
    double number(const Dictionary& light, const String& name, double fallback) const;
    Color color(const Dictionary& light, const String& name, Color fallback, bool source_is_linear = true) const;
    void queue_update(bool resolve = true);
    void flush_update();
    void detach_targets();
    void apply_render_state();
    bool probe_average(const Dictionary& probe, Color& average);
    void apply_probe_ambient(const Ref<Environment>& environment);
    void apply_probe_specular(const Ref<Environment>& environment);
    Ref<Cubemap> load_cubemap(const String& path);
    Ref<Texture2D> load_texture(const String& path);
    void update_sky(const Dictionary& sky_data);
    void update_world_shadow();
    void set_global_texture(const StringName& name, const Ref<Texture>& texture);
    Ref<ImageTexture3D> retail_tonemap_lut(const Dictionary& global);

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    void set_time_of_day(double hours);

    double get_time_of_day() const
    {
        return time_of_day;
    }

    void set_master_light(const Ref<TRLGT>& light);

    Ref<TRLGT> get_master_light() const
    {
        return master_light;
    }

    void set_enabled(bool value);

    bool is_enabled() const
    {
        return enabled;
    }

    void set_use_native_bloom(bool value);

    bool get_use_native_bloom() const
    {
        return use_native_bloom;
    }

    void set_lighting_category(int category);

    int get_lighting_category() const
    {
        return lighting_category;
    }

    void set_world_environment_path(const NodePath& path);

    NodePath get_world_environment_path() const
    {
        return world_environment_path;
    }

    void set_camera_path(const NodePath& path);

    NodePath get_camera_path() const
    {
        return camera_path;
    }

    void set_layer(const StringName& name, const Ref<TRLGT>& light, int priority = 0, double weight = 1.0,
                   bool additive = false);
    void set_layer_weight(const StringName& name, double weight);
    void remove_layer(const StringName& name);
    void clear_layers();
    void set_scene_layers(uint64_t owner, const std::vector<SceneLayer>& scene_layers);
    void remove_scene_layers(uint64_t owner);
    // Force an immediate rebuild, including edits made inside nested TRLGT resources.
    // Setters otherwise coalesce their changes until the deferred update.
    void apply();

    Dictionary get_effective_state() const
    {
        return effective_state.duplicate(true);
    }

    Ref<Environment> get_environment() const
    {
        return applied_environment;
    }

    Ref<CameraAttributesPractical> get_camera_attributes() const
    {
        return applied_attributes;
    }
};
}
