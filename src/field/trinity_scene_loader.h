#pragma once

#include "fbs/scene/trscn.h"
#include "fbs/scene/components/trinity_SceneObject.h"
#include "fbs/scene/components/trinity_CollisionComponent.h"
#include "fbs/scene/components/trinity_ModelInstancerComponent.h"
#include "field/trinity_map_replace.h"
#include "fbs/scene/components/trinity_LightApplierComponent.h"
#include "lights/trinity_environment.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <vector>

namespace godot
{

// Builds a node tree from a TRSCN file (sub scenes, scene objects, collision)
// and streams the referenced models in over several frames.
class TrinitySceneLoader : public Node3D
{
    GDCLASS(TrinitySceneLoader, Node3D)

    struct ModelJob
    {
        uint64_t parent_id = 0;
        String dir;
        String file;
        Ref<TrinityModelInstancerComponent> instancer;
    };

    String scene_path;
    int models_per_frame = 3;
    double max_load_time_ms = 4.0;
    // Geometry is immutable during a load; all placements share one physics shape.
    Dictionary collision_shapes;
    Dictionary story_state;
    bool use_map_replace = true;
    Ref<TrinityMapReplace> map_replace;
    uint64_t content_id = 0;
    std::vector<ModelJob> queue;
    size_t queue_pos = 0;
    bool loading = false;
    uint64_t load_generation = 0;

    struct LightJob
    {
        uint64_t parent_id = 0;
        Ref<TrinityLightApplierComponent> component;
        Ref<TRLGT> light;
        Transform3D instance_transform;
        bool instanced = false;
        double elapsed = 0.0;
    };

    NodePath environment_path;
    uint64_t light_environment_id = 0;
    std::vector<LightJob> light_jobs;
    std::vector<TrinityEnvironment::SceneLayer> applied_light_layers;
    bool warned_environment = false;

    Node3D* load_scene_file(const String& file, Node3D* parent);
    void load_entry(const Ref<TRScene>& entry, Node3D* parent, const String& base_dir,
                    const Ref<TrinityModelInstancerComponent>& instancer);
    Node3D* add_scene_object(const Ref<TrinitySceneObject>& object, const String& name, Node3D* parent);
    Ref<ConcavePolygonShape3D> load_collision_shape(const String& path);
    void add_collision(const Ref<TrinityCollisionComponent>& component, Node3D* parent);
    void add_light_applier(const Ref<TrinityLightApplierComponent>& component, Node3D* parent,
                           const Ref<TrinityModelInstancerComponent>& instancer);
    void update_lights(double delta);
    void detach_lights();
    void instantiate_model(const ModelJob& job);
    void set_editor_owner(Node* node, bool recursive = false);

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    void set_scene_path(const String& path)
    {
        scene_path = path;
    }

    String get_scene_path() const
    {
        return scene_path;
    }

    void set_models_per_frame(int count)
    {
        models_per_frame = MAX(count, 1);
    }

    int get_models_per_frame() const
    {
        return models_per_frame;
    }

    void set_max_load_time_ms(double milliseconds);

    double get_max_load_time_ms() const
    {
        return max_load_time_ms;
    }

    Callable get_load_button();

    void set_story_state(const Dictionary& state)
    {
        story_state = state;
    }

    Dictionary get_story_state() const
    {
        return story_state;
    }

    void set_use_map_replace(bool enabled)
    {
        use_map_replace = enabled;
    }

    bool get_use_map_replace() const
    {
        return use_map_replace;
    }

    void set_environment_path(const NodePath& path);

    NodePath get_environment_path() const
    {
        return environment_path;
    }

    void load_scene();
    void clear();

    bool is_loading() const
    {
        return loading;
    }

    Node3D* get_content() const;
    void _process(double delta) override;
};
}
