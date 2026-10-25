extends Node

signal loading_progress(current: int, total: int)

const player_prefab = preload("res://Scenes/Player/Player.tscn")

@export var day_cycle_enabled := true
## Real-time minutes for a full 24 hour cycle.
@export var day_length_minutes := 10.0
## Seconds between updates; 0 updates every frame. Each update re-resolves the
## whole light stack (~0.6 ms), so raise this if that matters more than smoothness.
@export var time_update_interval := 0.0

@onready var scene_loader: TrinitySceneLoader = $SceneLoader
@onready var environment: TrinityEnvironment = $TrinityEnvironment

var _time_since_update := 0.0

func _ready() -> void:
	scene_loader.loading_progress.connect(loading_progress.emit)

	var player = player_prefab.instantiate()
	player.transform = $Spawn.transform
	add_child(player)

func _process(delta: float) -> void:
	if not day_cycle_enabled or day_length_minutes <= 0.0:
		return

	_time_since_update += delta
	if _time_since_update < time_update_interval:
		return

	# time_of_day wraps at 24 h.
	environment.time_of_day += _time_since_update * 24.0 / (day_length_minutes * 60.0)
	_time_since_update = 0.0

func load_models_async() -> void:
	scene_loader.load_scene()
	await scene_loader.loading_finished
