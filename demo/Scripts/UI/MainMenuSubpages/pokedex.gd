extends "res://Scripts/UI/animated_ui.gd"

signal back_requested

const ARC_PATH := "res://Assets/ui/data/pokedex/pokedex_top_eng.arc"
const TRUIV_PATH := "res://Assets/ui/data/pokedex/view_pokedex_top_00.truiv"

func _ready() -> void:
	hide()
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load Pokédex UI: %s" % error_string(error))
		return

	# Hide dummy pane
	get_pane("N_ofsk_00").hide()
	
	_initialize_text()
	configure_entrance(["in", "f_in_keep", "keep"])

func _initialize_text():
	set_seen(0)
	set_caught(0)

func set_caught(num: int):
	_set_text("T_v_get_00", str(maxi(num, 0)))
	_set_text("T_hget_00", "Number Caught")
	
func set_seen(num: int):
	_set_text("T_v_find_00", str(maxi(num, 0)))
	_set_text("T_hget_01", "Number Seen")
	
func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
