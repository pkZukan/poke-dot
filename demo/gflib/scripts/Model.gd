extends Node3D
class_name TrinityModel

static func ParseMeshBuffer(accessorTable:VertexAccessors, verts:PackedByteArray, inds:PackedByteArray, polyType:int, start:int, count:int) -> Dictionary:
	var pos := PackedVector3Array()
	var norm := PackedVector3Array()
	var uv := PackedVector2Array()
	var indicies := PackedInt32Array()
	var blendInds := PackedInt32Array()
	var blendWeights := PackedFloat32Array()

	var stride:int = accessorTable.Strides[0].Size
	var streamVert := StreamPeerBuffer.new()
	var streamInd := StreamPeerBuffer.new()
	streamVert.data_array = verts
	streamInd.data_array = inds

	var currPos := 0
	while currPos < streamVert.get_size():
		for attrib:Accessors in accessorTable.Accessors:
			streamVert.seek(currPos + attrib.Position)
			match attrib.Attribute:
				"POSITION":
					pos.push_back(Vector3(streamVert.get_float(), streamVert.get_float(), streamVert.get_float()))
				"NORMAL":
					var x := Utils.half_to_float(streamVert.get_u16())
					var y := Utils.half_to_float(streamVert.get_u16())
					var z := Utils.half_to_float(streamVert.get_u16())
					streamVert.get_u16()
					norm.push_back(Vector3(x, y, z).normalized())
				"TEXCOORD":
					uv.push_back(Vector2(streamVert.get_float(), streamVert.get_float()))
				"BLEND_INDICES":
					blendInds.push_back(streamVert.get_8())
					blendInds.push_back(streamVert.get_8())
					blendInds.push_back(streamVert.get_8())
					blendInds.push_back(streamVert.get_8())
				"BLEND_WEIGHTS":
					var w1 = Utils.half_to_float(streamVert.get_u16())
					var w2 = Utils.half_to_float(streamVert.get_u16())
					var w3 = Utils.half_to_float(streamVert.get_u16())
					var w4 = Utils.half_to_float(streamVert.get_u16())
					var total = w1 + w2 + w3 + w4
					if total > 0.0:
						blendWeights.push_back(w1 / total)
						blendWeights.push_back(w2 / total)
						blendWeights.push_back(w3 / total)
						blendWeights.push_back(w4 / total)
					else:
						blendWeights.append_array([1.0, 0.0, 0.0, 0.0])
		currPos += stride

	var indSize := (1 << polyType)
	currPos = start * indSize
	streamInd.seek(currPos)
	while currPos < (start + count) * indSize:
		match polyType:
			0: indicies.push_back(streamInd.get_u8())
			1: indicies.push_back(streamInd.get_u16())
			2: indicies.push_back(streamInd.get_u32())
			3: indicies.push_back(streamInd.get_u64())
		currPos += indSize

	return { Pos=pos, Norm=norm, UV=uv, Indicies=indicies, BlendInds=blendInds, BlendWeights=blendWeights }

func _load_materials(path:String, materialFiles:Array) -> Dictionary:
	var materials := {}
	for matFile in materialFiles:
		var material:TRMaterial = ResourceLoader.load(str(path, matFile))
		for mat in material.Materials:
			var shdr := _build_shader_material(mat)
			_apply_textures(path, mat, shdr)
			_apply_params(mat, shdr)
			materials[mat.Name] = shdr
	return materials

func _build_shader_material(mat) -> ShaderMaterial:
	var sm := ShaderMaterial.new()
	var shader = mat.Shaders[0]
	sm.resource_name = mat.Name
	sm.shader = ResourceLoader.load(str("res://gflib/shaders/", shader.Name, ".gdshader"))
	for v:ShaderStringParam in shader.StringParams:
		var val:String = v.Value
		if val.is_valid_int():
			sm.set_shader_parameter(v.Name, int(val))
		elif val.to_lower() == "true":
			sm.set_shader_parameter(v.Name, true)
		elif val.to_lower() == "false":
			sm.set_shader_parameter(v.Name, false)
	return sm

func _apply_textures(path:String, mat, shdr:ShaderMaterial) -> void:
	for t in mat.Textures:
		var res = ResourceLoader.load(str(path, t.File), "", ResourceLoader.CACHE_MODE_IGNORE)
		if res == null or res.ImageData == null:
			continue
		var imgTex := ImageTexture.create_from_image(res.ImageData)
		if imgTex:
			shdr.set_shader_parameter(t.Name, imgTex)

