extends Node

## Coordinates the form swap, material cover, and converted retail PTCL emitters.
signal transformation_started
signal transformation_finished(success: bool)

const WHITE_SHADER := preload("res://Scripts/Player/evolution_white.gdshader")
const ENERGY_SHADER := preload("res://Scripts/Player/mega_energy.gdshader")

@export_range(0.05, 5.0, 0.05) var charge_duration := 1.1
@export_range(0.0, 2.0, 0.05) var swap_hold_duration := 0.2
@export_range(0.05, 5.0, 0.05) var reveal_duration := 0.85
@export_range(0.0, 5.0, 0.05) var settle_duration := 0.65

var is_evolving := false
var _character: PokemonCharacter
var _tween: Tween
var _visuals: Node3D
var _shell: MeshInstance3D
var _energy: ShaderMaterial
var _white: ShaderMaterial
var _rings: Array[MeshInstance3D] = []
var _overlays: Array[Dictionary] = []
var _sparks: TrinityParticles
var _burst: TrinityParticles
var _foot_particles: TrinityParticles
var _light: OmniLight3D
var _animator: AnimationTree
var _animator_was_active := false
var _success := false
var _reverting := false
var _radius := 1.0
var _strength := 0.0

func _ready() -> void:
	_character = get_parent() as PokemonCharacter
	if _character:
		_character.character_rebuilt.connect(_on_character_rebuilt)
	set_process(false)

func start(index: int = 0) -> bool:
	if is_evolving or not is_instance_valid(_character) or not is_inside_tree():
		return false
	_reverting = _character.IsMegaEvolved()
	if not _reverting and (index < 0 or index >= _character.GetMegaForms().size()):
		return false
	var actor = _character.get_model()
	if not is_instance_valid(actor) or not is_instance_valid(actor.GetModel()):
		return false
	is_evolving = true
	_success = false
	_character.velocity = Vector3.ZERO
	_animator = _character.get_node_or_null("AnimationTree") as AnimationTree
	_animator_was_active = _animator.active if _animator else false
	if _animator:
		_animator.active = false
	_white = ShaderMaterial.new()
	_white.shader = WHITE_SHADER
	_build_visuals()
	_cover_model()
	_set_strength(0.0)
	set_process(true)
	_tween = create_tween().set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_IN_OUT)
	_tween.tween_method(_set_strength, 0.0, 1.0, charge_duration)
	_tween.tween_callback(_swap_form.bind(index))
	_tween.tween_interval(swap_hold_duration)
	_tween.tween_callback(_reveal)
	_tween.tween_method(_set_strength, 1.0, 0.0, reveal_duration)
	_tween.tween_interval(settle_duration)
	_tween.tween_callback(_finish)
	transformation_started.emit()
	return true

func _model_bounds() -> AABB:
	var actor = _character.get_model()
	var model: Node3D = actor.GetModel()
	var bounds := AABB()
	var found := false
	for node in model.find_children("*", "MeshInstance3D", true, false):
		var mesh := node as MeshInstance3D
		if mesh.mesh == null or not mesh.visible:
			continue
		var local_bounds: AABB = (_character.global_transform.affine_inverse() * mesh.global_transform) * mesh.get_aabb()
		bounds = bounds.merge(local_bounds) if found else local_bounds
		found = true
	return bounds if found else AABB(Vector3(-0.5, 0.0, -0.5), Vector3.ONE)

func _build_visuals() -> void:
	var bounds := _model_bounds()
	_radius = maxf(maxf(bounds.size.x, bounds.size.z) * 0.7, 0.45)
	_visuals = Node3D.new()
	_visuals.name = "MegaTransformationVisuals"
	_character.add_child(_visuals)
	_visuals.position = bounds.get_center()
	_energy = ShaderMaterial.new()
	_energy.shader = ENERGY_SHADER
	_shell = MeshInstance3D.new()
	var sphere := SphereMesh.new()
	sphere.radius = 1.0
	sphere.height = 2.0
	_shell.mesh = sphere
	_shell.material_override = _energy
	_shell.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_shell.scale = Vector3(_radius, maxf(bounds.size.y * 0.65, _radius), _radius)
	_visuals.add_child(_shell)
	for i in range(3):
		var ring := MeshInstance3D.new()
		var torus := TorusMesh.new()
		torus.inner_radius = _radius * 0.97
		torus.outer_radius = _radius * 1.03
		torus.rings = 48
		torus.ring_segments = 8
		ring.mesh = torus
		ring.material_override = _glow_material(Color(0.35 + i * 0.2, 0.25, 1.0))
		ring.rotation = Vector3(i * 0.9, i * 1.1, i * 0.6)
		ring.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
		_visuals.add_child(ring)
		_rings.append(ring)
	# PTCL dimensions are authored separately from each Pokemon's model bounds.
	var particle_scale := maxf(bounds.size.y, 1.0) / 30.0
	_sparks = _load_particles("cmn_mega_charge_00", particle_scale)
	_foot_particles = _load_particles("cmn_mega_charge_foot_00", particle_scale)
	_foot_particles.position.y = bounds.position.y - bounds.get_center().y
	_burst = _load_particles("cmn_mega_charge_end_00", particle_scale)
	_sparks.play()
	_foot_particles.play()
	_light = OmniLight3D.new()
	_light.light_color = Color(0.6, 0.35, 1.0)
	_light.omni_range = maxf(_radius * 5.0, 3.0)
	_visuals.add_child(_light)

