extends TrinityUI

const ARC_PATH := "res://Assets/ui/data/main_menu/main_menu_top_00.arc"
const TRUIV_PATH := "res://Assets/ui/data/main_menu/view_main_menu_top_00.truiv"

func _ready() -> void:
	visible = false
	GameManager.state_changed.connect(_on_state_changed)
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load main UI: %s" % error_string(error))
		return

	_initialize()

func _initialize() -> void:
	apply_state(".", "in", 4.0)
	apply_state(".", "ptn_menu")
	apply_state(".", "ptn_progress")
	apply_state(".", "ptn_quest_parts")
	apply_state(".", "ptn_quest_text", 3.0)
	apply_state(".", "ptn_quest_icon")
	var names := ["Boxes", "Satchel", "Pokédex", "Mable’s Research", "Z-A Royale", "Link Play"]
	for i in names.size():
		var component := "L_menu_button_%02d" % i
		var part := get_pane(component)
		apply_state(component, "active")
		apply_state(component, "ptn_icon", i)
		apply_state(component, "ptn_time")
		apply_state(component, "ptn_inf")
		apply_state(component, "select" if i == 0 else "unselect", 4.0)
		for label_name in ["T_00", "T_01"]:
			var label := part.find_child(label_name, true, false) as Label
			if label:
				label.text = names[i]
	# The demo has no party/save-data binding yet. Use the authored empty-slot state.
	for i in 6:
		var component := "L_temochi_btn_%02d" % i
		apply_state(component, "empty")
		apply_state(component, "unselect")
		apply_state(component, "item_off")
		apply_state(component, "exp_out", 5.0)
		apply_state(component, "lvup_out", 6.0)
		apply_state(component, "num_select_on")
	_set_text("T_money_00", "0")
	_set_text("T_quest_name_00", "No tracked quest")
	_set_text("T_quest_03", "")
	_set_text("T_mrrw_name_00", "—")
	_set_text("T_mrrw_num_00", "")
	var research := get_pane("L_mr_00")
	var reward_label := research.find_child("T_option_00", true, false) as Label
	if reward_label:
		reward_label.text = "Reward List"
	var map_label := get_pane("L_opguide_03").find_child("T_option_00", true, false) as Label
	if map_label:
		map_label.text = "Map"
	# Cursor placement is normally driven by UIKit focus; start on the first menu entry.
	var cursor := get_pane("L_cursor_00")
	var first := get_pane("L_menu_button_00").find_child("N_cursor", true, false) as Control
	if cursor and first:
		var target := first.get_global_transform() * (first.size * 0.5)
		cursor.position = cursor.get_parent().get_global_transform().affine_inverse() * target - cursor.size * 0.5

func _set_text(pane_name: String, text: String) -> void:
	var label := get_pane(pane_name) as Label
	if label:
		label.text = text

func _on_state_changed(new_state: GameManager.GameState) -> void:
	visible = (new_state == GameManager.GameState.STATE_PAUSED)
