extends TrinityUI

signal back_requested

const ARC_PATH := "res://Assets/ui/data/box/box_top_00_eng.arc"
const TRUIV_PATH := "res://Assets/ui/data/box/view_box_top_00.truiv"

func _ready() -> void:
	hide()
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load Box UI: %s" % error_string(error))
		return

	# Hide dummy pane
	#get_pane("N_ofsk_00").hide()
	
	_initialize()

func _initialize() -> void:	
	_set_text("T_name_tmc_00", "Current Party")
	
func _set_text(pane_name: String, text: String) -> void:
	var label := get_pane(pane_name) as Label
	if label:
		label.text = text

func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
