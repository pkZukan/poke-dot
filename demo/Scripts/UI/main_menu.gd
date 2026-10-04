extends TrinityUI

signal option_selected(index: int)

#View files
const ARC_PATH := "res://Assets/ui/data/main_menu/main_menu_top_00_eng.arc"
const TRUIV_PATH := "res://Assets/ui/data/main_menu/view_main_menu_top_00.truiv"

#Consts
const menu_entry_names := ["Boxes", "Satchel", "Pokédex", "Mable's Research", "Z-A Royale", "Link Play"]

# Store ui scene instances
@export var submenu_scenes: Array[PackedScene] = [null, null, null, null, null, null]

#Vars
var submenus: Dictionary = {}
var active_submenu: Control = null

var selected_idx: int = 0

func _ready() -> void:
	visible = false
	GameManager.state_changed.connect(_on_state_changed)
	var error := load_ui(TRUIV_PATH, ARC_PATH, ^"canvas")
	if error != OK:
		push_error("Failed to load main UI: %s" % error_string(error))
		return

	_initialize()
	_initialize_submenus()
	option_selected.connect(_on_option_selected)

func _initialize_submenus() -> void:
	for i in mini(submenu_scenes.size(), menu_entry_names.size()):
		if submenu_scenes[i] == null:
			continue
		var instance := submenu_scenes[i].instantiate()
		var submenu := instance as Control
		if submenu == null:
			push_error("Submenu %s must have a Control root." % menu_entry_names[i])
			instance.free()
			continue
		submenu.hide()
		submenu.process_mode = Node.PROCESS_MODE_DISABLED
		if submenu.has_signal("back_requested"):
			submenu.connect("back_requested", _on_submenu_back_requested.bind(submenu))
		add_child(submenu)
		submenus[i] = submenu

func _on_option_selected(index: int) -> void:
	if not submenus.has(index) or active_submenu != null:
		return
	active_submenu = submenus[index]
	$canvas.hide()
	active_submenu.process_mode = Node.PROCESS_MODE_INHERIT
	active_submenu.show()

func _on_submenu_back_requested(submenu: Control) -> void:
	if submenu == active_submenu:
		_return_to_main_menu()

func _return_to_main_menu() -> void:
	if active_submenu != null:
		active_submenu.hide()
		active_submenu.process_mode = Node.PROCESS_MODE_DISABLED
		active_submenu = null
	$canvas.show()

func _initialize() -> void:
	apply_state(".", "in", 4.0)
	apply_state(".", "ptn_menu")
	apply_state(".", "ptn_progress")
	apply_state(".", "ptn_quest_parts")
	apply_state(".", "ptn_quest_text", 3.0)
	apply_state(".", "ptn_quest_icon")

	for i in menu_entry_names.size():
		var component := "L_menu_button_%02d" % i
		var part := get_pane(component)
		apply_state(component, "active")
		apply_state(component, "ptn_icon", i)
		apply_state(component, "ptn_time")
		apply_state(component, "ptn_inf")
		apply_state(component, "select" if i == selected_idx else "unselect", 4.0)
		for label_name in ["T_00", "T_01"]:
			var label := part.find_child(label_name, true, false) as Label
			if label:
				label.text = menu_entry_names[i]
	for i in 6:
		var component := "L_temochi_btn_%02d" % i
		apply_state(component, "empty")
		apply_state(component, "unselect")
		apply_state(component, "item_off")
		apply_state(component, "exp_out", 5.0)
		apply_state(component, "lvup_out", 6.0)
		apply_state(component, "num_select_on")
	set_money(0)
	set_research_level(0)
	set_research_exp(0, 1)
	set_quest("No tracked quest", "")
	set_next_award("-", "")
	var research := get_pane("L_mr_00")
	var reward_label := research.find_child("T_option_00", true, false) as Label
	if reward_label:
		reward_label.text = "Reward List"
	var map_label := get_pane("L_opguide_03").find_child("T_option_00", true, false) as Label
	if map_label:
		map_label.text = "Map"
	_update_cursor()

func _move_selection(direction: int) -> void:
	apply_state("L_menu_button_%02d" % selected_idx, "unselect", 4.0)
	selected_idx = wrapi(selected_idx + direction, 0, menu_entry_names.size())
	apply_state("L_menu_button_%02d" % selected_idx, "select", 4.0)
	_update_cursor()

func _update_cursor() -> void:
	var cursor := get_pane("L_cursor_00")
	var button := get_pane("L_menu_button_%02d" % selected_idx)
	if not cursor or not button:
		return
	var anchor := button.find_child("N_cursor", true, false) as Control
	if anchor:
		var target := anchor.get_global_transform() * (anchor.size * 0.5)
		cursor.position = cursor.get_parent().get_global_transform().affine_inverse() * target - cursor.size * 0.5

# Set money display
func set_money(amount: int) -> void:
	_set_text("T_money_00", str(maxi(amount, 0)))

# Set research lvl
func set_research_level(level: int) -> void:
	var display_level := clampi(level, 0, 99)
	apply_state("L_mr_lv_num_01", "ptn", floori(display_level / 10.0))
	apply_state("L_mr_lv_num_00", "ptn", display_level % 10)

# Set exp ring
func set_research_exp(current_exp: int, required_exp: int) -> void:
	var progress := 0.0
	if required_exp > 0:
		progress = clampf(float(current_exp) / float(required_exp), 0.0, 1.0)
	# The authored gauge spans frames 0–100. Its ring also requires wnd1
	# rendering and secondary-texture transforms, not yet supported by TrinityUI.
	apply_state("L_mr_gauge_00", "gauge", progress * 100.0)

func set_quest(name: String, desc: String):
	_set_text("T_quest_name_00", name)
	_set_text("T_quest_03", desc)

func set_next_award(name: String, num: String):
	_set_text("T_mrrw_name_00", name)
	_set_text("T_mrrw_num_00", num)

func _set_text(pane_name: String, text: String) -> void:
	var label := get_pane(pane_name) as Label
	if label:
		label.text = text

func _on_state_changed(new_state: GameManager.GameState) -> void:
	visible = (new_state == GameManager.GameState.STATE_PAUSED)
	if not visible:
		_return_to_main_menu()

func _input(event: InputEvent) -> void:
	if not is_visible_in_tree():
		return

	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("main_menu"):
		get_viewport().set_input_as_handled()
		if active_submenu != null:
			_return_to_main_menu()
		else:
			GameManager.set_state(GameManager.GameState.STATE_PLAYING)
		return

	if active_submenu != null:
		return

	if event.is_action_pressed("ui_down"):
		_move_selection(1)
	elif event.is_action_pressed("ui_up"):
		_move_selection(-1)
	elif event.is_action_pressed("ui_accept"):
		# Consume the event before a listener opens another UI.
		get_viewport().set_input_as_handled()
		option_selected.emit(selected_idx)
		return
	else:
		return

	get_viewport().set_input_as_handled()
