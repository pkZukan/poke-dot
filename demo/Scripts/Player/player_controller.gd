extends TrinityCharacterController

## Demo player: keyboard/mouse and joypad input with an orbiting camera.

const EvolutionEffect := preload("res://Scripts/Player/evolution_effect.gd")
const MegaEvolutionEffect := preload("res://Scripts/Player/mega_evolution_effect.gd")

@onready var camera_pivot: Node3D = $CameraPivot
@onready var camera: Camera3D = $CameraPivot/Camera3D

@export_category("Control sensitivity")
@export var mouse_sensitivity: float = 0.2
@export var joy_sensitivity: float = 120.0  # degrees per second at full tilt

var _air_time := 0.0
var _evolution_effect: EvolutionEffect
var _mega_effect: MegaEvolutionEffect

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	if character.has_signal("character_rebuilt"):
		character.character_rebuilt.connect(_check_isflying)
	_check_isflying()
	if character is PokemonCharacter:
		_evolution_effect = EvolutionEffect.new()
		_evolution_effect.name = "EvolutionEffect"
		add_child(_evolution_effect)
		_mega_effect = MegaEvolutionEffect.new()
		_mega_effect.name = "MegaEvolutionEffect"
		add_child(_mega_effect)

func evolve_with_effect(index: int = 0) -> bool:
	if _mega_effect and _mega_effect.is_evolving:
		return false
	return _evolution_effect.start(index) if _evolution_effect else false

## Toggle Mega form; index selects among alternatives such as Charizard X/Y.
func mega_evolve_with_effect(index: int = 0) -> bool:
	if _evolution_effect and _evolution_effect.is_evolving:
		return false
	return _mega_effect.start(index) if _mega_effect else false

func _physics_process(delta: float) -> void:
	if _evolution_effect:
		if Input.is_action_just_pressed("mega_evolve"):
			mega_evolve_with_effect(1 if Input.is_key_pressed(KEY_SHIFT) else 0)
		if Input.is_action_just_pressed("evolve"):
			evolve_with_effect()
		if _evolution_effect.is_evolving or (_mega_effect and _mega_effect.is_evolving):
			move_input = Vector2.ZERO
			running = false
			fly_vertical = 0.0
			character.velocity = Vector3.ZERO
			animator.set_movement(Vector2.ZERO, false)
			animator.update_animation(delta)
			return
	super._physics_process(delta)

func _check_isflying() -> void:
	# Anything with a flying animation set can take off by jumping again in
	# mid-air. The animator re-binds on character_rebuilt before this runs.
	can_fly = animator.has_flight_animations()

func _input(event: InputEvent) -> void:
	# mouse look
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		_apply_look(
			event.screen_relative.x * mouse_sensitivity,
			event.screen_relative.y * mouse_sensitivity
		)

func _apply_look(yaw_deg: float, pitch_deg: float) -> void:
	character.rotate_y(deg_to_rad(-yaw_deg))
	camera_pivot.rotation.x = clampf(
		camera_pivot.rotation.x - deg_to_rad(pitch_deg),
		deg_to_rad(-80.0), deg_to_rad(80.0)
	)

func _update_intent(delta: float) -> void:
	_air_time = 0.0 if character.is_on_floor() else _air_time + delta

	# joypad look
	var look := Input.get_vector("look_left", "look_right", "look_up", "look_down")
	if look != Vector2.ZERO:
		_apply_look(look.x * joy_sensitivity * delta, look.y * joy_sensitivity * delta)

	move_input = Input.get_vector("strafe_left", "strafe_right", "move_back", "move_forward")
	running = Input.is_action_pressed("run")
	# Pushing forward climbs or dives along the camera's pitch.
	fly_vertical = -camera.global_basis.z.y * move_input.y

	if Input.is_action_just_pressed("jump"):
		if is_flying():
			stop_flying()  # drop out of the sky
		# Past coyote_time, so a normal jump is no longer possible.
		elif can_fly and _air_time > coyote_time:
			start_flying()
		else:
			request_jump()
	if Input.is_action_just_pressed("roar"):
		play_action(&"Roar")
	if Input.is_action_just_pressed("attack"):
		play_action(&"Attack")
