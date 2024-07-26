extends Node3D

class_name TrinityModel

static func ParseVertexBuffer(accessorTable:VertexAccessors, verts:PackedByteArray, inds:PackedByteArray, polyType, start, count):
	var pos:PackedVector3Array = []
	var norm:PackedVector3Array = []
	var uv:PackedVector2Array = []
	var indicies:PackedInt32Array = []
	var blendInds:PackedInt32Array = []
	var blendWeights:PackedFloat32Array = []
	
	var stride = accessorTable.Strides[0].Size
	
	var streamVert = StreamPeerBuffer.new()
	var streamInd = StreamPeerBuffer.new()
	streamVert.data_array = verts
	streamInd.data_array = inds
	
	#Parse Vertex buffer
	var currPos = 0
	while currPos < streamVert.get_size():
		for attrib:Accessors in accessorTable.Accessors:
			streamVert.seek(currPos + attrib.Position)
			if attrib.Attribute == "POSITION":
				var x = streamVert.get_float()
				var y = streamVert.get_float()
				var z = streamVert.get_float()
				pos.push_back(Vector3(x,y,z))

			if attrib.Attribute == "NORMAL":
				var x = Utils.half_to_float(streamVert.get_u16())
				var y = Utils.half_to_float(streamVert.get_u16())
				var z = Utils.half_to_float(streamVert.get_u16())
				var w = Utils.half_to_float(streamVert.get_u16())
				norm.push_back(Vector3(x,y,z).normalized())
			
			if attrib.Attribute == "TEXCOORD":
				var x = streamVert.get_float()
				var y = streamVert.get_float()
				uv.push_back(Vector2(x,y))
			
			if attrib.Attribute == "BLEND_INDICES":
				blendInds.push_back(streamVert.get_u8())
				blendInds.push_back(streamVert.get_u8())
				blendInds.push_back(streamVert.get_u8())
				blendInds.push_back(streamVert.get_u8())
		
			if attrib.Attribute == "BLEND_WEIGHTS":
				blendWeights.push_back(Utils.half_to_float(streamVert.get_u16()))
				blendWeights.push_back(Utils.half_to_float(streamVert.get_u16()))
		
		currPos += stride
	
	#Parse Index buffer
	var indsize = (1 << polyType)
	currPos = start * indsize
	streamInd.seek(currPos)
	while currPos < ((start + count) * indsize):
		if polyType == 0:
			indicies.push_back(streamInd.get_u8())
		elif polyType == 1:
			indicies.push_back(streamInd.get_u16())
		elif polyType == 2:
			indicies.push_back(streamInd.get_u32())
		elif polyType == 3:
			indicies.push_back(streamInd.get_u64())
		currPos += indsize
	
	return {
		Pos = pos,
		Norm = norm,
		UV = uv,
		Indicies = indicies,
		BlendInds = blendInds,
		BlendWeights = blendWeights
	}

func GetShaderMat(shaderMat) -> ShaderMaterial:
	var sm:ShaderMaterial = ShaderMaterial.new()
	
	var shader = shaderMat.Shaders[0]
	var name = shader.Name
	var shdr = ResourceLoader.load(str("res://gflib/shaders/", shader.Name, ".gdshader"))

	for v:ShaderStringParam in shader.StringParams:
		var val = v.Value
		#Is there a better way to do this crap?
		if val.is_valid_int():
			sm.set_shader_parameter(v.Name, int(val))
		elif val.to_lower() == "true":
			sm.set_shader_parameter(v.Name, true)
		elif val.to_lower() == "false":
			sm.set_shader_parameter(v.Name, false)
	
	sm.resource_name = shaderMat.Name
	sm.shader = shdr
	return sm

