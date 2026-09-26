extends Control

@onready var loadButton = $Backdrop/MenuContainer/NewGame

func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	#loadButton.disabled()

func _process(_delta):
	pass

func NewGame():
	GameManager.state = GameManager.GameState.STATE_PLAYING
	GameManager.LoadScene("main.tscn")
	
func LoadGame():
	pass
	
func QuitGame():
	get_tree().quit()
