extends Node

signal loading_progress(current: int, total: int)

const player_prefab = preload("res://Scenes/Player/Player.tscn")

@onready var scene_loader: TrinitySceneLoader = $SceneLoader

func _ready() -> void:
	scene_loader.loading_progress.connect(loading_progress.emit)

	var player = player_prefab.instantiate()
	player.transform = $Spawn.transform
	add_child(player)

func load_models_async() -> void:
	scene_loader.load_scene()
	await scene_loader.loading_finished
