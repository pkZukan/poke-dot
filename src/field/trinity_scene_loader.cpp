#include "trinity_scene_loader.h"
#include "field/trcol.h"
#include "fbs/field/trins.h"
#include "fbs/scene/subscene.h"
#include "fbs/scene/components/trinity_ObjectTemplate.h"
#include "fbs/scene/components/trinity_ModelComponent.h"
#include "converters/model_converter.h"
#include "gflib.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>

using namespace godot;

void TrinitySceneLoader::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_scene_path", "path"), &TrinitySceneLoader::set_scene_path);
    ClassDB::bind_method(D_METHOD("get_scene_path"), &TrinitySceneLoader::get_scene_path);
    ClassDB::bind_method(D_METHOD("set_models_per_frame", "count"), &TrinitySceneLoader::set_models_per_frame);
    ClassDB::bind_method(D_METHOD("get_models_per_frame"), &TrinitySceneLoader::get_models_per_frame);
    ClassDB::bind_method(D_METHOD("set_max_load_time_ms", "milliseconds"), &TrinitySceneLoader::set_max_load_time_ms);
    ClassDB::bind_method(D_METHOD("get_max_load_time_ms"), &TrinitySceneLoader::get_max_load_time_ms);
    ClassDB::bind_method(D_METHOD("get_load_button"), &TrinitySceneLoader::get_load_button);
    ClassDB::bind_method(D_METHOD("set_story_state", "state"), &TrinitySceneLoader::set_story_state);
    ClassDB::bind_method(D_METHOD("get_story_state"), &TrinitySceneLoader::get_story_state);
    ClassDB::bind_method(D_METHOD("set_use_map_replace", "enabled"), &TrinitySceneLoader::set_use_map_replace);
    ClassDB::bind_method(D_METHOD("get_use_map_replace"), &TrinitySceneLoader::get_use_map_replace);
    ClassDB::bind_method(D_METHOD("set_environment_path", "path"), &TrinitySceneLoader::set_environment_path);
    ClassDB::bind_method(D_METHOD("get_environment_path"), &TrinitySceneLoader::get_environment_path);

    ClassDB::bind_method(D_METHOD("load_scene"), &TrinitySceneLoader::load_scene);
    ClassDB::bind_method(D_METHOD("clear"), &TrinitySceneLoader::clear);
    ClassDB::bind_method(D_METHOD("is_loading"), &TrinitySceneLoader::is_loading);
    ClassDB::bind_method(D_METHOD("get_content"), &TrinitySceneLoader::get_content);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "scene_path", PROPERTY_HINT_FILE, "*.trscn"), "set_scene_path",
                 "get_scene_path");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "story_state"), "set_story_state", "get_story_state");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_map_replace"), "set_use_map_replace", "get_use_map_replace");
    ADD_PROPERTY(
        PropertyInfo(Variant::NODE_PATH, "environment_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "TrinityEnvironment"),
        "set_environment_path", "get_environment_path");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "models_per_frame", PROPERTY_HINT_RANGE, "1,64,1,or_greater"),
                 "set_models_per_frame", "get_models_per_frame");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_load_time_ms", PROPERTY_HINT_RANGE, "0,100,0.1,or_greater,suffix:ms"),
                 "set_max_load_time_ms", "get_max_load_time_ms");
    ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "load_in_editor", PROPERTY_HINT_TOOL_BUTTON, "Load Scene,Reload",
                              PROPERTY_USAGE_EDITOR),
                 "", "get_load_button");

    ADD_SIGNAL(
        MethodInfo("loading_progress", PropertyInfo(Variant::INT, "current"), PropertyInfo(Variant::INT, "total")));
    ADD_SIGNAL(MethodInfo("loading_finished"));
}

Node3D* TrinitySceneLoader::load_scene_file(const String& file, Node3D* parent)
{
    Ref<TRSCN> scene = ResourceLoader::get_singleton()->load(file);
    if (scene.is_null())
    {
        UtilityFunctions::push_error("Failed to load scene file: " + file);
        return nullptr;
    }

    // Each file gets its own container to prevent name collisions.
    Node3D* container = memnew(Node3D);
    container->set_name(scene->get_Name().is_empty() ? file.get_file().get_basename() : scene->get_Name());
    parent->add_child(container);
    set_editor_owner(container);

    Array chunks = scene->get_chunks();
    for (int i = 0; i < chunks.size(); i++)
    {
        load_entry(chunks[i], container, file.get_base_dir(), Ref<TrinityModelInstancerComponent>());
    }
    return container;
}

