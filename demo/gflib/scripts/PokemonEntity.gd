@tool
extends Node
class_name PokemonEntity

@export var species: int = 2
var _anim_player: AnimationPlayer
var _model: TrinityModel
var _skeleton: Skeleton3D
var _species_path: String

func _ready():
	_model = TrinityModel.new()
	_anim_player = AnimationPlayer.new()
	_anim_player.name = "AnimationPlayer"
	_anim_player.root_node = _anim_player.get_path_to(self)
	_species_path = "res://Assets/ik_pokemon/data/pm%04d/pm%04d_00_00/" % [species, species]
	
	_model.load_model(_species_path, "pm%04d_00_00.trmdl" % species)
	add_child(_model)
	add_child(_anim_player)
	_setup_animation(_model)

func _setup_animation(pkmn: Node) -> void:
	_skeleton = _find_skeleton(pkmn)
	if not _skeleton:
		push_error("No Skeleton3D found in model")
		return

	_anim_player.root_node = _anim_player.get_path_to(self)

	var skl_path := str(get_path_to(_skeleton))

	#Load anim track
	var godot_anim := TrinityAnimationConverter.convert_to_godot_animation(
		_species_path, "pm%04d_00_00_00000_defaultwait01_loop.tranm" % species ,
		_skeleton, skl_path
	)

	#Create library to store track
	var anim_lib := AnimationLibrary.new()
	anim_lib.add_animation("default", godot_anim)
	_anim_player.add_animation_library("", anim_lib)
	
	#Play anim
	_anim_player.play("default")

func _find_skeleton(node: Node) -> Skeleton3D:
	if node is Skeleton3D:
		return node
	for child in node.get_children():
		var result = _find_skeleton(child)
		if result:
			return result
	return null
