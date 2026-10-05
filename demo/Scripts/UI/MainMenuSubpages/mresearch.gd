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
	
	_initialize_text()
	_initialize_state()

func _initialize_state() -> void:
	# Sample the completed entrance; apply_state does not play animations.
	apply_state(".", "in", 3.0)
	apply_state(".", "keep")
	apply_state(".", "reset_research")
	apply_state("L_rlv_00", "gauge", 0.0)
	apply_state("L_re_info_00", "switch_clear", 0.0)
	var research_button := get_scope("L_list_re_00").get_scope("L_btn_00")
	var component := str(get_pane(".").get_path_to(research_button))
	apply_state(component, "active")
	apply_state(component, "select")

func _initialize_text():
	get_scope("L_rlv_00")._set_text("T_v_next_pt_00", "0")
	get_scope("L_rlv_00")._set_text("T_u_next_pt_00", "pts to next level")
	for i in 9:
		get_scope("L_list_rw_%02d" % i)._set_text("T_rw_name_00", "No Reward")
		get_scope("L_list_rw_%02d" % i)._set_text("T_rw_num_00", "0")
		get_scope("L_list_rw_%02d" % i)._set_text("T_v_lv_00", "0")
	get_scope("L_list_rw_c_00")._set_text("T_rw_name_00", "No Reward")
	get_scope("L_list_rw_c_00")._set_text("T_rw_num_00", "0")
	get_scope("L_list_rw_c_00")._set_text("T_v_lv_00", "0")
	
	get_scope("L_list_re_00").get_scope("L_btn_00")._set_text("T_heading_00", "No research available")
	get_scope("L_list_re_00").get_scope("L_btn_00")._set_text("T_v_pt_00", "0")
	get_scope("L_list_re_00").get_scope("L_btn_00")._set_text("T_v_pt_01", "0")
	
	_set_text("T_sort_00", "Sort")
	
	var rw_info_00 = get_scope("L_rw_info_00")
	rw_info_00._set_text("T_w_id_00", "—")
	rw_info_00._set_text("T_w_name_00", "No Move")
	rw_info_00._set_text("T_w_detail_00", "Move details will appear here.")
	rw_info_00._set_text("T_classification_00", "Category")
	rw_info_00._set_text("T_power_00", "Power")
	rw_info_00._set_text("T_power_01", "—")
	rw_info_00._set_text("T_kaifuku_00", "Cooldown")
	rw_info_00._set_text("T_kaifuku_01", "—")
	rw_info_00._set_text("T_i_name_00", "No Reward")
	rw_info_00._set_text("T_i_detail_00", "Reward details will appear here.")
	
	var rw_next_00 = get_scope("N_rw_next_00")
	rw_next_00._set_text("T_rw_name_00", "No Reward")
	rw_next_00._set_text("T_rw_num_00", "0")
	
	var re_info_00 = get_scope("L_re_info_00")
	re_info_00._set_text("T_title_00", "Research")
	re_info_00._set_text("T_detail_00", "Select research to view its details.")
	re_info_00._set_text("T_task_00", "Objective")
	re_info_00._set_text("T_progress_00", "0 / 0")
	re_info_00._set_text("T_h_pt_00", "Research Points")
	re_info_00._set_text("T_v_pt_00", "0")
	re_info_00._set_text("T_u_pt_00", "pts")

func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