void TrinitySceneLoader::load_entry(const Ref<TRScene>& entry, Node3D* parent, const String& base_dir,
                                    const Ref<TrinityModelInstancerComponent>& instancer)
{
    if (entry.is_null())
    {
        return;
    }

    const String type = entry->get_Name();
    const Ref<Resource> data = entry->get_nested_type();
    if (type == "SubScene")
    {
        Ref<TRSubScene> sub = data;
        if (sub.is_null())
        {
            return;
        }
        // Story-dependent variants of the same spot; load only the active one.
        const String scene_name = sub->get_FileName().get_file().get_basename();
        if (use_map_replace && map_replace.is_valid() && !map_replace->is_scene_active(scene_name, story_state))
        {
            return;
        }
        // Sub scenes are stored with a _0 suffix: foo.trscn -> foo_0.trscn.
        const String path = base_dir.path_join(sub->get_FileName()).simplify_path();
        String file = path.get_file().get_basename() + "_0";
        if (!path.get_extension().is_empty())
        {
            file += "." + path.get_extension();
        }
        load_scene_file(path.get_base_dir().path_join(file), parent);
        return;
    }
    else if (type == "trinity_ObjectTemplate")
    {
        Ref<TrinityObjectTemplate> object_template = data;
        Ref<TrinitySceneObject> object = object_template.is_valid()
                                             ? Ref<TrinitySceneObject>(object_template->get_EntityData())
                                             : Ref<TrinitySceneObject>();
        if (object.is_valid())
        {
            parent = add_scene_object(
                object, object_template->get_Name().is_empty() ? object->get_Name() : object_template->get_Name(),
                parent);
        }
    }
    else if (type == "trinity_SceneObject")
    {
        Ref<TrinitySceneObject> object = data;
        if (object.is_valid())
        {
            parent = add_scene_object(
                object, object->get_Name().is_empty() ? String("SceneObject") : object->get_Name(), parent);
        }
    }
    else if (type == "trinity_ModelComponent")
    {
        Ref<TrinityModelComponent> model = data;
        if (model.is_valid())
        {
            const String path = GFLib::romfs_path(model->get_FilePath());
            queue.push_back({parent->get_instance_id(), path.get_base_dir(), path.get_file(), instancer});
        }
    }
    else if (type == "trinity_CollisionComponent")
    {
        add_collision(data, parent);
    }
    else if (type == "trinity_LightApplierComponent")
    {
        add_light_applier(data, parent, instancer);
    }

    // The instancer is a sibling and can follow the model in the file.
    Array subs = entry->get_sub_objects();
    Ref<TrinityModelInstancerComponent> model_instancer;
    for (int i = 0; i < subs.size(); i++)
    {
        Ref<TRScene> sub = subs[i];
        Ref<TrinityModelInstancerComponent> found = sub.is_valid()
                                                        ? Ref<TrinityModelInstancerComponent>(sub->get_nested_type())
                                                        : Ref<TrinityModelInstancerComponent>();
        if (found.is_valid())
        {
            model_instancer = found;
        }
    }
    for (int i = 0; i < subs.size(); i++)
    {
        Ref<TRScene> sub = subs[i];
        const bool uses_instancer = sub.is_valid() && (sub->get_Name() == "trinity_ModelComponent" ||
                                                       sub->get_Name() == "trinity_LightApplierComponent");
        load_entry(sub, parent, base_dir, uses_instancer ? model_instancer : Ref<TrinityModelInstancerComponent>());
    }
}

void TrinitySceneLoader::add_light_applier(const Ref<TrinityLightApplierComponent>& component, Node3D* parent,
                                           const Ref<TrinityModelInstancerComponent>& instancer)
{
    if (component.is_null() || component->get_FilePath().is_empty())
    {
        return;
    }
    const String path = GFLib::romfs_path(component->get_FilePath());
    if (!FileAccess::file_exists(path))
    {
        UtilityFunctions::push_warning("Light applier file not found: " + path);
        return;
    }
    Ref<TRLGT> light = ResourceLoader::get_singleton()->load(path);
    if (light.is_null())
    {
        return;
    }

    LightJob job;
    job.parent_id = parent->get_instance_id();
    job.component = component;
    job.light = light;
    job.instanced = component->get_UseInstancerSRT();
    if (!job.instanced)
    {
        light_jobs.push_back(job);
        return;
    }
    if (instancer.is_null())
    {
        UtilityFunctions::push_warning("Light applier requires a ModelInstancerComponent: " + path);
        return;
    }
    Ref<TRINS> instances = ResourceLoader::get_singleton()->load(GFLib::romfs_path(instancer->get_FilePath()));
    if (instances.is_null())
    {
        return;
    }
    const TypedArray<Transform3D> transforms = instances->get_transforms();
    for (int i = 0; i < transforms.size(); ++i)
    {
        job.instance_transform = transforms[i];
        light_jobs.push_back(job);
    }
}

