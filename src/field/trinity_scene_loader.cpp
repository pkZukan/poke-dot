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
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TrinitySceneLoader::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_scene_path", "path"), &TrinitySceneLoader::set_scene_path);
    ClassDB::bind_method(D_METHOD("get_scene_path"), &TrinitySceneLoader::get_scene_path);
    ClassDB::bind_method(D_METHOD("set_models_per_frame", "count"), &TrinitySceneLoader::set_models_per_frame);
    ClassDB::bind_method(D_METHOD("get_models_per_frame"), &TrinitySceneLoader::get_models_per_frame);
    ClassDB::bind_method(D_METHOD("get_load_button"), &TrinitySceneLoader::get_load_button);

    ClassDB::bind_method(D_METHOD("load_scene"), &TrinitySceneLoader::load_scene);
    ClassDB::bind_method(D_METHOD("clear"), &TrinitySceneLoader::clear);
    ClassDB::bind_method(D_METHOD("is_loading"), &TrinitySceneLoader::is_loading);
    ClassDB::bind_method(D_METHOD("get_content"), &TrinitySceneLoader::get_content);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "scene_path", PROPERTY_HINT_FILE, "*.trscn"), "set_scene_path", "get_scene_path");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "models_per_frame", PROPERTY_HINT_RANGE, "1,64,1,or_greater"), "set_models_per_frame", "get_models_per_frame");
    ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "load_in_editor", PROPERTY_HINT_TOOL_BUTTON, "Load Scene,Reload", PROPERTY_USAGE_EDITOR), "", "get_load_button");

    ADD_SIGNAL(MethodInfo("loading_progress", PropertyInfo(Variant::INT, "current"), PropertyInfo(Variant::INT, "total")));
    ADD_SIGNAL(MethodInfo("loading_finished"));
}

void TrinitySceneLoader::_notification(int what)
{
    switch (what)
    {
        case NOTIFICATION_READY:
            // Overriding _process turns processing on; only run while loading.
            set_process(loading);
            break;
        case NOTIFICATION_EDITOR_PRE_SAVE:
            // The packer skips unowned nodes along with their whole subtree,
            // so content loaded in the editor never ends up in the .tscn.
            if (Node3D *content = get_content())
                content->set_owner(nullptr);
            break;
        case NOTIFICATION_EDITOR_POST_SAVE:
            if (Node3D *content = get_content())
                set_editor_owner(content);
            break;
    }
}

Callable TrinitySceneLoader::get_load_button()
{
    return callable_mp(this, &TrinitySceneLoader::load_scene);
}

Node3D *TrinitySceneLoader::get_content() const
{
    return content_id ? Object::cast_to<Node3D>(ObjectDB::get_instance(content_id)) : nullptr;
}

void TrinitySceneLoader::set_editor_owner(Node *node, bool recursive)
{
    if (!Engine::get_singleton()->is_editor_hint() || !is_inside_tree())
        return;
    Node *root = get_tree()->get_edited_scene_root();
    if (!root)
        return;
    node->set_owner(root);
    if (!recursive)
        return;
    for (int i = 0; i < node->get_child_count(); i++)
        set_editor_owner(node->get_child(i), true);
}

void TrinitySceneLoader::clear()
{
    queue.clear();
    queue_pos = 0;
    loading = false;
    set_process(false);
    if (Node3D *content = get_content())
        memdelete(content);
    content_id = 0;
}

void TrinitySceneLoader::load_scene()
{
    // Reloading replaces the previous content instead of stacking a second copy.
    clear();
    Node3D *content = load_scene_file(scene_path, this);
    content_id = content ? content->get_instance_id() : 0;
    // Finish from _process even with an empty queue so callers can await it.
    loading = true;
    set_process(true);
}

void TrinitySceneLoader::_process(double delta)
{
    const size_t total = queue.size();
    for (int i = 0; i < models_per_frame && queue_pos < total; i++)
    {
        const ModelJob &job = queue[queue_pos++];
        if (FileAccess::file_exists(job.dir.path_join(job.file)))
            instantiate_model(job);
        emit_signal("loading_progress", (int64_t)queue_pos, (int64_t)total);
    }
    if (queue_pos < total)
        return;

    queue.clear();
    queue_pos = 0;
    loading = false;
    set_process(false);
    emit_signal("loading_finished");
}

