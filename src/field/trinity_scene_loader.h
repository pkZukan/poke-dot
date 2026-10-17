#pragma once

#include "fbs/scene/trscn.h"
#include "fbs/scene/components/trinity_SceneObject.h"
#include "fbs/scene/components/trinity_CollisionComponent.h"
#include "fbs/scene/components/trinity_ModelInstancerComponent.h"
#include <godot_cpp/classes/node3d.hpp>
#include <vector>

namespace godot {

// Builds a node tree from a TRSCN file (sub scenes, scene objects, collision)
// and streams the referenced models in over several frames.
class TrinitySceneLoader : public Node3D {
    GDCLASS(TrinitySceneLoader, Node3D)
    struct ModelJob {
        uint64_t parent_id = 0;
        String dir;
        String file;
        Ref<TrinityModelInstancerComponent> instancer;
    };
    String scene_path;
    int models_per_frame = 3;
    uint64_t content_id = 0;
    std::vector<ModelJob> queue;
    size_t queue_pos = 0;
    bool loading = false;

    Node3D *load_scene_file(const String &file, Node3D *parent);
    void load_entry(const Ref<TRScene> &entry, Node3D *parent, const String &base_dir, const Ref<TrinityModelInstancerComponent> &instancer);
    Node3D *add_scene_object(const Ref<TrinitySceneObject> &object, const String &name, Node3D *parent);
    void add_collision(const Ref<TrinityCollisionComponent> &component, Node3D *parent);
    void instantiate_model(const ModelJob &job);
    void set_editor_owner(Node *node, bool recursive = false);

protected:
    static void _bind_methods();
    void _notification(int what);

public:
    void set_scene_path(const String &path) { scene_path = path; }
    String get_scene_path() const { return scene_path; }
    void set_models_per_frame(int count) { models_per_frame = MAX(count, 1); }
    int get_models_per_frame() const { return models_per_frame; }
    Callable get_load_button();

    void load_scene();
    void clear();
    bool is_loading() const { return loading; }
    Node3D *get_content() const;
    void _process(double delta) override;
};
}
