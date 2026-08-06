extends Control

@onready var progress_bar: ProgressBar = $ProgressBar 

var next_scene_path: String
var progress: Array = []

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)

	if progress_bar:
		progress_bar.value = 0.0
	
	next_scene_path = "res://Scenes/" + GameManager.nextScene
	
	if not next_scene_path.is_empty():
		ResourceLoader.load_threaded_request(next_scene_path)

func _process(_delta: float) -> void:
	if next_scene_path.is_empty():
		return

	var status = ResourceLoader.load_threaded_get_status(next_scene_path, progress)

	if progress_bar and progress.size() > 0:
		progress_bar.value = progress[0] * 100.0

	match status:
		ResourceLoader.THREAD_LOAD_LOADED:
			var scene: PackedScene = ResourceLoader.load_threaded_get(next_scene_path)
			get_tree().change_scene_to_packed(scene)
			
		ResourceLoader.THREAD_LOAD_FAILED:
			push_error("Failed to load scene: " + next_scene_path)
			set_process(false)
