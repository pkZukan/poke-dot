extends TrinityUI

var _entrance_states: Array[String] = []
var _entrance_index := -1

func configure_entrance(states: Array[String]) -> void:
	_entrance_states = states.duplicate()
	if not visibility_changed.is_connected(_on_playback_visibility_changed):
		visibility_changed.connect(_on_playback_visibility_changed)
	if not state_finished.is_connected(_on_entrance_state_finished):
		state_finished.connect(_on_entrance_state_finished)
	_on_playback_visibility_changed()

func replay_entrance() -> void:
	if not is_visible_in_tree() or _entrance_states.is_empty():
		return
	_entrance_index = 0
	_play_entrance_state()
	_on_entrance_started()

func _on_playback_visibility_changed() -> void:
	if is_visible_in_tree():
		replay_entrance()
	else:
		_entrance_index = -1
		stop_state(".")

func _play_entrance_state() -> void:
	var state := _entrance_states[_entrance_index]
	var error := play_state(".", state)
	if error != OK:
		_entrance_index = -1
		push_error("Failed to play %s UI state '%s': %s" % [name, state, error_string(error)])

func _on_entrance_state_finished(component: String, state: String) -> void:
	if component != "." or _entrance_index < 0:
		return
	if state != _entrance_states[_entrance_index]:
		return
	_entrance_index += 1
	if _entrance_index < _entrance_states.size():
		_play_entrance_state()
	else:
		_entrance_index = -1

func _on_entrance_started() -> void:
	pass
