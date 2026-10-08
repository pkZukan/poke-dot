extends "res://Scripts/UI/animated_ui.gd"

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
	
	_initialize_text()
	_initialize_state()
	configure_entrance(["in", "keep"])

func _initialize_state() -> void:
	apply_state(".", "switch_tmc")
	apply_state(".", "switch_box_reset")
	apply_state(".", "switch_team_reset")
	apply_state(".", "switch_judge_off")
	apply_state(".", "switch_judge_deactivate")
	apply_state(".", "status_off")
	for i in 90:
		var component := "L_icon_box_%02d" % i
		apply_state(component, "empty")
		apply_state(component, "unselect")
		apply_state(component, "item_off")
		apply_state(component, "check_none")
	for i in 12:
		var component := "L_i_tmc_%02d" % i
		apply_state(component, "empty")
		apply_state(component, "unselect")
		apply_state(component, "item_off")
		apply_state(component, "check_none")
	for i in 18:
		var component := "L_icon_team_%02d" % i
		apply_state(component, "empty")
		apply_state(component, "unselect")
		apply_state(component, "item_off")
		apply_state(component, "check_none")

func _initialize_text():
	for i in 4:
		get_scope("L_skill_%02d" % i)._set_text("T_name_skill_00", "—")
	_set_text("T_key_guid_00", "Change View")
	_set_text("T_v_parent_00", "—")
	_set_text("T_dex_num_00", "No. —")
	_set_text("T_race_name_00", "—")
	_set_text("T_v_item_00", "None")
	_set_text("T_h_item_00", "Held Item")
	_set_text("T_h_perso_00", "Nature")
	_set_text("T_v_perso_00", "—")
	_set_text("T_nickname_00", "No Pokémon")
	_set_text("T_Lv_00", "Lv. —")
	_set_text("T_name_team_01", "Battle Team")
	_set_text("T_name_tmc_00", "Current Party")
	_set_text("T_name_box_00", "Box 1")
	
	var radar_00 = get_scope("L_radar_00")
	radar_00._set_text("T_h_gstatus_00", "HP")
	radar_00._set_text("T_h_gstatus_01", "Attack")
	radar_00._set_text("T_h_gstatus_02", "Defense")
	radar_00._set_text("T_h_gstatus_03", "Sp. Atk")
	radar_00._set_text("T_h_gstatus_04", "Sp. Def")
	radar_00._set_text("T_h_gstatus_05", "Speed")
	radar_00._set_text("T_v_gstatus_00", "—")
	radar_00._set_text("T_v_gstatus_01", "—")
	radar_00._set_text("T_v_gstatus_02", "—")
	radar_00._set_text("T_v_gstatus_03", "—")
	radar_00._set_text("T_v_gstatus_04", "—")
	radar_00._set_text("T_v_gstatus_05", "—")
	radar_00._set_text("T_v_judge_00", "—")
	radar_00._set_text("T_v_judge_01", "—")
	radar_00._set_text("T_v_judge_02", "—")
	radar_00._set_text("T_v_judge_03", "—")
	radar_00._set_text("T_v_judge_04", "—")
	radar_00._set_text("T_v_judge_05", "—")
	
	get_scope("box_top_00/all/N_inout_00/N_inout_01/N_blist_out_00/N_status_00/N_egg_00/N_fixed_00/N_judge_00")._set_text("T_v_judge_00", "—")
	for i in 18:
		get_scope("L_icon_team_%02d" % i)._set_text("T_lv_00", "Lv. —")
		get_scope("L_icon_team_%02d" % i)._set_text("T_check_00", "0")
	for i in 12:
		get_scope("L_i_tmc_%02d" % i)._set_text("T_lv_00", "Lv. —")
		get_scope("L_i_tmc_%02d" % i)._set_text("T_check_00", "0")
	for i in 90:
		get_scope("L_icon_box_%02d" % i)._set_text("T_lv_00", "Lv. —")
		get_scope("L_icon_box_%02d" % i)._set_text("T_check_00", "0")
	get_scope("L_switch_ms_00")._set_text("T_guide_00", "Select")
	get_scope("L_cursor_box_00")._set_text("T_num_catch_r_00", "0")
	get_scope("L_cursor_box_00")._set_text("T_info_poke_00", "No Pokémon")
	get_scope("L_cursor_box_00")._set_text("T_info_poke_01", "No Pokémon")
	get_scope("L_cursor_box_00")._set_text("T_info_poke_02", "No Pokémon")

func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
