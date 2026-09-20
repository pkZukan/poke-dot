extends Control

@onready var loadButton = $Backdrop/MenuContainer/NewGame

func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	#loadButton.disabled()
	var trcol : TRCOL = ResourceLoader.load("res://Assets/field/model/t1/t1_b/t1_b_bank/t1_b_bank_col.trcol")

func _process(_delta):
	pass

func NewGame():
	GameManager.state = GameManager.GameState.STATE_PLAYING
	GameManager.LoadScene("main.tscn")
	
func LoadGame():
	pass
	
func QuitGame():
	get_tree().quit()
