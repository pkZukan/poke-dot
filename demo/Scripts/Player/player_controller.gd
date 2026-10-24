extends TrinityCharacterController

## Demo player: keyboard/mouse and joypad input with an orbiting camera.

@onready var camera_pivot: Node3D = $CameraPivot
@onready var camera: Camera3D = $CameraPivot/Camera3D

@export_category("Control sensitivity")
@export var mouse_sensitivity: float = 0.2
@export var joy_sensitivity: float = 120.0  # degrees per second at full tilt

var _air_time := 0.0

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	if character.has_signal("character_rebuilt"):
		character.character_rebuilt.connect(_check_isflying)
	_check_isflying()

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
		character.Evolve()
		#play_action(&"Attack")
