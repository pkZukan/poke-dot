class_name TrinityCharacterController
extends Node

## Moves a PokemonCharacter or TrainerCharacter from movement intent and keeps
## its TrinityCharacterAnimator in sync. Attach to the character itself.
##
## This class reads no input. Extend it and override _update_intent() to set
## move_input/running and call request_jump()/play_action() from player
## input, AI, a network peer, etc. Flight works the same way: set can_fly, then
## start_flying()/stop_flying() and steer with move_input and fly_vertical.

@onready var character = $"."
@onready var animator: TrinityCharacterAnimator = $AnimationTree

@export_category("Jump")
@export_range(0.0, 20.0, 0.1) var jump_velocity: float = 4.5
@export_range(0.0, 0.3, 0.01) var coyote_time: float = 0.1
@export_range(0.0, 0.3, 0.01) var jump_buffer_time: float = 0.12

@export_category("Flight")
@export_range(0.0, 50.0, 0.1, "or_greater") var fly_speed: float = 8.0
@export_range(0.0, 100.0, 0.1, "or_greater") var fly_acceleration: float = 20.0

## Desired movement relative to the character: x = strafe, y = forward.
var move_input := Vector2.ZERO
var running := false
## Whether start_flying() is allowed; decided by the subclass (type, item, ...).
var can_fly := false
## Climb (+1) or descend (-1) while flying.
var fly_vertical := 0.0

var _coyote_remaining: float = 0.0
var _jump_buffer_remaining: float = 0.0
var _jump_requested := false
var _flying := false

## Override to fill in move_input/running and request jumps or actions.
func _update_intent(_delta: float) -> void:
	pass

## Jumps now if possible, or within jump_buffer_time once grounded.
func request_jump() -> void:
	_jump_requested = true

## Takes off if can_fly. Gravity stops until stop_flying() or landing.
func start_flying() -> void:
	if can_fly:
		_flying = true

func stop_flying() -> void:
	_flying = false

func is_flying() -> bool:
	return _flying

## Plays a OneShot action from the animation graph, e.g. &"Roar".
func play_action(action: StringName) -> void:
	animator.play_action(action)

func _physics_process(delta: float) -> void:
	_update_intent(delta)
	animator.set_movement(move_input, running)
	animator.set_flying(_flying)
	if _flying:
		_jump_requested = false
		_fly(delta)
		return

	if character.is_on_floor():
		_coyote_remaining = coyote_time
	else:
		_coyote_remaining = maxf(0.0, _coyote_remaining - delta)
	_jump_buffer_remaining = maxf(0.0, _jump_buffer_remaining - delta)
	if _jump_requested:
		_jump_buffer_remaining = jump_buffer_time
	if (_jump_requested or _jump_buffer_remaining > 0.0) and (
		character.is_on_floor() or _coyote_remaining > 0.0
	):
		character.velocity.y = jump_velocity
		_coyote_remaining = 0.0
		_jump_buffer_remaining = 0.0
	_jump_requested = false

	animator.update_animation(delta)
	character.apply_movement(delta, animator.movement_direction)

# Velocity-driven flight without gravity, relative to the character's facing.
func _fly(delta: float) -> void:
	var local := Vector3(-move_input.x, 0.0, move_input.y).limit_length()
	var speed := fly_speed * (2.0 if running else 1.0)
	var target: Vector3 = character.global_basis * local * speed + Vector3.UP * clampf(fly_vertical, -1.0, 1.0) * speed
	character.velocity = character.velocity.move_toward(target, fly_acceleration * delta)
	animator.update_animation(delta)
	character.move_and_slide()
	if character.is_on_floor() and character.velocity.y <= 0.0:
		stop_flying()
		animator.set_flying(false)
