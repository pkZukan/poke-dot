extends Node

const AnimationController = preload("res://gflib/scripts/AnimationController.gd")

@onready var pkmn: PokemonCharacter = $"."
@onready var camera_pivot: Node3D = $CameraPivot
@onready var animations: AnimationController = $AnimationTree

@export_category("Control sensitivity")
@export var mouse_sensitivity: float = 0.2

@export_category("Jump")
@export_range(0.0, 20.0, 0.1) var jump_velocity: float = 4.5
@export_range(0.0, 0.3, 0.01) var coyote_time: float = 0.1
@export_range(0.0, 0.3, 0.01) var jump_buffer_time: float = 0.12

var _coyote_remaining: float = 0.0
var _jump_buffer_remaining: float = 0.0

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		pkmn.rotate_y(deg_to_rad(-event.screen_relative.x * mouse_sensitivity))
		camera_pivot.rotation.x = clampf(
			camera_pivot.rotation.x - deg_to_rad(event.screen_relative.y * mouse_sensitivity),
			deg_to_rad(-80.0), deg_to_rad(80.0)
		)

func _physics_process(delta: float) -> void:
	var movement := Input.get_vector("strafe_left", "strafe_right", "move_back", "move_forward")
	animations.set_movement(movement, Input.is_action_pressed("run"))

	if pkmn.is_on_floor():
		_coyote_remaining = coyote_time
	else:
		_coyote_remaining = maxf(0.0, _coyote_remaining - delta)
	_jump_buffer_remaining = maxf(0.0, _jump_buffer_remaining - delta)
	var jump_pressed := Input.is_action_just_pressed("jump")
	if jump_pressed:
		_jump_buffer_remaining = jump_buffer_time
	if (jump_pressed or _jump_buffer_remaining > 0.0) and (
		pkmn.is_on_floor() or _coyote_remaining > 0.0
	):
		pkmn.velocity.y = jump_velocity
		_coyote_remaining = 0.0
		_jump_buffer_remaining = 0.0

	if Input.is_action_just_pressed("roar"):
		animations.play_action(AnimationController.ROAR)
	if Input.is_action_just_pressed("attack"):
		animations.play_action(AnimationController.ATTACK)

	animations.update_animation(delta)
	pkmn.apply_movement(delta, animations.movement_direction)
