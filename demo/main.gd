@tool
extends Node

@onready var pkmn = $PokemonEntity
func _ready():
	await get_tree().process_frame
	pkmn.PlayAnim("00000_defaultwait01_loop")
