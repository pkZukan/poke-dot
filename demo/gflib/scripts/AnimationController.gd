@tool
extends AnimationTree

const LOCOMOTION := &"Locomotion"
const IDLE_VARIATION := &"IdleVariation"
const JUMP_START := &"JumpUpStart"
const JUMP_LOOP := &"JumpUpLoop"
const FALL_START := &"JumpDownStart"
const FALL_LOOP := &"JumpDownLoop"
const LAND := &"Land"
const ROAR := &"Roar"
const ATTACK := &"Attack"

const BLEND_PARAM := &"parameters/Locomotion/blend_position"
const TRANSITION_PARAM := &"parameters/Motion/transition_request"

# Transition input names on the "Motion" node.
const GROUND := &"ground"
const STATE_INPUT := {
	JUMP_START: &"jump_up_start",
	JUMP_LOOP: &"jump_up_loop",
	FALL_START: &"jump_down_start",
	FALL_LOOP: &"jump_down_loop",
	LAND: &"land",
}

# Which Animation node plays inside each OneShot.
const ONE_SHOT_CLIPS := {
	ROAR: &"RoarAnim",
	ATTACK: &"AttackAnim",
	IDLE_VARIATION: &"IdleVariationAnim",
}

@export_range(0.0, 1.0, 0.01) var locomotion_blend_time: float = 0.15
@export_range(0.0, 30.0, 0.1, "or_greater") var turn_speed: float = 12.0
@export_range(0.0, 60.0, 0.1, "or_greater") var idle_variation_delay: float = 6.0

# Camera-forward is local +Z; retain direction while blending to a stop.
var movement_direction := Vector3(0, 0, 1)
var _input := Vector2.ZERO
var _running := false
var _blend := 0.0
var _airborne := false
var _falling := false
var _air_time := 0.0
var _landing_remaining := 0.0
var _action_remaining := 0.0
var _action_interruptible := false
var _action_state: StringName
var _requested_state: StringName
var _idle_elapsed := 0.0
var _character = null
var _model: Node3D
var _player: AnimationPlayer
var _graph_template: AnimationNodeBlendTree

var _lengths := {}

func _ready() -> void:
	_character = get_parent()
	_graph_template = tree_root as AnimationNodeBlendTree
	_character.character_rebuilt.connect(_bind_character)
	_bind_character()


func _anim_name(graph: AnimationNodeBlendTree, node_name: StringName) -> StringName:
	if not graph.has_node(node_name):
		return &""
	var node := graph.get_node(node_name) as AnimationNodeAnimation
	return node.animation if node else &""


func _bind_character() -> void:
	active = false
	_model = _character.get_model()
	_player = _character.get_animation_player()
	_character.configure_animation_tree(self)
	if _player == null or _graph_template == null:
		return
	# Preserve the editable resource in the editor.
	if Engine.is_editor_hint():
		return
	tree_root = _graph_template.duplicate(true)
	var graph := tree_root as AnimationNodeBlendTree

	_lengths.clear()
	var keys: Array = STATE_INPUT.keys() + ONE_SHOT_CLIPS.keys()
	for key in keys:
		var anim := _anim_name(graph, ONE_SHOT_CLIPS.get(key, key))
		if anim != &"" and _player.has_animation(anim):
			_lengths[key] = _player.get_animation(anim).length

	var loco_clips: Array[StringName] = []
	if graph.has_node(LOCOMOTION):
		var loco := graph.get_node(LOCOMOTION) as AnimationNodeBlendSpace1D
		for i in loco.get_blend_point_count():
			var clip := loco.get_blend_point_node(i) as AnimationNodeAnimation
			if clip and _player.has_animation(clip.animation):
				loco_clips.append(clip.animation)
	if loco_clips.is_empty():
		push_warning("Character has no locomotion animations")
		return
	var fallback := loco_clips[0]
	var walk_fallback := loco_clips[1] if loco_clips.size() > 1 else fallback

	for prop in graph.get_property_list():
		var prop_name: String = prop.name
		if not (prop_name.begins_with("nodes/") and prop_name.ends_with("/node")):
			continue
		var node := graph.get_node(StringName(prop_name.get_slice("/", 1)))
		if node is AnimationNodeAnimation:
			_resolve_clip(node, fallback)
		elif node is AnimationNodeBlendSpace1D:
			for i in node.get_blend_point_count():
				_resolve_clip(node.get_blend_point_node(i) as AnimationNodeAnimation, walk_fallback)

	_input = Vector2.ZERO
	_running = false
	movement_direction = Vector3(0, 0, 1)
	_blend = 0.0
	_airborne = false
	_falling = false
	_air_time = 0.0
	_landing_remaining = 0.0
	_action_remaining = 0.0
	_idle_elapsed = 0.0
	_action_state = &""
	_requested_state = &""
	_action_interruptible = false
	active = true


