extends Node

signal evolution_started
signal evolution_finished(success: bool)

const WHITE_SHADER := preload("res://Scripts/Player/evolution_white.gdshader")

@export_range(0.05, 5.0, 0.05) var fade_in_duration := 0.65
@export_range(0.0, 5.0, 0.05) var before_swap_duration := 0.25
@export_range(0.0, 5.0, 0.05) var after_swap_duration := 0.35
@export_range(0.05, 5.0, 0.05) var fade_out_duration := 0.9

var is_evolving := false
var _character: PokemonCharacter
var _white_material: ShaderMaterial
var _covered_model: Node3D
var _original_overlays: Array[Dictionary] = []
var _tween: Tween
var _success := false

func _ready() -> void:
	_character = get_parent() as PokemonCharacter
	if _character:
		_character.character_rebuilt.connect(_on_character_rebuilt)

func start(index: int = 0) -> bool:
	if is_evolving or not is_instance_valid(_character):
		return false
	var actor = _character.get_model()
	if not is_instance_valid(actor):
		return false
	var evolutions: Array = actor.GetEvolutions()
	if index < 0 or index >= evolutions.size():
		return false

	_white_material = ShaderMaterial.new()
	_white_material.shader = WHITE_SHADER
	_set_whiteness(0.0)
	_cover_model()
	is_evolving = true
	_success = false
	_tween = create_tween().set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_IN_OUT)
	_tween.tween_method(_set_whiteness, 0.0, 1.0, fade_in_duration)
	_tween.tween_interval(before_swap_duration)
	_tween.tween_callback(_swap_model.bind(index))
	_tween.tween_interval(after_swap_duration)
	_tween.tween_method(_set_whiteness, 1.0, 0.0, fade_out_duration)
	_tween.tween_callback(_finish)
	evolution_started.emit()
	return true

func _set_whiteness(amount: float) -> void:
	_white_material.set_shader_parameter("whiteness", amount)

func _cover_model() -> void:
	var actor = _character.get_model()
	var model: Node3D = actor.GetModel() if is_instance_valid(actor) else null
	if not is_instance_valid(model) or model == _covered_model:
		return
	_restore_overlays()
	_covered_model = model
	for node in model.find_children("*", "MeshInstance3D", true, false):
		var mesh := node as MeshInstance3D
		_original_overlays.append({"mesh": mesh, "overlay": mesh.material_overlay})
		mesh.material_overlay = _white_material

func _on_character_rebuilt() -> void:
	if is_evolving:
		_cover_model()

func _swap_model(index: int) -> void:
	# Evolve emits character_rebuilt synchronously, covering the new meshes
	# before their first rendered frame while whiteness is still at one.
	_success = _character.Evolve(index)
	_cover_model()

func _restore_overlays() -> void:
	for entry in _original_overlays:
		var mesh = entry.mesh
		if is_instance_valid(mesh) and mesh.material_overlay == _white_material:
			mesh.material_overlay = entry.overlay
	_original_overlays.clear()
	_covered_model = null

func _finish() -> void:
	_restore_overlays()
	_white_material = null
	_tween = null
	is_evolving = false
	evolution_finished.emit(_success)

func _exit_tree() -> void:
	if _tween:
		_tween.kill()
	_restore_overlays()
	is_evolving = false
