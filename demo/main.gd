@tool
extends Node

@onready var pkmn = $PokemonEntity
func _ready():
	pkmn.species = 5
	pkmn.Initialize()
	#pkmn.PlayAnim("00300_roar01")
	
	#Load static objects
	$Lamp.load_model("res://Assets/field/model/t1/t1_ar/t1_ar001", "t1_ar001.trmdl")
	$Enkei.load_model("res://Assets/field/model/env/enkei/enkei1", "enkei_landscape1.trmdl")
