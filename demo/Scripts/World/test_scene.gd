@tool
extends Node

const player_prefab = preload("res://gflib/prefabs/Player.tscn")

func _ready() -> void:
	var player = player_prefab.instantiate()
	player.transform = $Spawn.transform
	add_child(player)

func _process(delta: float) -> void:
	pass
