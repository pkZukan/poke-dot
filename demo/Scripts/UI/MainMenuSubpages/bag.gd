extends "res://Scripts/UI/animated_ui.gd"

signal back_requested

const ARC_PATH := "res://Assets/ui/data/bag/bag_top_00_eng.arc"
const TRUIV_PATH := "res://Assets/ui/data/bag/view_bag_top_00.truiv"

func _ready() -> void:
	hide()
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load Bag UI: %s" % error_string(error))
		return

	# Hide dummy pane
	get_pane("P_ofsc_00").hide()
	
	_initialize_text()
	_initialize_state()
	configure_entrance(["in", "keep"])

func _initialize_state() -> void:
	apply_state(".", "noitem_bag")
	for i in 8:
		var tab := get_scope("L_tab_00").get_scope("L_icon_%02d" % i)
		var component := str(get_pane(".").get_path_to(tab))
		apply_state(component, "ptn_icon", i)
		apply_state(component, "active" if i == 0 else "passive")
	for i in 7:
		var component := "L_temochi_btn_%02d" % i
		apply_state(component, "empty")
		apply_state(component, "unselect")
		apply_state(component, "item_off")
		apply_state(component, "exp_out", 6.0)
		apply_state(component, "lvup_out", 6.0)

func _initialize_text():
	var tab_00 = get_scope("L_tab_00")
	tab_00.get_scope("L_icon_00")._set_text("T_title_tab_00", "Medicine")
	tab_00.get_scope("L_icon_01")._set_text("T_title_tab_00", "Poké Balls")
	tab_00.get_scope("L_icon_02")._set_text("T_title_tab_00", "Battle Items")
	tab_00.get_scope("L_icon_03")._set_text("T_title_tab_00", "Berries")
	tab_00.get_scope("L_icon_04")._set_text("T_title_tab_00", "Other Items")
	tab_00.get_scope("L_icon_05")._set_text("T_title_tab_00", "TMs")
	tab_00.get_scope("L_icon_06")._set_text("T_title_tab_00", "Treasures")
	tab_00.get_scope("L_icon_07")._set_text("T_title_tab_00", "Key Items")
	
	get_scope("L_list_item_00").get_scope("L_button_item_00")._set_text("T_name_00", "No Item")
	get_scope("L_list_item_00").get_scope("L_button_item_00")._set_text("T_num_00", "0")
	
	get_scope("L_list_item_01").get_scope("L_button_item_00")._set_text("T_name_00", "No Item")
	get_scope("L_list_item_01").get_scope("L_button_item_00")._set_text("T_num_01", "0")
	get_scope("L_list_item_01").get_scope("L_button_item_00")._set_text("T_price_00", "0")
	get_scope("L_list_item_01").get_scope("L_button_item_00")._set_text("T_num_00", "0")
	
	_set_text("T_noitem_00", "No items")
	_set_text("T_detail_00", "Select an item to view its details.")
	_set_text("T_id_00", "—")
	_set_text("T_name_01", "No Move")
	_set_text("T_detail_01", "Select a TM to view its move.")
	_set_text("T_classification_00", "Category")
	_set_text("T_power_00", "Power")
	_set_text("T_power_01", "—")
	_set_text("T_kaifuku_00", "Cooldown")
	_set_text("T_kaifuku_01", "—")
	_set_text("T_poke_lv_00", "Lv. —")
	_set_text("T_poke_name_00", "No Pokémon")
	get_scope("N_status_00")._set_text("./T_hp_num_00", "0")
	_set_text("T_exp_num_00", "0")
	_set_text("T_exp_num_01", "/")
	_set_text("T_exp_num_02", "0")
	_set_text("T_item_name_00", "None")
	get_scope("N_sort")._set_text("T_option_00", "Sort")
	for i in 7:
		var hp := get_scope("L_temochi_00").get_scope("L_temochi_btn_%02d" % i).get_scope("L_hp_00")
		hp._set_text("T_hp_num_00", "0")
		hp._set_text("T_hp_num_01", "/")
		hp._set_text("T_hp_num_02", "0")
		get_scope("L_temochi_00").get_scope("L_temochi_btn_%02d" % i)._set_text("T_lv_num_00", "Lv. —")
		get_scope("L_temochi_00").get_scope("L_temochi_btn_%02d" % i)._set_text("T_exp_00", "0")
	get_scope("bag_top_00/all/N_inout_03/N_inout_06/N_sec_item_01/P_bg_item_00")._set_text("T_name_00", "No Item")
	get_scope("L_gauge_hp_00")._set_text("T_hp_num_00", "0")
	get_scope("L_gauge_hp_00")._set_text("T_hp_num_01", "/")
	get_scope("L_gauge_hp_00")._set_text("T_hp_num_02", "0")
	for i in 4:
		get_scope("L_skill_%02d" % i)._set_text("T_00", "—")
	
	get_scope("L_numselect_00")._set_text("T_num_00", "0")
	get_scope("L_numselect_00")._set_text("T_slash_00", "/")
	get_scope("L_numselect_00")._set_text("T_num_01", "0")
	get_scope("L_numselect_00").get_scope("L_key_00")._set_text("T_option_00", "Confirm")
	get_scope("L_numselect_00").hide()

func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
