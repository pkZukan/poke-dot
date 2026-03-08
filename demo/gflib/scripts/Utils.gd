extends Node
class_name Debug

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
