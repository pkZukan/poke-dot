@tool
extends Node

signal loading_progress(current: int, total: int)

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
		# Skip hidden self/parent directory references
		if file_name != "." and file_name != "..":
			var full_path := path.path_join(file_name)

			if dir.current_is_dir():
				# Recursively search subdirectories
				results.append_array(get_trmdl_files_recursive(full_path))
			elif file_name.get_extension() == "trmdl":
				results.append(full_path)

		file_name = dir.get_next()

	dir.list_dir_end()
	return results

func _ready() -> void:
	pass

func load_models_async() -> void:
	var load_queue: Array[Dictionary] = []

	# Outer mountains
	load_queue.append({"parent": $Landscape, "dir": "res://Assets/field/model/env/enkei/enkei1", "file": "enkei_landscape1.trmdl"})
	load_queue.append({"parent": $Landscape, "dir": "res://Assets/field/model/env/enkei/enkei1", "file": "enkei_landscape2.trmdl"})
	load_queue.append({"parent": $Mountains, "dir": "res://Assets/field/model/env/enkei/enkei1", "file": "enkei_mountains1.trmdl"})
	load_queue.append({"parent": $Mountains, "dir": "res://Assets/field/model/env/enkei/enkei1", "file": "enkei_mountains2.trmdl"})
	load_queue.append({"parent": $Mountains, "dir": "res://Assets/field/model/env/enkei/enkei1", "file": "enkei_mountain_range.trmdl"})

	# Buildings
	for i in range(1, 25):
		var path: String = "res://Assets/field/model/t1/t1_b/t1_b%02d_1/t1_b%02d_1.trmdl" % [i, i]
		if FileAccess.file_exists(path):
			load_queue.append({"parent": $City/Buildings, "dir": path.get_base_dir(), "file": path.get_file()})

	# Ground
	for path in get_trmdl_files_recursive("res://Assets/field/model/t1/t1_g"):
		var file_name: String = path.get_file()
		if not file_name.contains("t1_g13_5_replace01"):
			load_queue.append({"parent": $City/Ground, "dir": path.get_base_dir(), "file": file_name})

	# Tower
	load_queue.append({"parent": $Tower, "dir": "res://Assets/field/model/t1/t1_b/t1_b_prism/", "file": "t1_b_prism_bottom.trmdl"})
	load_queue.append({"parent": $Tower, "dir": "res://Assets/field/model/t1/t1_b/t1_b_prism/", "file": "t1_b_prism_main.trmdl"})
	load_queue.append({"parent": $Tower, "dir": "res://Assets/field/model/t1/t1_b/t1_b_prism/", "file": "t1_b_prism_top.trmdl"})
	load_queue.append({"parent": $Tower, "dir": "res://Assets/field/model/t1/t1_b/t1_b_prism/", "file": "t1_b_prism_lod.trmdl"})

	var total_items: int = load_queue.size()

	# Iterate through and instantiate models with frame yielding
	for i in range(total_items):
		var job: Dictionary = load_queue[i]
		var model = TrinityModel.new()
		model.load_model(job["dir"], job["file"])
		model.name = job["file"].get_basename()
		
		job["parent"].add_child(model)

		loading_progress.emit(i + 1, total_items)

		# Yield control every 3 models so UI can redraw and progress bar updates smoothly
		if i % 3 == 0:
			await get_tree().process_frame
