extends Node

@onready var pkmn: PokemonCharacter = $"."
@onready var camera_pivot = $CameraPivot
@onready var camera = $CameraPivot/Camera3D

@export_category("Control sensitivity")
@export var mouse_sensitivity: float = 0.2
@export var look_speed: float = 0.1
@export var move_speed: float = 2.0

func _ready():	
	pkmn.Initialize()
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
func _input(event) -> void:
	# Handle mouse movement for looking around
	if event is InputEventMouseMotion:
		# Rotate the body left/right (yaw)
		pkmn.rotate_y(deg_to_rad(-event.relative.x * mouse_sensitivity))
		
		# Apply inverted look if enabled
		var look_direction = -1 
		var rotX = deg_to_rad(event.relative.y * mouse_sensitivity * look_direction)
		camera_pivot.rotate_x(rotX)
		#head_pivot.rotate_x(rotX)
		camera_pivot.rotation_degrees.x = clamp(camera_pivot.rotation_degrees.x, -90, 90)
func _process(_delta: float) -> void:
	pass
	
func ApplyMovement(delta, input_dir):
	var forward_vec = -pkmn.global_transform.basis.z
	var left_vec = -pkmn.global_transform.basis.x
	
	var move_dir = (forward_vec * input_dir.z) + (left_vec * input_dir.x)
	move_dir = move_dir.normalized() if move_dir.length() > 0 else Vector3.ZERO
	if pkmn.is_on_floor():
		pkmn.velocity.x = move_dir.x * move_speed
		pkmn.velocity.z = move_dir.z * move_speed
	
	# gravity always applies, separate from floor check
	if not pkmn.is_on_floor():
		pkmn.velocity.y -= 9.8 * delta
	pkmn.move_and_slide()
func Idle():
	pkmn.Idle()
	
func Walk(dir):
	pkmn.Walk()
	move_speed = 2.0
	
func Run():
	pkmn.Run()
	move_speed = 4.0
	
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
	if Input.is_action_pressed("strafe_left"):
		input_dir.x = 1
	if Input.is_action_pressed("strafe_right"):
		input_dir.x = -1
	if input_dir == Vector3.ZERO:
		Idle()
	else:
		if Input.is_action_pressed("run"):
			Run()
		else:
			Walk(input_dir)
		
	if Input.is_action_just_pressed("jump"):
		Jump()
		
	if Input.is_action_pressed("roar"):
		pkmn.Roar()	
	
	ApplyMovement(delta, -input_dir)
