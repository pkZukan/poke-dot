#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include "middleware/sarc.h"
#include "ui/bflan_animator.h"
#include "ui_builder.h"
#include "ui_pane.h"
#include "middleware/wwise.h"

namespace godot
{

class TrinityUI : public TrinityPane {
    GDCLASS(TrinityUI, TrinityPane)

protected:
    static void _bind_methods();
    void _notification(int what);
    Control *scope_root() const override;

public:
    Error load_ui(const String &truiv_path, const String &arc_path, const NodePath &parent_path);
    Ref<Font> get_font(const String &p_name);
    Error apply_state(const String &component, const String &state, double frame = 0.0);
    Error play_state(const String &component, const String &state, double frame_rate = 60.0);
    Error pause_state(const String &component, bool paused = true);
    void stop_state(const String &component);
    Error seek_state(const String &component, double frame);
    bool is_state_playing(const String &component) const;
    double get_state_frame(const String &component) const;
    BflanAnimator *get_state_animator(const String &component) const;
    PackedStringArray get_warnings() const;
    Error play_ui_sound(const String &event_name);
    void set_sound_bank(const Ref<WwiseSoundBank> &bank) { sound_bank = bank; missing_sound_events.clear(); }
    Ref<WwiseSoundBank> get_sound_bank() const { return sound_bank; }
    void set_sound_enabled(bool enabled) { sound_enabled = enabled; }
    bool is_sound_enabled() const { return sound_enabled; }

private:
    void play_animation_sound(const String &component, const String &state, const String &event);
    Ref<WwiseSoundBank> sound_bank;
    bool sound_enabled = true;
    PackedStringArray missing_sound_events;
    Control *layout_node() const;
    void clear_loaded_ui();
    void fit_layout();

    void on_state_finished(const StringName &animation, const String &root);
    UIStateRuntime state_runtime;
    std::map<String, NodePath> animators;
    UIFontResolver fonts;
    PackedStringArray warnings;
    Control *layout = nullptr;
    uint64_t layout_id = 0;
};

}
