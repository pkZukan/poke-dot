extends TrinityUI

signal back_requested

const ARC_PATH := "res://Assets/ui/data/net_topmenu/net_topmenu_top_00.arc"
const TRUIV_PATH := "res://Assets/ui/data/net_topmenu/view_net_topmenu_top_00.truiv"

func _ready() -> void:
	hide()
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load NetTopmenu UI: %s" % error_string(error))
		return

	_set_text("T_guide_text_00", "Move up the ranks by taking part in online battles and earning points!")
	get_scope("L_info_00")._set_text("T_option_00", "Check News")
	
	_initialize()

func _initialize() -> void:	
	pass
	
func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
