extends TrinityUI

signal back_requested

const ARC_PATH := "res://Assets/ui/data/mresearch/mresearch_top_eng.arc"
const TRUIV_PATH := "res://Assets/ui/data/mresearch/view_mresearch_top_00.truiv"

func _ready() -> void:
	hide()
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load MResearch UI: %s" % error_string(error))
		return

	# Hide dummy pane
	#get_pane("N_ofsk_00").hide()
	
	_initialize()

func _initialize() -> void:
	pass

func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
