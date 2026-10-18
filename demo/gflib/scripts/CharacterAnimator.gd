@tool
class_name TrinityCharacterAnimator
extends AnimationTree

## Drives a locomotion graph (see gflib/animations) for a PokemonCharacter or
## TrainerCharacter: walk/run blending, facing, jump/fall/land states and
## one-shot actions. Each OneShot node "X" in the graph plays the Animation
## node "XAnim" and can be triggered with play_action(&"X").
##
## Flying Pokémon ship a second clip set numbered 2xxxx that mirrors the ground
## set (20030_walk01_loop for 00030_walk01_loop). set_flying() swaps the graph
## onto those clips where they exist.

const LOCOMOTION := &"Locomotion"
const IDLE_VARIATION := &"IdleVariation"
const JUMP_START := &"JumpUpStart"
const JUMP_LOOP := &"JumpUpLoop"
const FALL_START := &"JumpDownStart"
const FALL_LOOP := &"JumpDownLoop"
const LAND := &"Land"

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

# Played inside a OneShot node "X" by convention.
const ONE_SHOT_CLIP_SUFFIX := "Anim"

# Stand-ins for clips a model lacks, matched on the name after the number.
# Mega forms only ship the battle stance.
const ALTERNATE_CLIPS := {
	"defaultwait01_loop": ["battlewait01_loop"],
}
# Clip sets by leading digit: 0xxxx ground, 2xxxx flying, 1xxxx other. Models
# that never touch the ground (Zubat, Gastly, ...) only ship 2xxxx or 1xxxx.
const CLIP_SETS := ["0", "2", "1"]

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
# OneShot node name -> the Animation node it plays.
var _one_shots := {}
var _flying := false
# [AnimationNodeAnimation, ground clip, flight clip] for clips with a flying twin.
var _flight_clips: Array = []
# Locomotion already runs on the flying set: the model has no ground set.
var _always_flying := false

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
	_flying = false
	_flight_clips.clear()
	_always_flying = false
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

	_one_shots.clear()
	for node_name in _graph_nodes(graph):
		var clip_node := StringName(node_name + ONE_SHOT_CLIP_SUFFIX)
		if graph.get_node(node_name) is AnimationNodeOneShot and graph.has_node(clip_node):
			_one_shots[node_name] = clip_node

	_lengths.clear()
	var keys: Array = STATE_INPUT.keys() + _one_shots.keys()
	for key in keys:
		var anim := _available_clip(_anim_name(graph, _one_shots.get(key, key)))
		if anim != &"":
			_lengths[key] = _player.get_animation(anim).length

	# The lowest locomotion point (the standing wait) stands in for any
	# other missing clip.
	var fallback := &""
	if graph.has_node(LOCOMOTION):
		var loco := graph.get_node(LOCOMOTION) as AnimationNodeBlendSpace1D
		var lowest := INF
		for i in loco.get_blend_point_count():
			var clip := loco.get_blend_point_node(i) as AnimationNodeAnimation
			var anim := _available_clip(clip.animation) if clip else &""
			if anim != &"" and loco.get_blend_point_position(i) < lowest:
				lowest = loco.get_blend_point_position(i)
				fallback = anim
	if fallback == &"":
		push_warning("Character has no locomotion animations")
		return

	for node_name in _graph_nodes(graph):
		var node := graph.get_node(node_name)
		if node is AnimationNodeAnimation:
			_resolve_clip(node, fallback)
		elif node is AnimationNodeBlendSpace1D:
			_resolve_blend_space(node)

	for clip in _clip_nodes(graph):
		var flight := _flight_clip_name(clip.animation)
		if flight != &"" and _player.has_animation(flight):
			_flight_clips.append([clip, clip.animation, flight])
	var loco := graph.get_node(LOCOMOTION) as AnimationNodeBlendSpace1D
	for i in loco.get_blend_point_count():
		var clip := loco.get_blend_point_node(i) as AnimationNodeAnimation
		if clip and String(clip.animation).begins_with("2"):
			_always_flying = true

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


func _graph_nodes(graph: AnimationNodeBlendTree) -> Array[StringName]:
	var names: Array[StringName] = []
	for prop in graph.get_property_list():
		var prop_name: String = prop.name
		if prop_name.begins_with("nodes/") and prop_name.ends_with("/node"):
			names.append(StringName(prop_name.get_slice("/", 1)))
	return names


# Every Animation node, including blend space points.
func _clip_nodes(graph: AnimationNodeBlendTree) -> Array[AnimationNodeAnimation]:
	var clips: Array[AnimationNodeAnimation] = []
	for node_name in _graph_nodes(graph):
		var node := graph.get_node(node_name)
		if node is AnimationNodeAnimation:
			clips.append(node)
		elif node is AnimationNodeBlendSpace1D:
			for i in node.get_blend_point_count():
				var clip := node.get_blend_point_node(i) as AnimationNodeAnimation
				if clip:
					clips.append(clip)
	return clips


