@tool
extends Node3D

func _ready():
	var trainer:TrinityModel = TrinityModel.new()
	trainer.Load("res://tr0380_musician/", "tr0380_00.trmdl")
	trainer.position = Vector3.ZERO
	
	var pkmn = TrinityModel.new()
	pkmn.Load("res://pm0004_00_00/", "pm0004_00_00.trmdl")
	pkmn.position = Vector3(1.0,0.0,0.0)
	
	add_child(trainer)
	add_child(pkmn)

func _process(delta):
	pass
