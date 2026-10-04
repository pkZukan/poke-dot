extends TrinityUI

const ARC_PATH := "res://Assets/ui/data/main_menu/main_menu_top_00.arc"
const TRUIV_PATH := "res://Assets/ui/data/main_menu/view_main_menu_top_00.truiv"

func _ready() -> void:
	visible = false
	GameManager.state_changed.connect(_on_state_changed)
	var error := load_ui(TRUIV_PATH, ARC_PATH)
	if error != OK:
		push_error("Failed to load main UI: %s" % error_string(error))
		return
	# Draw the converted layout over the backdrop, below the working menu.
	move_child(get_node("Layout"), $Backdrop.get_index() + 1)

func _on_state_changed(new_state: GameManager.GameState) -> void:
	visible = (new_state == GameManager.GameState.STATE_PAUSED)
