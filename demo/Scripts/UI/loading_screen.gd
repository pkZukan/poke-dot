extends Control

@onready var progress_bar: ProgressBar = $ProgressBar 

var next_scene_path: String
var progress: Array = []
var is_instantiating: bool = false

func _ready() -> void:
	if progress_bar:
		progress_bar.value = 0.0
	
	next_scene_path = "res://Scenes/" + GameManager.nextScene
	
	if not next_scene_path.is_empty():
		ResourceLoader.load_threaded_request(next_scene_path)

func _process(_delta: float) -> void:
	if next_scene_path.is_empty() or is_instantiating:
		return

	var status = ResourceLoader.load_threaded_get_status(next_scene_path, progress)

	if progress_bar and progress.size() > 0:
		progress_bar.value = progress[0] * 50.0

	match status:
		ResourceLoader.THREAD_LOAD_LOADED:
			is_instantiating = true
			_start_scene_initialization()
			
		ResourceLoader.THREAD_LOAD_FAILED:
			push_error("Failed to load scene: " + next_scene_path)
			set_process(false)

func _start_scene_initialization() -> void:
	var packed_scene: PackedScene = ResourceLoader.load_threaded_get(next_scene_path)
	var scene_instance = packed_scene.instantiate()

	if scene_instance.has_signal("loading_progress"):
		scene_instance.loading_progress.connect(_on_scene_loading_progress)

	get_tree().root.add_child(scene_instance)

	if scene_instance.has_method("load_models_async"):
		await scene_instance.load_models_async()

	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	queue_free()

func _on_scene_loading_progress(current: int, total: int) -> void:
	if progress_bar and total > 0:
		var model_percent = float(current) / float(total)
		progress_bar.value = 50.0 + (model_percent * 50.0)
