extends TrinityUI

const ARC_PATH := "res://Assets/ui/data/title_menu/title_menu_00.arc"
const LAYOUT_FILE := "blyt/title_menu_00.bflyt"
const TRUIV_PATH := "res://Assets/ui/data/title_menu/view_title_menu_00.truiv"

func _ready() -> void:
	Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	var error := load_ui(TRUIV_PATH, ARC_PATH, LAYOUT_FILE)
	if error != OK:
		push_error("Failed to load title UI: %s" % error_string(error))
		return

	# The BFLYT starts transparent, awaiting the entrance BFLAN animation.
	# Show the static title until animation playback is implemented.
	for pane_name in ["N_inout_00", "N_inout_01"]:
		var pane := get_pane(pane_name)
		if pane != null:
			pane.modulate.a = 1.0

	# Draw the converted layout over the backdrop, below the working menu.
	move_child(get_node("Layout"), $Backdrop.get_index() + 1)
	$MenuContainer/NewGame.grab_focus()
	
	# Turn on and off some panes
	get_pane("N_banner_00").show()
	get_pane("P_ofsc_00").hide()

func NewGame():
	GameManager.state = GameManager.GameState.STATE_PLAYING
	GameManager.LoadScene("test_scene.tscn")
	
func LoadGame():
	pass
	
func QuitGame():
	get_tree().quit()