Node3D *TrinitySceneLoader::load_scene_file(const String &file, Node3D *parent)
{
    Ref<TRSCN> scene = ResourceLoader::get_singleton()->load(file);
    if (scene.is_null())
    {
        UtilityFunctions::push_error("Failed to load scene file: " + file);
        return nullptr;
    }

    // Each file gets its own container to prevent name collisions.
    Node3D *container = memnew(Node3D);
    container->set_name(scene->get_Name().is_empty() ? file.get_file().get_basename() : scene->get_Name());
    parent->add_child(container);
    set_editor_owner(container);

    Array chunks = scene->get_chunks();
    for (int i = 0; i < chunks.size(); i++)
        load_entry(chunks[i], container, file.get_base_dir(), Ref<TrinityModelInstancerComponent>());
    return container;
}

void TrinitySceneLoader::load_entry(const Ref<TRScene> &entry, Node3D *parent, const String &base_dir, const Ref<TrinityModelInstancerComponent> &instancer)
{
    if (entry.is_null())
        return;

    const String type = entry->get_Name();
    const Ref<Resource> data = entry->get_nested_type();
    if (type == "SubScene")
    {
        Ref<TRSubScene> sub = data;
        if (sub.is_null())
            return;
        // Sub scenes are stored with a _0 suffix: foo.trscn -> foo_0.trscn.
        const String path = base_dir.path_join(sub->get_FileName()).simplify_path();
        String file = path.get_file().get_basename() + "_0";
        if (!path.get_extension().is_empty())
            file += "." + path.get_extension();
        load_scene_file(path.get_base_dir().path_join(file), parent);
        return;
    }
    else if (type == "trinity_ObjectTemplate")
    {
        Ref<TrinityObjectTemplate> object_template = data;
        Ref<TrinitySceneObject> object = object_template.is_valid() ? Ref<TrinitySceneObject>(object_template->get_EntityData()) : Ref<TrinitySceneObject>();
        if (object.is_valid())
            parent = add_scene_object(object, object_template->get_Name().is_empty() ? object->get_Name() : object_template->get_Name(), parent);
    }
    else if (type == "trinity_SceneObject")
    {
        Ref<TrinitySceneObject> object = data;
        if (object.is_valid())
            parent = add_scene_object(object, object->get_Name().is_empty() ? String("SceneObject") : object->get_Name(), parent);
    }
    else if (type == "trinity_ModelComponent")
    {
        Ref<TrinityModelComponent> model = data;
        if (model.is_valid())
        {
            const String path = GFLib::romfs_path(model->get_FilePath());
            queue.push_back({ parent->get_instance_id(), path.get_base_dir(), path.get_file(), instancer });
        }
    }
    else if (type == "trinity_CollisionComponent")
    {
        add_collision(data, parent);
    }

    // The instancer is a sibling and can follow the model in the file.
    Array subs = entry->get_sub_objects();
    Ref<TrinityModelInstancerComponent> model_instancer;
    for (int i = 0; i < subs.size(); i++)
    {
        Ref<TRScene> sub = subs[i];
        Ref<TrinityModelInstancerComponent> found = sub.is_valid() ? Ref<TrinityModelInstancerComponent>(sub->get_nested_type()) : Ref<TrinityModelInstancerComponent>();
        if (found.is_valid())
            model_instancer = found;
    }
    for (int i = 0; i < subs.size(); i++)
    {
        Ref<TRScene> sub = subs[i];
        const bool is_model = sub.is_valid() && sub->get_Name() == "trinity_ModelComponent";
        load_entry(sub, parent, base_dir, is_model ? model_instancer : Ref<TrinityModelInstancerComponent>());
    }
}

