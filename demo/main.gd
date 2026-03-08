@tool
extends Node

@onready var pkmn = $PokemonEntity
func _ready():
	await get_tree().process_frame
	pkmn.species = 5
	pkmn.Initialize()
	#pkmn.PlayAnim("00300_roar01")
