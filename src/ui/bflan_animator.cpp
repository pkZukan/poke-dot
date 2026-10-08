#include "bflan_animator.h"

#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <cmath>

using namespace godot;

void BflanAnimator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_frame", "frame"), &BflanAnimator::set_frame);
    ClassDB::bind_method(D_METHOD("get_frame"), &BflanAnimator::get_frame);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frame", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_frame", "get_frame");
}

Error BflanAnimator::configure(Node *owner, Control *layout, const String &p_component, const String &p_state, double p_frame_rate) {
    ERR_FAIL_COND_V(!std::isfinite(p_frame_rate) || p_frame_rate <= 0, ERR_INVALID_PARAMETER);
    runtime.reset(owner, layout);
    String root;
    Dictionary data;
    Error error = runtime.resolve(owner, layout, p_component, p_state, root, data);
    if (error != OK) return error;

    Ref<Animation> animation;
    animation.instantiate();
    double frame_count = data["frame_count"];
    double duration = MAX(frame_count / p_frame_rate, 0.001);
    animation->set_length(duration);
    animation->set_loop_mode(bool(data["loop"]) ? Animation::LOOP_LINEAR : Animation::LOOP_NONE);
    int track = animation->add_track(Animation::TYPE_VALUE);
    animation->track_set_path(track, NodePath(".:frame"));
    animation->track_set_interpolation_type(track, Animation::INTERPOLATION_LINEAR);
    animation->track_set_interpolation_loop_wrap(track, false);
    animation->value_track_set_update_mode(track, Animation::UPDATE_CONTINUOUS);
    animation->track_insert_key(track, 0.0, 0.0);
    animation->track_insert_key(track, duration, frame_count);
    Ref<AnimationLibrary> library;
    library.instantiate();
    error = library->add_animation("clip", animation);
    if (error != OK) return error;

    stop(true);
    if (has_animation_library("")) remove_animation_library("");
    error = add_animation_library("", library);
    if (error != OK) return error;
    set_root(NodePath("."));
    owner_path = get_path_to(owner);
    layout_path = owner->get_path_to(layout);
    component = p_component;
    state = p_state;
    frame_rate = p_frame_rate;
    set_frame(0);
    return OK;
}

void BflanAnimator::set_frame(double value) {
    if (!std::isfinite(value)) return;
    Node *owner = get_node_or_null(owner_path);
    if (!owner) return;
    Control *layout = Object::cast_to<Control>(owner->get_node_or_null(layout_path));
    if (!layout) return;
    Error error = runtime.apply(owner, layout, component, state, value);
    ERR_FAIL_COND_MSG(error != OK, "Could not apply BFLAN animation frame.");
    frame = value;
}
