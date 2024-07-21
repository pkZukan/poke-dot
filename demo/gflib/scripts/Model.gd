extends Node3D

class_name TrinityModel

static func ParseVertexBuffer(attrib:MeshAttrib, verts:PackedByteArray, inds:PackedByteArray, polyType):
	var pos:PackedVector3Array
	var norm:PackedVector3Array
	var uv:PackedVector2Array
	var indicies:PackedInt32Array
	
	var stride = attrib.Stride
	var streamVert = StreamPeerBuffer.new()
	var streamInd = StreamPeerBuffer.new()
	streamVert.data_array = verts
	streamInd.data_array = inds
	
	var currPos = 0
	while currPos < streamVert.get_size():
		var x = 0
		var y = 0
		var z = 0
		var w = 0
		streamVert.seek(currPos + attrib.Descriptor["POSITION"])
		x = streamVert.get_float()
		y = streamVert.get_float()
		z = streamVert.get_float()
		pos.push_back(Vector3(x,y,z))
		
		streamVert.seek(currPos + attrib.Descriptor["NORMAL"])
		x = Utils.half_to_float(streamVert.get_u16())
		y = Utils.half_to_float(streamVert.get_u16())
		z = Utils.half_to_float(streamVert.get_u16())
		w = Utils.half_to_float(streamVert.get_u16())
		norm.push_back(Vector3(x,y,z).normalized())
		
		streamVert.seek(currPos + attrib.Descriptor["TEXCOORD"])
		x = streamVert.get_float()
		y = streamVert.get_float()
		uv.push_back(Vector2(x,y))
		
		currPos += stride
	
	currPos = 0
	while currPos < streamInd.get_size():
		if polyType == 0:
			indicies.push_back(streamInd.get_u8())
		elif polyType == 1:
			indicies.push_back(streamInd.get_u16())
		elif polyType == 2:
			indicies.push_back(streamInd.get_u32())
		elif polyType == 3:
			indicies.push_back(streamInd.get_u64())
		currPos += (1 << polyType)
	
	return {
		Pos = pos,
		Norm = norm,
		UV = uv,
		Indicies = indicies
	}

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
	
		#Load mats
	var Materials:Array
	for matFileInd in range(0, mdl.Materials.size()):
		var material:TRMaterial = ResourceLoader.load(str(path, mdl.Materials[matFileInd]))
		for mat in material.Materials:
			var shaders = mat.Shaders

			var sm:ShaderMaterial = ShaderMaterial.new()
			
			sm.resource_name = mat.Name
			sm.shader = ResourceLoader.load("res://test.gdshader")
			Materials.push_back(sm)
	
	#Load buffer file
	var buff:TRModelBuffer = ResourceLoader.load(str(path, buffName))
	
	#Skeleton
	var skl:Skeleton3D = Skeleton3D.new()
	var skel:TRSkeleton = ResourceLoader.load(str(path, mdl.Skeleton))
	skl.name = "Armature"
	name = skel.TransformNodes[0].Name
	for b in range(0, skel.Bones.size()):
		var bone = skel.Bones[b]
		var transforms = skel.TransformNodes[b]
		skl.add_bone(transforms.Name)
		skl.set_bone_name(b, transforms.Name)
	add_child(skl)
		
	#Iterate through descriptors and buffers
	for d in range(0,descCnt):
		var mi:MeshInstance3D = MeshInstance3D.new()
		var meshShape:MeshShape = mesh.MeshDescriptors[d]
		var vertBuf:PackedByteArray = buff.Buffers[d].VertexBuffers[0]
		var indBuf:PackedByteArray = buff.Buffers[d].IndexBuffers[0]
		var arrMesh:ArrayMesh = ArrayMesh.new()
		
		var result = ParseVertexBuffer(meshShape.Attributes[0], vertBuf, indBuf, meshShape.PolygonType)
		var arr:Array
		arr.resize(Mesh.ARRAY_MAX)
		arr[Mesh.ARRAY_VERTEX] = result.Pos
		arr[Mesh.ARRAY_NORMAL] = result.Norm
		arr[Mesh.ARRAY_TEX_UV] = result.UV
		arr[Mesh.ARRAY_INDEX] = result.Indicies
	
		arrMesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arr)
		
		mi.name = meshShape.Name
		mi.mesh = arrMesh
		mi.material_override = Materials[0]
		mi.skeleton = NodePath(skl.name)
		MeshInstances.push_back(mi)

	for m in MeshInstances:
		add_child(m)
		
func ParseAnimations():
	pass

func Load(path:String, file:String):
	ParseModel(path, file)
	ParseAnimations()