func _glow_material(color: Color) -> StandardMaterial3D:
	var material := StandardMaterial3D.new()
	material.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	material.blend_mode = BaseMaterial3D.BLEND_MODE_ADD
	material.vertex_color_use_as_albedo = true
	material.albedo_color = color
	material.emission_enabled = true
	material.emission = color
	material.emission_energy_multiplier = 2.0
	return material

func _load_particles(effect_name: String, unit_scale: float) -> TrinityParticles:
	var particles := TrinityParticles.new()
	_visuals.add_child(particles)
	var path := GFLib.romfs_path("ik_effect/data/battle/cmn/mega/%s.ptcl" % effect_name)
	if FileAccess.file_exists(path):
		var error := particles.load_effect(path, "", unit_scale)
		if error != OK:
			push_warning("Could not convert Mega effect: " + path)
	else:
		push_warning("Missing Mega particle archive: " + path)
	return particles

func _set_strength(value: float) -> void:
	_strength = value
	_white.set_shader_parameter("whiteness", value)
	_energy.set_shader_parameter("strength", value)
	_light.light_energy = value * 3.0
	for ring in _rings:
		ring.visible = value > 0.01
		ring.scale = Vector3.ONE * lerpf(1.6, 1.0, value)
		var material := ring.material_override as StandardMaterial3D
		material.albedo_color.a = value

func _process(delta: float) -> void:
	for i in range(_rings.size()):
		_rings[i].rotate_y(delta * (1.5 + i) * (1.0 + _strength))
		_rings[i].rotate_x(delta * (0.5 + i * 0.3))

func _cover_model() -> void:
	_restore_overlays()
	var model: Node3D = _character.get_model().GetModel()
	for node in model.find_children("*", "MeshInstance3D", true, false):
		var mesh := node as MeshInstance3D
		_overlays.append({"mesh": mesh, "overlay": mesh.material_overlay})
		mesh.material_overlay = _white

func _on_character_rebuilt() -> void:
	if is_evolving:
		_cover_model()
		if _animator:
			_animator.active = false

func _swap_form(index: int) -> void:
	_success = _character.RevertMegaEvolution() if _reverting else _character.MegaEvolve(index)
	# Keep both old and new bounds covered when the Mega form is larger.
	var bounds := _model_bounds()
	_visuals.position = bounds.get_center()
	_shell.scale = _shell.scale.max(Vector3(bounds.size.x * 0.7, bounds.size.y * 0.65, bounds.size.z * 0.7))

func _reveal() -> void:
	_sparks.stop()
	_foot_particles.stop()
	_burst.play()
	# Let the actual Mega appeal clip play while locomotion is suspended.
	var player := _character.get_animation_player()
	if _success and not _reverting and player:
		for clip in player.get_animation_list():
			if String(clip).contains("megaappeal"):
				player.play(clip)
				break

func _restore_overlays() -> void:
	for entry in _overlays:
		if is_instance_valid(entry.mesh) and entry.mesh.material_overlay == _white:
			entry.mesh.material_overlay = entry.overlay
	_overlays.clear()

func _cleanup() -> void:
	set_process(false)
	_restore_overlays()
	if is_instance_valid(_visuals):
		_visuals.hide()
		_visuals.queue_free()
	_visuals = null
	_rings.clear()
	if is_instance_valid(_animator):
		_animator.active = _animator_was_active
	_white = null
	_energy = null
	is_evolving = false

func _finish() -> void:
	_cleanup()
	_tween = null
	transformation_finished.emit(_success)

func _exit_tree() -> void:
	if _tween:
		_tween.kill()
	_cleanup()
