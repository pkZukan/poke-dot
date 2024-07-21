@tool
extends Node3D

func _ready():
	var mdl:TrinityModel = TrinityModel.new()
	mdl.Load("res://pm0004_00_00/", "pm0004_00_00.trmdl")
	mdl.position = Vector3.ZERO
	add_child(mdl)

func _process(delta):
	pass
