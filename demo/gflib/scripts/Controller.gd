extends Node

@onready var pkmn: PokemonCharacter = $"."
@onready var camera_pivot = $CameraPivot
@onready var camera = $CameraPivot/Camera3D

@export_category("Control sensitivity")
@export var mouse_sensitivity: float = 0.2
@export var look_speed: float = 0.1

func _ready():	
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	
func _input(event) -> void:
	# Handle mouse movement for looking around
	if event is InputEventMouseMotion:
		# Rotate the body left/right (yaw)
		pkmn.rotate_y(deg_to_rad(-event.relative.x * mouse_sensitivity))
		
		var rotX = deg_to_rad(event.relative.y * mouse_sensitivity)
		camera_pivot.rotate_x(rotX)
		camera_pivot.rotation_degrees.x = clamp(camera_pivot.rotation_degrees.x, -80, 80)
func _process(_delta: float) -> void:
	pass
	
func ApplyMovement(delta):
	var root_motion = pkmn.GetRootMotionPos()
	
	var max_delta = 0.1
	if root_motion.length() > max_delta:
		root_motion = Vector3.ZERO
	
	if pkmn.is_on_floor():
		var motion = pkmn.global_transform.basis * root_motion
		pkmn.velocity.x = motion.x / delta
		pkmn.velocity.z = motion.z / delta
		if pkmn.velocity.y < 0:
			pkmn.velocity.y = 0
	else:
		pkmn.velocity.y -= 9.8 * delta

	pkmn.move_and_slide()
	
func Idle():
	pkmn.Idle()
	
func Walk(dir):
	pkmn.Walk(dir.z)
	
func Run():
	pkmn.Run()
	
func Jump():
	if pkmn.is_on_floor():
		pkmn.velocity.y = 4.5
		
func _physics_process(delta) -> void:
	# Get movement input
	var input_dir = Vector3.ZERO
	if Input.is_action_pressed("move_forward"):
		input_dir.z = 1
	if Input.is_action_pressed("move_back"):
		input_dir.z = -1
	#if Input.is_action_pressed("strafe_left"):
	#	input_dir.x = 1
	#if Input.is_action_pressed("strafe_right"):
	#	input_dir.x = -1
	if input_dir == Vector3.ZERO:
		Idle()
	else:
		if Input.is_action_pressed("run") && input_dir.z > 0:
			Run()
		else:
			Walk(input_dir)
		
	if Input.is_action_just_pressed("jump"):
		Jump()
		
	if Input.is_action_pressed("roar"):
		pkmn.Roar()	
	
	if Input.is_action_pressed("attack"):
		pkmn.Attack()	
	
	ApplyMovement(delta)
