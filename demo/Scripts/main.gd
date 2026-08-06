@tool
extends Node

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

func _ready():		
	#Outter mountains
	var land_enkei = TrinityModel.new()
	land_enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape1.trmdl")
	$Landscape.add_child(land_enkei)
	var land_enkei2 = TrinityModel.new()
	land_enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape2.trmdl")
	$Landscape.add_child(land_enkei2)
	
	var mtn_enkei = TrinityModel.new()
	mtn_enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains1.trmdl")
	$Mountains.add_child(mtn_enkei)
	var mtn_enkei2 = TrinityModel.new()
	mtn_enkei2.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountains2.trmdl")
	$Mountains.add_child(mtn_enkei2)
	var mtn_range = TrinityModel.new()
	mtn_range.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_mountain_range.trmdl")
	$Mountains.add_child(mtn_range)
	
	#Buildings
	for i in range(1, 25):
		var path: String = "res://Assets/field/model/t1/t1_b/t1_b%02d_1/t1_b%02d_1.trmdl" % [i,i];
		var dir_path: String = path.get_base_dir() 
		var file_name: String = path.get_file()
		if not FileAccess.file_exists(path):
			continue
		var building = TrinityModel.new()
		building.load_model(dir_path, file_name)
		building.name = file_name.get_basename()
		$City/Buildings.add_child(building)
		building.owner = get_tree().edited_scene_root
		
	#Ground
	for path in get_trmdl_files_recursive("res://Assets/field/model/t1/t1_g"):
		var dir_path: String = path.get_base_dir() 
		var file_name: String = path.get_file()
		if file_name.contains("t1_g13_5_replace01"):
			continue
		var ground = TrinityModel.new()
		ground.load_model(dir_path, file_name)
		ground.name = file_name.get_basename()
		$City/Ground.add_child(ground)
		ground.owner = get_tree().edited_scene_root
	
	#Tower
	var prism = TrinityModel.new()
	prism.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_bottom.trmdl")
	$Tower.add_child(prism)
	var prism2 = TrinityModel.new()
	prism2.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_main.trmdl")
	$Tower.add_child(prism2)
	var prism3 = TrinityModel.new()
	prism3.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_top.trmdl")
	$Tower.add_child(prism3)
	var prism4 = TrinityModel.new()
	prism4.load_model("res://Assets/field/model/t1/t1_b/t1_b_prism/", "t1_b_prism_lod.trmdl")
	$Tower.add_child(prism4)
