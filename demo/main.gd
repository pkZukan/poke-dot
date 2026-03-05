@tool
extends Node3D

@onready var pkmn: PokemonEntity = $PokemonEntity

func _ready():
	await get_tree().process_frame
	pkmn.PlayAnim("default")
