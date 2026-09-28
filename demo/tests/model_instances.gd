# Run: godot --headless --path demo --script res://tests/model_instances.gd
extends SceneTree

const SCENE = "res://Assets/world/ik_scene/field/unity_output/t1_nb1_b03_/t1_nb1_b03_0.trscn"
const BUFFER = "world/ik_scene/field/unity_output/t1_nb1_b03_/ins_root_t1_b_pokemon_center_plantpot_001_X00_Z00.trins"
var failures := 0

func _initialize() -> void:
    call_deferred("run")

func check(ok: bool, message: String) -> void:
    if not ok:
        failures += 1
        push_error(message)

func find_group(entries: Array) -> TRScene:
    for entry in entries:
        for child in entry.sub_objects:
            if child.nested_type is TrinityModelInstancerComponent and child.nested_type.FilePath == BUFFER:
                return entry
        var found = find_group(entry.sub_objects)
        if found:
            return found
    return null

func run() -> void:
    var scene = load(SCENE)
    var group = find_group(scene.chunks)
    check(group != null, "Plant-pot instancer was not parsed")
    if not group:
        quit(1)
        return
    var loader = load("res://Scripts/main.gd").new()
    var container := Node3D.new()
    container.position = Vector3(100, 20, -50)
    root.add_child(container)
    var jobs: Array[Dictionary] = []
    loader.load_scene(group, container, "", jobs)
    check(jobs.size() == 1, "Expected one model job for the plant group")
    if jobs.size() != 1:
        loader.free()
        container.free()
        quit(1)
        return
    var job = jobs[0]
    check(job.instancer != null, "Model job lost its sibling instancer")
    var instances = ResourceLoader.load("res://Assets/" + BUFFER) as TRINS
    check(instances != null, "TRINS resource loader failed")
    if instances == null:
        loader.free()
        container.free()
        quit(1)
        return
    var transforms = instances.transforms
    check(transforms.size() == 2, "Expected two plant pots")
    check(transforms[0].origin.is_equal_approx(Vector3(-485.359863, 0.6, -792.586182)), "Wrong first plant position")
    check(transforms[1].origin.is_equal_approx(Vector3(-478.586792, 0.6, -792.567505)), "Wrong second plant position")
    check(transforms[0].basis.x.is_equal_approx(Vector3(-0.0027584876, 0, 0.9999961853)), "Matrix basis was transposed")
    var before = job.parent.get_child_count()
    loader.instantiate_model_job(job)
    check(job.parent.get_child_count() - before == 2, "Incorrect number of model instances")
    var first = job.parent.get_child(before)
    var second = job.parent.get_child(before + 1)
    check(first.global_transform.is_equal_approx(transforms[0]), "Group transform was applied twice")
    check(second.global_transform.is_equal_approx(transforms[1]), "Second instance placement differs from TRINS")
    var a = first.find_children("*", "MeshInstance3D", true, false)
    var b = second.find_children("*", "MeshInstance3D", true, false)
    check(not a.is_empty() and a.size() == b.size(), "Duplicate lost model meshes")
    for i in mini(a.size(), b.size()):
        check(a[i].mesh == b[i].mesh, "Instances should share mesh resources")
        check(a[i].material_override == b[i].material_override, "Instances should share material resources")
    # Ordinary model components still create one model in parent-local space.
    var plain = Node3D.new()
    container.add_child(plain)
    plain.position = Vector3(3, 4, 5)
    loader.instantiate_model_job({"parent": plain, "dir": job.dir, "file": job.file})
    check(plain.get_child_count() == 1, "Ordinary model did not load")
    check(plain.get_child(0).global_transform.is_equal_approx(plain.global_transform), "Ordinary model lost parent transform")
    loader.free()
    container.free()
    print("Model instance placement failures: ", failures)
    quit(1 if failures else 0)
