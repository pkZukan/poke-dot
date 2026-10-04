extends TrinityUI

signal back_requested

const ARC_PATH := "res://Assets/ui/data/box/box_top_00.arc"
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
	# Sample the completed entrance states; apply_state does not play animations.
	apply_state(".", "in", 3.0)
	# f_in alone leaves the title transparent. Its continuation reveals it.
	apply_state(".", "f_in_keep", 33.0)
	apply_state(".", "keep", 0.0)
	
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
