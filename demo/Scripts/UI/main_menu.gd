extends Control

@onready var loadButton = $Backdrop/MenuContainer/NewGame

func _ready():
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	#loadButton.disabled()
	var trcol : TRCOL = ResourceLoader.load("res://Assets/field/model/t1/t1_b/t1_b_bank/t1_b_bank_col.trcol")
	var tree = trcol.get_tag().get_tree_item()
	traverse_tree_item(tree)

func _process(_delta):
	pass

func NewGame():
	GameManager.state = GameManager.GameState.STATE_PLAYING
	GameManager.LoadScene("main.tscn")
	
func LoadGame():
	pass
	
func QuitGame():
	get_tree().quit()

func traverse_tree_item(item: TreeItem) -> void:
	if item == null:
		return
	
	# Process current item
	print("Item text: ", item.get_text(0))
	
	# Recursively process the first child
	var child = item.get_first_child()
	while child != null:
		traverse_tree_item(child)
		child = child.get_next() # Get the next sibling