func ParseModel(path:String, file:String):
	#Create array for meshes
	var MeshInstances:Array
	
	#Load model file that points to meshes/skel/etc
	var mdl:TRModel = ResourceLoader.load(str(path, file))
	
	var mainMesh = mdl.Meshes[0]; #rest are LODs
	
	#Load LOD0 mesh and get its mesh descriptors
	var mesh:TRMesh = ResourceLoader.load(str(path, mainMesh))
	var descCnt = mesh.MeshDescriptors.size()
	var buffName = mesh.BufferName
	
	#Load materials (map mat name to shader instance)
	var Materials:Dictionary
	for matFileInd in range(0, mdl.Materials.size()):
		var material:TRMaterial = ResourceLoader.load(str(path, mdl.Materials[matFileInd]))
		for mat in material.Materials:
			var shdr = GetShaderMat(mat)
			
			#Iterate though the textures and map uniform names to image data
			#TODO: seperate image loading code and optimize it to not load the same bntx twice
			var textures:Dictionary;
			for t in mat.Textures:
				textures[t.Name] = ResourceLoader.load(str(path, t.File))
			for t in textures:
				shdr.set_shader_parameter(t, ImageTexture.create_from_image(textures[t].ImageData))
			
			Materials[mat.Name] = shdr
	
	#Load buffer file
	var buff:TRModelBuffer = ResourceLoader.load(str(path, buffName))
	
	#Skeleton
	var skl:Skeleton3D = Skeleton3D.new()
	var skel:TRSkeleton = ResourceLoader.load(str(path, mdl.Skeleton))
	skl.name = "Armature"
	name = skel.TransformNodes[0].Name
	for b in range(0, skel.TransformNodes.size()):
		var transforms = skel.TransformNodes[b]
		skl.add_bone(transforms.Name)
		skl.set_bone_name(b, transforms.Name)
		skl.transform = transforms.Transform
		skl.set_bone_parent(b, transforms.ParentIndex)
	add_child(skl)
		
	#Iterate through descriptors and buffers
	for d in range(0,descCnt):
		var meshShape:MeshShape = mesh.MeshDescriptors[d]
		var vertBuf:PackedByteArray = buff.Buffers[d].VertexBuffers[0]
		var indBuf:PackedByteArray = buff.Buffers[d].IndexBuffers[0]
		
		#Create meshes per material info
		for subMesh in range(0, meshShape.Materials.size()):
			var arrMesh:ArrayMesh = ArrayMesh.new()
			var mi:MeshInstance3D = MeshInstance3D.new()
			var mat = meshShape.Materials[subMesh]
			var result = ParseVertexBuffer(meshShape.Attributes[0], vertBuf, indBuf, meshShape.PolygonType, mat.PolyOffset, mat.PolyCount)
			var arr:Array
			arr.resize(Mesh.ARRAY_MAX)
			arr[Mesh.ARRAY_VERTEX] = result.Pos
			arr[Mesh.ARRAY_NORMAL] = result.Norm
			arr[Mesh.ARRAY_TEX_UV] = result.UV
			arr[Mesh.ARRAY_INDEX] = result.Indicies
			#arr[Mesh.ARRAY_BONES] = result.BlendInds
			#arr[Mesh.ARRAY_WEIGHTS] = result.BlendWeights
			
			var flags = 0
			flags |= ArrayMesh.ARRAY_FORMAT_VERTEX
			flags |= ArrayMesh.ARRAY_FORMAT_NORMAL
			flags |= ArrayMesh.ARRAY_FORMAT_TEX_UV
			flags |= ArrayMesh.ARRAY_FORMAT_BONES
			flags |= ArrayMesh.ARRAY_FORMAT_WEIGHTS

			arrMesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arr, [], {}, flags)

			mi.name = str(meshShape.Name, "_", mat.MaterialName)
			mi.mesh = arrMesh
			mi.material_override = Materials[mat.MaterialName]
			mi.skeleton = NodePath("../" + skl.name)
			MeshInstances.push_back(mi)

	for m in MeshInstances:
		add_child(m)
		
func ParseAnimations():
	pass

func Load(path:String, file:String):
	ParseModel(path, file)
	ParseAnimations()
