extends Node

enum GameState {
	STATE_MAINMENU,
	STATE_PLAYING, 
	STATE_PAUSED,
}

var state : GameState
@onready var loadingScreen = preload("res://Scenes/UI/loading_screen.tscn")

var nextScene : String

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_ALWAYS
	state = GameState.STATE_MAINMENU
	
func _process(_delta: float) -> void:	
	if Input.is_action_just_pressed("menu_back"):
		if (state == GameState.STATE_PLAYING or state == GameState.STATE_PAUSED):
			pass
	
func LoadScene(scene :String):
	nextScene = scene
	get_tree().change_scene_to_packed(loadingScreen)
