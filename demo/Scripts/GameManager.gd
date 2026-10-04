extends Node

signal state_changed(new_state: GameState)

enum GameState {
	STATE_MAINMENU,
	STATE_PLAYING, 
	STATE_PAUSED,
}
var state : GameState

@onready var loadingScreen = preload("res://Scenes/UI/loading_screen.tscn")
var MainMenuScene = preload("res://Scenes/UI/main_menu.tscn")

var nextScene : String

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_ALWAYS
	state = GameState.STATE_MAINMENU
	add_child(MainMenuScene.instantiate())
	
func _process(_delta: float) -> void:
	if Input.is_action_just_pressed("main_menu"):
		match state:
			GameState.STATE_PLAYING:
				set_state(GameState.STATE_PAUSED)
			GameState.STATE_PAUSED:
				set_state(GameState.STATE_PLAYING)

func set_state(new_state: GameState) -> void:
	state = new_state
	get_tree().paused = (state == GameState.STATE_PAUSED)
	state_changed.emit(state)
	
func LoadScene(scene :String):
	nextScene = scene
	get_tree().change_scene_to_packed(loadingScreen)