Node3D *TrinitySceneLoader::add_scene_object(const Ref<TrinitySceneObject> &object, const String &name, Node3D *parent)
{
    Node3D *node = memnew(Node3D);
    node->set_name(name);
    node->set_transform(object->get_transform());
    if (object->get_keep_world_srt() || !object->get_attach_xform())
        node->set_as_top_level(true);
    parent->add_child(node);
    set_editor_owner(node);
    return node;
}

void TrinitySceneLoader::add_collision(const Ref<TrinityCollisionComponent> &component, Node3D *parent)
{
    if (component.is_null())
        return;
    Ref<TrinityBodyCollision> body = component->get_collision();
    Ref<ModelShape> shape = body.is_valid() ? Ref<ModelShape>(body->get_shape()) : Ref<ModelShape>();
    if (shape.is_null())
        return;

    const String path = GFLib::romfs_path(shape->get_path());
    if (!FileAccess::file_exists(path))
    {
        UtilityFunctions::push_warning("TRCOL file not found: " + path);
        return;
    }
    Ref<TRCOL> trcol = ResourceLoader::get_singleton()->load(path);
    if (trcol.is_null())
    {
        UtilityFunctions::push_warning("Failed to load TRCOL: " + path);
        return;
    }
    Ref<ArrayMesh> mesh = trcol->get_mesh();
    if (mesh.is_null())
    {
        UtilityFunctions::push_warning("Failed to create TRCOL mesh: " + path);
        return;
    }

    StaticBody3D *static_body = memnew(StaticBody3D);
    static_body->set_name(path.get_file().get_basename());

    const Vector3 scale = shape->get_scale();
    const Vector3 rot = shape->get_rot();
    Transform3D xform = Transform3D().scaled(scale != Vector3() ? scale : Vector3(1, 1, 1));
    if (rot != Vector3())
        xform = Transform3D(Basis::from_euler(rot), Vector3()) * xform;
    xform.origin = shape->get_pos();
    static_body->set_transform(xform);

    parent->add_child(static_body);
    set_editor_owner(static_body);

    PackedVector3Array faces;
    for (int s = 0; s < mesh->get_surface_count(); s++)
    {
        Array arrays = mesh->surface_get_arrays(s);
        PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
        PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];
        if (indices.is_empty())
        {
            faces.append_array(vertices);
            continue;
        }
        for (int64_t i = 0; i + 2 < indices.size(); i += 3)
        {
            faces.push_back(vertices[indices[i]]);
            faces.push_back(vertices[indices[i + 1]]);
            faces.push_back(vertices[indices[i + 2]]);
        }
    }
    if (faces.is_empty())
        return;

    Ref<ConcavePolygonShape3D> concave;
    concave.instantiate();
    concave->set_faces(faces);

    CollisionShape3D *col_node = memnew(CollisionShape3D);
    col_node->set_name("CollisionShape");
    col_node->set_shape(concave);
    static_body->add_child(col_node);
    set_editor_owner(col_node);
}

void TrinitySceneLoader::instantiate_model(const ModelJob &job)
{
    Node3D *parent = Object::cast_to<Node3D>(ObjectDB::get_instance(job.parent_id));
    if (!parent)
        return;

    TypedArray<Transform3D> transforms;
    if (job.instancer.is_valid())
    {
        Ref<TRINS> instances = ResourceLoader::get_singleton()->load(GFLib::romfs_path(job.instancer->get_FilePath()));
        if (instances.is_null())
            return;
        transforms = instances->get_transforms();
        // Do not leave a phantom model at the group origin for empty/invalid data.
        if (transforms.is_empty())
            return;
    }

    const String base_name = job.file.get_basename();
    TrinityModel *model = memnew(TrinityModel);
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
        // Duplicate nodes while sharing meshes, materials and textures.
        Node3D *instance = i == 0 ? model : Object::cast_to<Node3D>(model->duplicate(0));
        instance->set_name(base_name + String("_") + String::num_int64(i));
        // TRINS matrices are world transforms: do not add the group origin again.
        instance->set_as_top_level(true);
        instance->set_transform(transforms[i]);
        parent->add_child(instance);
        set_editor_owner(instance, true);
    }
}
