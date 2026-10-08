extends "res://Scripts/UI/animated_ui.gd"

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
	
	_initialize_text()
	_initialize_state()
	configure_entrance(["in", "keep"])

func _initialize_state() -> void:
	for i in 5:
		var component := "L_menu_list_%02d" % i
		apply_state(component, "active")

func _on_entrance_started() -> void:
	for i in 5:
		play_state("L_menu_list_%02d" % i, "select" if i == 0 else "unselect")

func _initialize_text():
	var entries := ["Link Trade", "Link Battle", "Mystery Gift"]
	for i in entries.size():
		get_scope("L_menu_list_%02d" % (i + 2))._set_text("T_list_00", entries[i])
	
func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
