class_name TrinityAnimationConverter

static func convert_to_godot_animation(path: String, animFile: String, skl: Skeleton3D, root: NodePath) -> Animation:
	var anim: TRAnimation = ResourceLoader.load(str(path, animFile))
	if not anim:
		push_error("Failed to load animation")
		return
	
	var godot_anim := Animation.new()
	var frame_rate: float = anim.Info.animation_rate
	var key_frames: int = anim.Info.animation_count
	var anim_length: float = key_frames / frame_rate

	godot_anim.length = anim_length
	godot_anim.loop_mode = Animation.LOOP_LINEAR if anim.Info.does_loop else Animation.LOOP_NONE

	for bone_idx in range(skl.get_bone_count()):
		var bone_name := skl.get_bone_name(bone_idx)
		var bt := get_bone_track(anim, bone_name)

		if bt == null:
			continue

		var bone_path := NodePath(str(root) + ":" + bone_name)

		if bt.translate != null:
			var pos_track := godot_anim.add_track(Animation.TYPE_POSITION_3D)
			godot_anim.track_set_path(pos_track, bone_path)
			sample_vector_track(godot_anim, pos_track, bt.translate, frame_rate, key_frames)

		if bt.rotate != null:
			var rot_track := godot_anim.add_track(Animation.TYPE_ROTATION_3D)
			godot_anim.track_set_path(rot_track, bone_path)
			sample_rotation_track(godot_anim, rot_track, bt.rotate, frame_rate, key_frames)

		if bt.scale != null:
			var scale_track := godot_anim.add_track(Animation.TYPE_SCALE_3D)
			godot_anim.track_set_path(scale_track, bone_path)
			sample_vector_track(godot_anim, scale_track, bt.scale, frame_rate, key_frames)

	return godot_anim

static func get_bone_track(anim: TRAnimation, bone_name: String) -> BoneTrack:
	for t in anim.Track.tracks:
		var bt := t as BoneTrack
		if bt and bt.Name == bone_name:
			return bt
	return null

static func sample_vector_track(anim: Animation, track_idx: int, trk: Resource, frame_rate: float, key_frames: int):
	if trk is FixedVectorTrack:
		var value: Vector3 = trk.co
		anim.track_insert_key(track_idx, 0.0, value)
		anim.track_insert_key(track_idx, key_frames / frame_rate, value)

	elif trk is DynamicVectorTrack:
		var total: int = trk.co.size()
		for frame in range(total):
			# Use actual frame index for time; clamp to key_frames boundary
			var time := minf(frame / frame_rate, key_frames / frame_rate)
			anim.track_insert_key(track_idx, time, trk.co[frame])

	elif trk is Framed8VectorTrack or trk is Framed16VectorTrack:
		for i in range(trk.frames.size()):
			var time: float = trk.frames[i] / frame_rate
			anim.track_insert_key(track_idx, time, trk.co[i])

static func sample_rotation_track(anim: Animation, track_idx: int, trk: Resource, frame_rate: float, key_frames: int):
	if trk is FixedRotationTrack:
		var quat: Quaternion = trk.co
		anim.track_insert_key(track_idx, 0.0, quat)
		anim.track_insert_key(track_idx, key_frames / frame_rate, quat)

	elif trk is DynamicRotationTrack:
		var total: int = trk.co.size()
		for frame in range(total):
			var time := minf(frame / frame_rate, key_frames / frame_rate)
			anim.track_insert_key(track_idx, time, trk.co[frame])

	elif trk is Framed8RotationTrack or trk is Framed16RotationTrack:
		for i in range(trk.frames.size()):
			var time: float = trk.frames[i] / frame_rate
			anim.track_insert_key(track_idx, time, trk.co[i])