void TrinitySceneLoader::set_environment_path(const NodePath& path)
{
    detach_lights();
    environment_path = path;
    warned_environment = false;
    if (is_inside_tree())
    {
        update_lights(0.0);
    }
}

void TrinitySceneLoader::detach_lights()
{
    if (auto* environment = Object::cast_to<TrinityEnvironment>(ObjectDB::get_instance(light_environment_id)))
    {
        environment->remove_scene_layers(get_instance_id());
    }
    light_environment_id = 0;
    applied_light_layers.clear();
}

void TrinitySceneLoader::update_lights(double delta)
{
    if (!is_inside_tree() || light_jobs.empty())
    {
        return;
    }
    auto* environment = Object::cast_to<TrinityEnvironment>(ObjectDB::get_instance(light_environment_id));
    if (environment && (!environment->is_inside_tree() || environment->get_viewport() != get_viewport()))
    {
        detach_lights();
        environment = nullptr;
    }
    if (!environment)
    {
        if (!environment_path.is_empty())
        {
            environment = Object::cast_to<TrinityEnvironment>(get_node_or_null(environment_path));
        }
        else
        {
            const TypedArray<Node> candidates = get_tree()->get_nodes_in_group("trinity_environments");
            for (int i = 0; i < candidates.size(); ++i)
            {
                auto* candidate = Object::cast_to<TrinityEnvironment>(candidates[i]);
                if (!candidate || candidate->get_viewport() != get_viewport())
                {
                    continue;
                }
                if (environment)
                {
                    if (!warned_environment)
                    {
                        UtilityFunctions::push_warning(
                            "Multiple TrinityEnvironments found; set SceneLoader.environment_path for scene lighting.");
                    }
                    warned_environment = true;
                    return;
                }
                environment = candidate;
            }
        }
        if (!environment || !environment->is_inside_tree() || environment->get_viewport() != get_viewport())
        {
            return; // Retry when an environment enters the tree later.
        }
        light_environment_id = environment->get_instance_id();
        applied_light_layers.clear();
    }

    std::vector<TrinityEnvironment::SceneLayer> next;
    for (size_t i = 0; i < light_jobs.size(); ++i)
    {
        auto& job = light_jobs[i];
        auto* parent = Object::cast_to<Node3D>(ObjectDB::get_instance(job.parent_id));
        if (!parent || !parent->is_inside_tree() || !parent->is_visible_in_tree())
        {
            continue;
        }
        job.elapsed += delta;
        auto& component = job.component;
        TrinityEnvironment::SceneLayer layer;
        layer.light = job.light;
        layer.priority = component->get_Priority();
        const double span = component->get_TimeSpan();
        layer.weight = std::isfinite(span) && span > 0 ? MIN(job.elapsed / span, 1.0) : 1.0;
        const double time = component->get_CurrentTime();
        layer.sample_time = std::isfinite(time) ? MAX(time, 0.0) : 0.0;
        if (component->get_ApplyGameObjectSRT())
        {
            layer.transform = parent->get_global_transform();
        }
        if (job.instanced)
        {
            layer.transform = job.instance_transform * layer.transform;
        }
        if (component->get_UniqueFlag() || job.instanced)
        {
            layer.name_prefix = "scene_" + String::num_uint64(get_instance_id()) + "_" + String::num_uint64(i) + ":";
        }
        next.push_back(layer);
    }
    bool changed = next.size() != applied_light_layers.size();
    for (size_t i = 0; !changed && i < next.size(); ++i)
    {
        const auto& a = next[i];
        const auto& b = applied_light_layers[i];
        changed = a.light != b.light || a.priority != b.priority || a.weight != b.weight ||
                  a.sample_time != b.sample_time || a.transform != b.transform || a.name_prefix != b.name_prefix;
    }
    if (changed)
    {
        applied_light_layers = next;
        environment->set_scene_layers(get_instance_id(), next);
    }
}

Node3D* TrinitySceneLoader::add_scene_object(const Ref<TrinitySceneObject>& object, const String& name, Node3D* parent)
{
    Node3D* node = memnew(Node3D);
    node->set_name(name);
    node->set_transform(object->get_transform());
    if (object->get_keep_world_srt() || !object->get_attach_xform())
    {
        node->set_as_top_level(true);
    }
    parent->add_child(node);
    set_editor_owner(node);
    return node;
}