func _apply_params(mat, shdr:ShaderMaterial) -> void:
	for p in mat.FloatParams:      shdr.set_shader_parameter(p.Name, p.Value)
	for p in mat.FloatLightParams: shdr.set_shader_parameter(p.Name, p.Value)
	for p in mat.Float4Params:     shdr.set_shader_parameter(p.Name, p.Value)
	for p in mat.IntParams:        shdr.set_shader_parameter(p.Name, p.Value)

func _build_skeleton(skel: TRSkeleton) -> Array:
	var skl := Skeleton3D.new()
	var skin := Skin.new()
	name = skel.TransformNodes[0].Name
	skl.name = skel.TransformNodes[0].Name

	var boneIdx := 0
	var node_to_bone_idx: Dictionary[int, int] = {}
	
	for i in range(skel.TransformNodes.size()):
		var node: TransformNode = skel.TransformNodes[i]

		if node.ParentIndex != -1:
			skl.add_bone(node.Name)
			skl.set_bone_rest(boneIdx, node.Transform)
			node_to_bone_idx[i] = boneIdx

			if node_to_bone_idx.has(node.ParentIndex):
				skl.set_bone_parent(boneIdx, node_to_bone_idx[node.ParentIndex])

			# set skin bind pose using BoneEntry inverse bind matrix
			if node.RigIndex >= 0:
				var bone_entry: BoneEntry = skel.Bones[node.RigIndex]
				
				if bone_entry.InfluenceSkinning > 0:
					skin.add_named_bind(node.Name, bone_entry.Matrix)
			else:
				skin.add_named_bind(node.Name, Transform3D.IDENTITY)
				
			boneIdx += 1

	add_child(skl)
	return [skl, skin]
		
func _build_meshes(path:String, mesh:TRMesh, buff:TRModelBuffer, materials:Dictionary, skl:Skeleton3D, skin:Skin) -> void:
	for d in range(mesh.MeshDescriptors.size()):
		var meshShape:MeshShape = mesh.MeshDescriptors[d]
		var vertBuf:PackedByteArray = buff.Buffers[d].VertexBuffers[0]
		var indBuf:PackedByteArray = buff.Buffers[d].IndexBuffers[0]

		for subMesh in range(meshShape.Materials.size()):
			var mat = meshShape.Materials[subMesh]
			var result := ParseMeshBuffer(meshShape.Attributes[0], vertBuf, indBuf, meshShape.PolygonType, mat.PolyOffset, mat.PolyCount)

			var arr := Array()
			arr.resize(Mesh.ARRAY_MAX)
			arr[Mesh.ARRAY_VERTEX]  = result.Pos
			arr[Mesh.ARRAY_NORMAL]  = result.Norm
			arr[Mesh.ARRAY_TEX_UV]  = result.UV
			arr[Mesh.ARRAY_INDEX]   = result.Indicies
			arr[Mesh.ARRAY_BONES]   = result.BlendInds
			arr[Mesh.ARRAY_WEIGHTS] = result.BlendWeights

			var arrMesh := ArrayMesh.new()
			arrMesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arr)

			var mi := MeshInstance3D.new()
			mi.skin = skin
			mi.name = str(meshShape.Name, "_", mat.MaterialName)
			mi.mesh = arrMesh
			mi.material_override = materials.get(mat.MaterialName)
			mi.skeleton = NodePath("../" + skl.name)
			add_child(mi)
		
func load_model(path:String, file:String) -> void:
	var mdl:TRModel = ResourceLoader.load(str(path, file))
	var mesh:TRMesh = ResourceLoader.load(str(path, mdl.Meshes[0]))
	var buff:TRModelBuffer = ResourceLoader.load(str(path, mesh.BufferName))
	var skel:TRSkeleton = ResourceLoader.load(str(path, mdl.Skeleton))
	var materials := _load_materials(path, mdl.Materials)
	var skel_result := _build_skeleton(skel)
	#Debug.print_bone_tree_compact(skel_result[0])
	#Debug.draw_skeleton(skel_result[0])
	_build_meshes(path, mesh, buff, materials, skel_result[0], skel_result[1])
