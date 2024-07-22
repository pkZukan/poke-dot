@tool
extends Node3D

func _ready():
	var trainer:TrinityModel = TrinityModel.new()
	trainer.Load("res://Assets/chara/model_tr/tr0380_musician/", "tr0380_00.trmdl")
	trainer.position = Vector3.ZERO
	
	var pkmn = TrinityModel.new()
	pkmn.Load("res://Assets/pokemon/data/pm0004/pm0004_00_00/", "pm0004_00_00.trmdl")
	pkmn.position = Vector3(1.0,0.0,0.0)
	
	var pkmn2 = TrinityModel.new()
	pkmn2.Load("res://Assets/pokemon/data/pm0248/pm0248_00_00/", "pm0248_00_00.trmdl")
	pkmn2.position = Vector3(-1.5,0.0,0.0)
	
	add_child(trainer)
	add_child(pkmn)
	add_child(pkmn2)

func _process(delta):
	pass
