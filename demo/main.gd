@tool
extends Node

@onready var pkmn = $PokemonEntity
func _ready():
	await get_tree().process_frame
	pkmn.species = 5
	pkmn.Initialize()
	#pkmn.PlayAnim("00300_roar01")
	
	$TrinityModel.load_model("res://Assets/field/model/t1/t1_ar/t1_ar001", "t1_ar001.trmdl")
