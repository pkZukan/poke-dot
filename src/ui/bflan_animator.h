#pragma once

#include <godot_cpp/classes/animation_player.hpp>
#include "ui_state_runtime.h"

namespace godot {
    
class BflanAnimator : public AnimationPlayer {
    GDCLASS(BflanAnimator, AnimationPlayer)
protected:
    static void _bind_methods();
public:
    Error configure(Node *owner, Control *layout, const String &component, const String &state, double frame_rate);
    void set_frame(double value);
    double get_frame() const { return frame; }
    double get_frame_rate() const { return frame_rate; }
    String get_component() const { return component; }
    String get_state() const { return state; }
private:
    UIStateRuntime runtime;
    NodePath owner_path;
    NodePath layout_path;
    String component;
    String state;
    double frame = 0;
    double frame_rate = 60;
};
}