Ref<ConcavePolygonShape3D> TrinitySceneLoader::load_collision_shape(const String& path)
{
    if (collision_shapes.has(path))
    {
        return collision_shapes[path];
    }

    Ref<TRCOL> trcol = ResourceLoader::get_singleton()->load(path);
    if (trcol.is_null() || trcol->get_tag().is_null())
    {
        UtilityFunctions::push_warning("Failed to load TRCOL: " + path);
        return Ref<ConcavePolygonShape3D>();
    }

    // Physics only needs triangles. Avoid uploading an ArrayMesh to the renderer
    // and then reading its surface arrays back just to build a collision shape.
    const Vector<HavokMeshSection> sections = trcol->get_tag()->GetGeometrySections();
    PackedVector3Array faces;
    int64_t face_vertex_count = 0;
    for (int i = 0; i < sections.size(); ++i)
    {
        if (!sections[i].vertices.is_empty())
        {
            face_vertex_count += (sections[i].faceIndices.size() / 3) * 3;
        }
    }
    faces.resize(face_vertex_count);
    Vector3* output = faces.ptrw();
    int64_t offset = 0;
    for (int i = 0; i < sections.size(); ++i)
    {
        const auto& section = sections[i];
        if (section.vertices.is_empty())
        {
            continue;
        }
        for (int64_t j = 0; j + 2 < section.faceIndices.size(); j += 3)
        {
            output[offset++] = section.vertices[section.faceIndices[j]];
            output[offset++] = section.vertices[section.faceIndices[j + 1]];
            output[offset++] = section.vertices[section.faceIndices[j + 2]];
        }
    }
    if (faces.is_empty())
    {
        return Ref<ConcavePolygonShape3D>();
    }

    Ref<ConcavePolygonShape3D> concave;
    concave.instantiate();
    concave->set_faces(faces);
    collision_shapes[path] = concave;
    return concave;
}

void TrinitySceneLoader::add_collision(const Ref<TrinityCollisionComponent>& component, Node3D* parent)
{
    if (component.is_null())
    {
        return;
    }
    Ref<TrinityBodyCollision> body = component->get_collision();
    Ref<ModelShape> shape = body.is_valid() ? Ref<ModelShape>(body->get_shape()) : Ref<ModelShape>();
    if (shape.is_null())
    {
        return;
    }

    const String path = GFLib::romfs_path(shape->get_path());
    if (!collision_shapes.has(path) && !FileAccess::file_exists(path))
    {
        UtilityFunctions::push_warning("TRCOL file not found: " + path);
        return;
    }
    Ref<ConcavePolygonShape3D> concave = load_collision_shape(path);
    if (concave.is_null())
    {
        return;
    }

    StaticBody3D* static_body = memnew(StaticBody3D);
    static_body->set_name(path.get_file().get_basename());

    const Vector3 scale = shape->get_scale();
    const Vector3 rot = shape->get_rot();
    Transform3D xform = Transform3D().scaled(scale != Vector3() ? scale : Vector3(1, 1, 1));
    if (rot != Vector3())
    {
        xform = Transform3D(Basis::from_euler(rot), Vector3()) * xform;
    }
    xform.origin = shape->get_pos();
    static_body->set_transform(xform);

    parent->add_child(static_body);
    set_editor_owner(static_body);

    CollisionShape3D* col_node = memnew(CollisionShape3D);
    col_node->set_name("CollisionShape");
    col_node->set_shape(concave);
    static_body->add_child(col_node);
    set_editor_owner(col_node);
}

void TrinitySceneLoader::instantiate_model(const ModelJob& job)
{
    Node3D* parent = Object::cast_to<Node3D>(ObjectDB::get_instance(job.parent_id));
    if (!parent)
    {
        return;
    }

    TypedArray<Transform3D> transforms;
    if (job.instancer.is_valid())
    {
        Ref<TRINS> instances = ResourceLoader::get_singleton()->load(GFLib::romfs_path(job.instancer->get_FilePath()));
        if (instances.is_null())
        {
            return;
        }
        transforms = instances->get_transforms();
        // Do not leave a phantom model at the group origin for empty/invalid data.
        if (transforms.is_empty())
        {
            return;
        }
    }

    const String base_name = job.file.get_basename();
    TrinityModel* model = memnew(TrinityModel);
    model->load_model(job.dir, job.file);
    model->set_name(base_name);
    if (job.instancer.is_null())
    {
        parent->add_child(model);
        set_editor_owner(model, true);
        return;
    }

    for (int64_t i = 0; i < transforms.size(); i++)
    {
        Node3D* instance = i == 0 ? model : Object::cast_to<Node3D>(model->duplicate(0));
        instance->set_name(base_name + String("_") + String::num_int64(i));
        // TRINS matrices are world transforms: do not add the group origin again.
        instance->set_as_top_level(true);
        instance->set_transform(transforms[i]);
        parent->add_child(instance);
        set_editor_owner(instance, true);
    }
}

