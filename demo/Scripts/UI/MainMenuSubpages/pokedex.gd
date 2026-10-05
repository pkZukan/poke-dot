extends TrinityUI

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
	_initialize_state()

func _initialize_state() -> void:
	# Sample the completed entrance states; apply_state does not play animations.
	apply_state(".", "in", 3.0)
	# f_in alone leaves the title transparent. Its continuation reveals it.
	apply_state(".", "f_in_keep", 33.0)
	apply_state(".", "keep", 0.0)

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
