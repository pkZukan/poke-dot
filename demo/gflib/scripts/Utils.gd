extends Node
class_name Debug

static func print_bone_tree_compact(skeleton: Skeleton3D) -> void:
	if skeleton == null:
		push_error("print_bone_tree_compact: skeleton is null")
		return
		
	print_rich("[b]%s Bone Hierarchy (%d bones):[/b]" % [skeleton.name, skeleton.get_bone_count()])
	
	var printed = []
	for i in range(skeleton.get_bone_count()):
		if skeleton.get_bone_parent(i) == -1:
			_print_compact_recursive(skeleton, i, "", true, printed)

static func _print_compact_recursive(skeleton: Skeleton3D, idx: int, prefix: String, is_last: bool, printed: Array):
	if idx in printed:
		return
	printed.append(idx)
	
	var connector = "└── " if is_last else "├── "
	var line = prefix + connector + skeleton.get_bone_name(idx) + " [%d]" % idx 
	print(line)
	
	var children = []
	for i in range(skeleton.get_bone_count()):
		if skeleton.get_bone_parent(i) == idx:
			children.append(i)
	
	for i in range(children.size()):
		var ext = "    " if is_last else "│   "
		_print_compact_recursive(skeleton, children[i], prefix + ext, i == children.size() - 1, printed)

static func draw_skeleton(skl: Skeleton3D) -> void:
	for i in range(skl.get_bone_count()):
		var im := ImmediateMesh.new()
		var mi := MeshInstance3D.new()
		var mat := StandardMaterial3D.new()
		mat.albedo_color = Color.YELLOW
		mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		
		var from := skl.get_bone_global_rest(i).origin
		var parent := skl.get_bone_parent(i)
		var to := skl.get_bone_global_rest(parent).origin if parent >= 0 else from + Vector3(0, 0.1, 0)
		
		im.surface_begin(Mesh.PRIMITIVE_LINES)
		im.surface_add_vertex(from)
		im.surface_add_vertex(to)
		im.surface_end()
		
		mi.mesh = im
		mi.material_override = mat
		skl.add_child(mi)
