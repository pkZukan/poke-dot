@tool
extends Node

const player_prefab = preload("res://Scenes/Player/Player_tr.tscn")

func _ready() -> void:
	var player = player_prefab.instantiate()
	player.transform = $Spawn.transform
	add_child(player)

func _process(delta: float) -> void:
	pass
