@tool
extends Node3D

func _ready():
	var pkmn = TrinityModel.new()
	pkmn.Load("res://Assets/ik_pokemon/data/pm0002/pm0002_00_00/", "pm0002_00_00.trmdl")
	pkmn.position = Vector3(0.0,0.0,0.0)
	
	add_child(pkmn)

func _process(delta):
	pass