void TrinitySceneLoader::set_editor_owner(Node* node, bool recursive)
{
    if (!Engine::get_singleton()->is_editor_hint() || !is_inside_tree())
    {
        return;
    }
    Node* root = get_tree()->get_edited_scene_root();
    if (!root)
    {
        return;
    }
    node->set_owner(root);
    if (!recursive)
    {
        return;
    }
    for (int i = 0; i < node->get_child_count(); i++)
    {
        set_editor_owner(node->get_child(i), true);
    }
}

void TrinitySceneLoader::_notification(int what)
{
    switch (what)
    {
        case NOTIFICATION_READY:
            set_process(loading || !light_jobs.empty());
            update_lights(0.0);
            break;
        case NOTIFICATION_EXIT_TREE:
            detach_lights();
            request_ready();
            break;
        case NOTIFICATION_EDITOR_PRE_SAVE:
            // The packer skips unowned nodes along with their whole subtree,
            // so content loaded in the editor never ends up in the .tscn.
            if (Node3D* content = get_content())
            {
                content->set_owner(nullptr);
            }
            break;
        case NOTIFICATION_EDITOR_POST_SAVE:
            if (Node3D* content = get_content())
            {
                set_editor_owner(content);
            }
            break;
    }
}

void TrinitySceneLoader::set_max_load_time_ms(double milliseconds)
{
    ERR_FAIL_COND_MSG(!std::isfinite(milliseconds) || milliseconds < 0, "Load time budget must be finite and non-negative.");
    max_load_time_ms = milliseconds;
}

Callable TrinitySceneLoader::get_load_button()
{
    return callable_mp(this, &TrinitySceneLoader::load_scene);
}

void TrinitySceneLoader::load_scene()
{
    clear();
    if (use_map_replace && map_replace.is_null())
    {
        map_replace.instantiate();
        if (map_replace->load(TrinityMapReplace::default_path()) != OK)
        {
            map_replace.unref();
        }
    }
    Node3D* content = load_scene_file(scene_path, this);
    content_id = content ? content->get_instance_id() : 0;
    update_lights(0.0);
    // Finish from _process even with an empty queue so callers can await it.
    loading = true;
    set_process(true);
}

void TrinitySceneLoader::clear()
{
    ++load_generation;
    detach_lights();
    light_jobs.clear();
    collision_shapes.clear();
    queue.clear();
    queue_pos = 0;
    loading = false;
    set_process(false);
    if (Node3D* content = get_content())
    {
        memdelete(content);
    }
    content_id = 0;
}

Node3D* TrinitySceneLoader::get_content() const
{
    return content_id ? Object::cast_to<Node3D>(ObjectDB::get_instance(content_id)) : nullptr;
}

void TrinitySceneLoader::_process(double delta)
{
    update_lights(delta);
    if (!loading)
    {
        return;
    }
    const uint64_t generation = load_generation;
    const size_t total = queue.size();
    const uint64_t started = Time::get_singleton()->get_ticks_usec();
    for (int i = 0; i < models_per_frame && queue_pos < total; i++)
    {
        const ModelJob& job = queue[queue_pos++];
        if (FileAccess::file_exists(job.dir.path_join(job.file)))
        {
            instantiate_model(job);
        }
        emit_signal("loading_progress", (int64_t)queue_pos, (int64_t)total);
        // Progress callbacks may cancel or replace the load, invalidating the queue.
        if (load_generation != generation)
        {
            return;
        }
        // A model conversion is atomic: always allow one job, then yield when
        // the elapsed budget or models_per_frame limit is reached.
        if (max_load_time_ms > 0 &&
            double(Time::get_singleton()->get_ticks_usec() - started) >= max_load_time_ms * 1000.0)
        {
            break;
        }
    }
    if (queue_pos < total)
    {
        return;
    }

    queue.clear();
    queue_pos = 0;
    loading = false;
    set_process(!light_jobs.empty());
    emit_signal("loading_finished");
}