# 00030_walk01_loop -> 20030_walk01_loop; empty for names outside the 0xxxx set.
func _flight_clip_name(clip: StringName) -> StringName:
	var name := String(clip)
	if name.length() > 6 and name[0] == "0" and name.substr(0, 5).is_valid_int() and name[5] == "_":
		return StringName("2" + name.substr(1))
	return &""


## True when the model has a flying clip set: ground clips with 2xxxx twins, or
## a model that only ships the flying set (Zubat, Beedrill, ...).
func has_flight_animations() -> bool:
	return _always_flying or not _flight_clips.is_empty()


## Switches locomotion onto the flying clip set (where it exists) and keeps the
## character out of the jump/fall states while airborne.
func set_flying(flying: bool) -> void:
	if flying == _flying:
		return
	_flying = flying
	for entry in _flight_clips:
		entry[0].animation = entry[2] if flying else entry[1]
	_cancel_action()
	_landing_remaining = 0.0
	_airborne = false


## OneShot actions available in the current graph, e.g. [&"Roar", &"Attack"].
func get_actions() -> Array:
	return _one_shots.keys()


# The clip itself, else a stand-in from ALTERNATE_CLIPS, preferring the clip's
# own set and then the other sets; empty when the model has none of them.
func _available_clip(clip: StringName) -> StringName:
	if _player.has_animation(clip):
		return clip
	var name := String(clip)
	if name.length() < 7 or name[5] != "_":
		return &""
	var sets: Array = [name[0]]
	for set_digit in CLIP_SETS:
		if set_digit != name[0]:
			sets.append(set_digit)
	var anims := _player.get_animation_list()
	for set_digit in sets:
		var same := StringName(set_digit + name.substr(1))
		if _player.has_animation(same):
			return same
		for alternate in ALTERNATE_CLIPS.get(name.substr(6), []):
			for anim in anims:
				if anim.length() > 6 and anim[0] == set_digit and anim.substr(6) == alternate:
					return StringName(anim)
	return &""


func _resolve_clip(clip: AnimationNodeAnimation, fallback: StringName) -> void:
	if clip == null:
		return
	var anim := _available_clip(clip.animation)
	clip.animation = anim if anim != &"" else fallback


# Missing points take the nearest available point's clip, so a missing wait
# borrows walk rather than run.
func _resolve_blend_space(space: AnimationNodeBlendSpace1D) -> void:
	var available := {}
	for i in space.get_blend_point_count():
		var clip := space.get_blend_point_node(i) as AnimationNodeAnimation
		var anim := _available_clip(clip.animation) if clip else &""
		if anim != &"":
			clip.animation = anim
			available[i] = anim
	for i in space.get_blend_point_count():
		var clip := space.get_blend_point_node(i) as AnimationNodeAnimation
		if clip == null or available.has(i):
			continue
		var nearest := INF
		for j in available:
			var distance := absf(space.get_blend_point_position(j) - space.get_blend_point_position(i))
			if distance < nearest:
				nearest = distance
				clip.animation = available[j]


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
	if not _one_shots.has(action):
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
	if _action_remaining > 0.0 and _one_shots.has(_action_state):
		set(_one_shot_param(_action_state), AnimationNodeOneShot.ONE_SHOT_REQUEST_FADE_OUT)
	_action_remaining = 0.0


func _clip_length(key: StringName) -> float:
	return _lengths.get(key, 0.0)


# Maps a clip key to its Transition input, falling back to ground if missing.
func _available_state(key: StringName) -> StringName:
	return STATE_INPUT[key] if _lengths.has(key) else GROUND


# Called once per physics frame by TrinityCharacterController, after jump
# handling and before native physics.
func update_animation(delta: float) -> void:
	if not active:
		return
	var moving := not _input.is_zero_approx()
	var target := _input.length() * (2.0 if _running else 1.0)
	_blend = move_toward(_blend, target, delta / locomotion_blend_time) if locomotion_blend_time > 0.0 else target
	set(BLEND_PARAM, _blend)

	if moving and (_flying or _character.is_on_floor()) and _action_remaining <= 0.0:
		movement_direction = Vector3(-_input.x, 0, _input.y).normalized()
		_model.rotation.y = lerp_angle(
			_model.rotation.y, atan2(movement_direction.x, movement_direction.z),
			1.0 - exp(-maxf(turn_speed, 0.0) * delta)
		)

	# Flight plays locomotion (hover, fly-walk, fly-run) instead of jump/fall.
	var airborne = not _flying and (not _character.is_on_floor() or _character.velocity.y > 0.0)
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