func _resolve_clip(clip: AnimationNodeAnimation, fallback: StringName) -> void:
	if clip != null and not _player.has_animation(clip.animation):
		clip.animation = fallback


func set_movement(input: Vector2, running: bool) -> void:
	_input = input.limit_length() if input.is_finite() else Vector2.ZERO
	_running = running


func play_action(action: StringName, interruptible: bool = false) -> void:
	if not active:
		return
	if not _character.is_on_floor() or _character.velocity.y > 0.0:
		return
	if _action_remaining > 0.0 and not _action_interruptible:
		return
	if not ONE_SHOT_CLIPS.has(action):
		return
	var duration := _clip_length(action)
	if duration <= 0.0:
		return
	_action_state = action
	_action_remaining = duration
	_action_interruptible = interruptible
	_idle_elapsed = 0.0
	set(_one_shot_param(action), AnimationNodeOneShot.ONE_SHOT_REQUEST_FIRE)


func _one_shot_param(action: StringName) -> StringName:
	return StringName("parameters/%s/request" % action)


func _cancel_action() -> void:
	if _action_remaining > 0.0 and ONE_SHOT_CLIPS.has(_action_state):
		set(_one_shot_param(_action_state), AnimationNodeOneShot.ONE_SHOT_REQUEST_FADE_OUT)
	_action_remaining = 0.0


func _clip_length(key: StringName) -> float:
	return _lengths.get(key, 0.0)


# Maps a clip key to its Transition input, falling back to ground if missing.
func _available_state(key: StringName) -> StringName:
	return STATE_INPUT[key] if _lengths.has(key) else GROUND


# Called once by Controller, after input/jump handling and before native physics.
func update_animation(delta: float) -> void:
	if not active:
		return
	var moving := not _input.is_zero_approx()
	var target := _input.length() * (2.0 if _running else 1.0)
	_blend = move_toward(_blend, target, delta / locomotion_blend_time) if locomotion_blend_time > 0.0 else target
	set(BLEND_PARAM, _blend)

	if moving and _character.is_on_floor() and _action_remaining <= 0.0:
		movement_direction = Vector3(-_input.x, 0, _input.y).normalized()
		_model.rotation.y = lerp_angle(
			_model.rotation.y, atan2(movement_direction.x, movement_direction.z),
			1.0 - exp(-maxf(turn_speed, 0.0) * delta)
		)

	var airborne = not _character.is_on_floor() or _character.velocity.y > 0.0
	var falling = _character.velocity.y <= 0.0
	var state := GROUND
	if airborne:
		_air_time = 0.0 if not _airborne or falling != _falling else _air_time + delta
		_cancel_action()
		_landing_remaining = 0.0
		_idle_elapsed = 0.0
		var start := FALL_START if falling else JUMP_START
		var loop := FALL_LOOP if falling else JUMP_LOOP
		state = _available_state(start if _air_time < _clip_length(start) else loop)
	else:
		if _airborne:
			_landing_remaining = _clip_length(LAND)
		if moving:
			_idle_elapsed = 0.0
			if _action_interruptible:
				_cancel_action()
			# Keep the contact pose brief when the player wants to keep moving.
			_landing_remaining = minf(_landing_remaining, 0.12)
		if _action_remaining > 0.0:
			# The one-shot overlays the base state, so the base stays on ground.
			_action_remaining = maxf(0.0, _action_remaining - delta)
		elif _landing_remaining > 0.0:
			state = _available_state(LAND)
			_landing_remaining = maxf(0.0, _landing_remaining - delta)
		elif not moving and idle_variation_delay > 0.0:
			_idle_elapsed += delta
			if _idle_elapsed >= idle_variation_delay:
				play_action(IDLE_VARIATION, true)
				_idle_elapsed = 0.0
	_airborne = airborne
	_falling = falling
	if state != _requested_state:
		set(TRANSITION_PARAM, state)
		_requested_state = state
	advance(delta)
