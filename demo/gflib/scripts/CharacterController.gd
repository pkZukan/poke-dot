class_name TrinityCharacterController
extends Node

## Moves a PokemonCharacter or TrainerCharacter from movement intent and keeps
## its TrinityCharacterAnimator in sync. Attach to the character itself.
##
## This class reads no input. Extend it and override _update_intent() to set
## move_input/running and call request_jump()/play_action() from player
## input, AI, a network peer, etc.

@onready var character = $"."
@onready var animator: TrinityCharacterAnimator = $AnimationTree

@export_category("Jump")
@export_range(0.0, 20.0, 0.1) var jump_velocity: float = 4.5
@export_range(0.0, 0.3, 0.01) var coyote_time: float = 0.1
@export_range(0.0, 0.3, 0.01) var jump_buffer_time: float = 0.12

## Desired movement relative to the character: x = strafe, y = forward.
var move_input := Vector2.ZERO
var running := false

var _coyote_remaining: float = 0.0
var _jump_buffer_remaining: float = 0.0
var _jump_requested := false

## Override to fill in move_input/running and request jumps or actions.
func _update_intent(_delta: float) -> void:
	pass

## Jumps now if possible, or within jump_buffer_time once grounded.
func request_jump() -> void:
	_jump_requested = true

## Plays a OneShot action from the animation graph, e.g. &"Roar".
func play_action(action: StringName) -> void:
	animator.play_action(action)

func _physics_process(delta: float) -> void:
	_update_intent(delta)
	animator.set_movement(move_input, running)

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
