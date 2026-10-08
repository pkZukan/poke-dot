extends "res://Scripts/UI/animated_ui.gd"

signal back_requested

const ARC_PATH := "res://Assets/ui/data/ryl/ryl_top_00_eng.arc"
const TRUIV_PATH := "res://Assets/ui/data/ryl/view_ryl_top_00.truiv"

func _ready() -> void:
	hide()
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load Ryl UI: %s" % error_string(error))
		return

	# Hide dummy pane
	#get_pane("P_ofsc_00").hide()
	
	_initialize_text()
	_initialize_state()
	configure_entrance(["in", "f_in_keep", "keep"])

func _initialize_state() -> void:
	apply_state(".", "reset_ryl")
	apply_state(".", "ptn_time", 0.0)
	apply_state(".", "ptn_a_rank", 0.0)
	apply_state(".", "ptn_info_text", 0.0)
	apply_state(".", "ptn_card", 0.0)
	for i in 2:
		var component := "L_tab_item_%02d" % i
		apply_state(component, "ptn_tab", i)
		apply_state(component, "ptn_clear", 0.0)
		apply_state(component, "ptn_time", 0.0)
		apply_state(component, "active" if i == 0 else "passive")

func _initialize_text():
	_set_text("T_a_rank_00", "Rank —")
	_set_text("T_npc_name_00", "—")
	_set_text("T_ef_name_00", "—")
	_set_text("T_medal_pt_00", "0")
	_set_text("T_medal_00", "Medals")
	_set_text("T_win_bonus_00", "Win Bonus")
	_set_text("T_win_bonus_01", "0")
	_set_text("T_card_title_00", "Bonus Cards")
	_set_text("T_ryl_info_00", "The Z-A Royale takes place at night.")
	_set_text("T_a_rank_info_00", "Rank-up details will appear here.")
	_set_text("T_player_name_00", "Trainer")
	_set_text("T_pt_00", "0")
	_set_text("T_pt_01", "/")
	_set_text("T_pt_02", "0")
	_set_text("T_ticket_get_00", "No ticket available")
	_set_text("T_ticket_00", "Ticket Points")
	_set_text("T_ticket_pt_00", "0")
	_set_text("T_rewards_wins_00", "Total Wins")
	_set_text("T_wins_num_00", "0")
	for i in 3:
		var card = get_scope("L_card_%02d" % i)
		card._set_text("T_card_00", "No bonus")
		card._set_text("T_card_num_00", "0")
		card._set_text("T_pt_win_00", "0")
		card._set_text("T_pt_lose_00", "0")
		card._set_text("T_card_pt_00", "0")
		card._set_text("T_medal_lose_00", "0")
		card._set_text("T_medal_win_00", "0")
		card._set_text("T_card_medal_00", "0")
	for i in 2:
		var tab_item = get_scope("L_tab_item_%02d" % i)
		tab_item._set_text("T_tab_00", "Rank Up")
		tab_item._set_text("T_tab_01", "Rewards")

func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		back_requested.emit()
