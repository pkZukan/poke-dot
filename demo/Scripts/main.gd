@tool
extends Node

signal loading_progress(current: int, total: int)

const player_prefab = preload("res://gflib/prefabs/Player.tscn")

@export var run_in_editor: bool = false:
	set(value):
		if value:
			run_in_editor = false
			load_models_async()

func get_trmdl_files_recursive(path: String) -> Array[String]:
	var results: Array[String] = []
	var dir := DirAccess.open(path)

	if dir == null:
		push_error("Failed to access path: " + path)
		return results

	dir.list_dir_begin()
	var file_name := dir.get_next()

	while file_name != "":
		if file_name != "." and file_name != "..":
			var full_path := path.path_join(file_name)

			if dir.current_is_dir():
				results.append_array(get_trmdl_files_recursive(full_path))
			elif file_name.get_extension() == "trmdl":
				results.append(full_path)

		file_name = dir.get_next()

	dir.list_dir_end()
	return results

func _ready() -> void:
	var player = player_prefab.instantiate()
	player.transform = $Spawn.transform
	add_child(player)

func add_suffix_num(path: String, num: int = 0) -> String:
	var file_basename: String = path.get_file().get_basename()
	var file_ext: String = path.get_extension()
	var new_filename: String = file_basename + "_" + str(num)
	
	if not file_ext.is_empty():
		new_filename += "." + file_ext
		
	return path.get_base_dir().path_join(new_filename)


func load_scene(scene: TRScene, parent_node: Node3D, base_path: String = "", load_queue: Array[Dictionary] = []) -> void:
	match scene.Name:
		"SubScene":
			var subscn: TRSubScene = scene.nested_type
			var sub_path: String = base_path.path_join(subscn.FileName).simplify_path()
			sub_path = add_suffix_num(sub_path)
			load_scene_file(sub_path, parent_node, load_queue)
			return

		"trinity_ObjectTemplate":
			var obj_template: TrinityObjectTemplate = scene.nested_type as TrinityObjectTemplate
			if obj_template and obj_template.EntityData is TrinitySceneObject:
				var scn_obj: TrinitySceneObject = obj_template.EntityData as TrinitySceneObject
				var template_node := Node3D.new()
				template_node.name = obj_template.Name if obj_template.Name != "" else scn_obj.Name
				template_node.transform = scn_obj.transform
				
				if scn_obj.keep_world_srt or not scn_obj.attach_xform:
					template_node.top_level = true
				
				parent_node.add_child(template_node)
				parent_node = template_node
				
				if Engine.is_editor_hint():
					template_node.owner = get_tree().edited_scene_root

		"trinity_SceneObject":
			var scn_obj: TrinitySceneObject = scene.nested_type as TrinitySceneObject
			var obj_node := Node3D.new()
			obj_node.name = scn_obj.Name if scn_obj and scn_obj.Name != "" else "SceneObject"
			obj_node.transform = scn_obj.transform
			
			if scn_obj.keep_world_srt or not scn_obj.attach_xform:
				obj_node.top_level = true

			parent_node.add_child(obj_node)
			parent_node = obj_node

			if Engine.is_editor_hint():
				obj_node.owner = get_tree().edited_scene_root

		"trinity_ModelComponent":
			var model_comp: TrinityModelComponent = scene.nested_type as TrinityModelComponent
			var model_path: String = "res://Assets/".path_join(model_comp.FilePath)

			load_queue.append({
				"parent": parent_node,
				"dir": model_path.get_base_dir(),
				"file": model_path.get_file()
			})
		"trinity_CollisionComponent":
			var col_comp: TrinityCollisionComponent = scene.nested_type as TrinityCollisionComponent

			if col_comp.collision is TrinityBodyCollision:
				var col_body: TrinityBodyCollision = col_comp.collision

				if col_body.shape is ModelShape:
					var model_shape: ModelShape = col_body.shape
					var col_path: String = "res://Assets/".path_join(model_shape.path)

					if not FileAccess.file_exists(col_path):
						push_warning("TRCOL file not found: " + col_path)
						return

					var trcol: TRCOL = ResourceLoader.load(col_path)

					if trcol == null:
						push_warning("Failed to load TRCOL: " + col_path)
						return

					var faces: PackedVector3Array = trcol.get_faces()

					var static_body := StaticBody3D.new()
					static_body.name = col_path.get_file().get_basename()

					# Apply ModelShape transform to the collision body.
					var s: Vector3 = model_shape.scale
					var r: Vector3 = model_shape.rot
					var p: Vector3 = model_shape.pos

					var xform := Transform3D()
					xform = xform.scaled(s if s != Vector3.ZERO else Vector3.ONE)

					if r != Vector3.ZERO:
						xform = Transform3D(
							Basis.from_euler(r),
							Vector3.ZERO
						) * xform

					xform.origin = p
					static_body.transform = xform

					parent_node.add_child(static_body)
					if Engine.is_editor_hint():
						static_body.owner = get_tree().edited_scene_root

					var concave := ConcavePolygonShape3D.new()
					concave.set_faces(faces)

					var col_node := CollisionShape3D.new()
					col_node.name = "CollisionMesh"
					col_node.shape = concave

					static_body.add_child(col_node)
					if Engine.is_editor_hint():
						col_node.owner = get_tree().edited_scene_root

	# Recurse into sub_objects with the updated parent_node
	var sub_objs: Array[TRScene] = []
	sub_objs.assign(scene.sub_objects)
	for sub in sub_objs:
		load_scene(sub, parent_node, base_path, load_queue)


func load_scene_file(scene_file: String, parent_node: Node3D, load_queue: Array[Dictionary]) -> void:
	var scene: TRSCN = ResourceLoader.load(scene_file)
	if not scene:
		push_error("Failed to load scene file: " + scene_file)
		return

	# Optional: Create a container Node3D for the subscene to prevent name collisions
	var subscene_container := Node3D.new()
	subscene_container.name = scene.Name if scene.Name != "" else scene_file.get_file().get_basename()
	parent_node.add_child(subscene_container)
	if Engine.is_editor_hint():
		subscene_container.owner = get_tree().edited_scene_root

	var chunks: Array[TRScene] = []
	chunks.assign(scene.chunks)
	for c in chunks:
		load_scene(c, subscene_container, scene_file.get_base_dir(), load_queue)
	
func load_models_async() -> void:
	var load_queue: Array[Dictionary] = []

	var root_scene_node: Node3D = $"."

	load_scene_file(
		"res://Assets/world/ik_scene/field/area/t1/sub_scene/field_/field_0.trscn",
		root_scene_node,
		load_queue
	)

	var total_items: int = load_queue.size()
	if total_items == 0:
		return

	# Load all models async
	for i in range(total_items):
		var job: Dictionary = load_queue[i]
		
		if not is_instance_valid(job["parent"]):
			continue

		if not FileAccess.file_exists(job["dir"].path_join(job["file"])):
			continue
			
		var model = TrinityModel.new()
		model.load_model(job["dir"], job["file"])
		model.name = job["file"].get_basename()
		
		job["parent"].add_child(model)
		if Engine.is_editor_hint():
			model.owner = get_tree().edited_scene_root

		loading_progress.emit(i + 1, total_items)

		if i % 3 == 0:
			await get_tree().process_frame
