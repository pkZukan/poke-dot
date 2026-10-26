#include "ui_pane_transform.h"

#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/transform3d.hpp>

using namespace godot;

namespace {
Vector2 xy(const Vector3 &v) { return Vector2(v.x, -v.y); }
Vector2 origin_fraction(int flags) {
    auto fraction = [](int v) -> real_t { return v == 1 ? 0 : v == 2 ? 1 : 0.5; };
    return Vector2(fraction(flags & 3), fraction((flags >> 2) & 3));
}

void resolve_pane(Control *pane, const Transform3D &parent_world, const Vector2 &parent_position,
        const Vector2 &parent_size, const Vector2 &parent_pivot) {
    Transform3D world = parent_world;
    Vector2 position = parent_position;
    Vector2 size = pane->get_size(), pivot;
    if (pane->has_meta("pane_transform")) {
        Dictionary state = pane->get_meta("pane_transform");
        Dictionary source = pane->get_meta("bflyt_pane");
        size = state["size"];
        pivot = size * origin_fraction(int(source["origin"]));
        Vector2 anchor = state["anchor"];
        if (pane->get_parent()->has_meta("pane_transform"))
            anchor = parent_pivot + parent_size * (origin_fraction(int(source["origin"]) >> 4) - Vector2(0.5, 0.5));
        Vector3 translation = state["translation"];
        Vector2 expected = anchor + xy(translation) - pivot;
        Vector2 previous = state["logical_position"];
        // Preserve explicit script offsets (for example the main-menu cursor).
        Vector2 offset = pane->get_position() - previous;
        pane->set_position(expected + offset);
        pane->set_size(size);
        state["logical_position"] = expected;
        pane->set_meta("pane_transform", state);

        Vector3 degrees = state["rotation"];
        Basis basis = Basis::from_euler(degrees * (Math_PI / 180.0), EULER_ORDER_ZYX);
        Vector2 scale = Vector2(state["scale"]) * Vector2(state["parts_scale"]);
        basis.scale_local(Vector3(scale.x, scale.y, 1));
        Vector2 top_left = pane->get_position();
        Vector3 origin(top_left.x + pivot.x, -top_left.y - pivot.y, translation.z);
        origin -= basis.xform(Vector3(pivot.x, -pivot.y, 0));
        world = parent_world * Transform3D(basis, origin);
        position += top_left;
        Transform2D projected(xy(world.basis.get_column(0)), -xy(world.basis.get_column(1)), xy(world.origin));
        pane->set_meta("pane_projected_transform", projected);
        if (Node2D *visual = UIPaneTransform::content(pane)) {
            projected.columns[2] -= position;
            visual->set_transform(projected);
        }
    }
    for (int i = 0; i < pane->get_child_count(); ++i) {
        Control *child = Object::cast_to<Control>(pane->get_child(i));
        if (child) resolve_pane(child, world, position, size, pivot);
    }
}
}

Node2D *UIPaneTransform::content(Control *pane) {
    return Object::cast_to<Node2D>(pane->get_node_or_null("Content"));
}

void UIPaneTransform::initialize(Control *pane, const Vector2 &anchor) {
    Dictionary source = pane->get_meta("bflyt_pane");
    Dictionary state;
    state["translation"] = source["translation"];
    state["rotation"] = source["rotation"];
    state["scale"] = source["scale"];
    state["size"] = source["size"];
    state["anchor"] = anchor;
    state["parts_scale"] = Vector2(1, 1);
    state["logical_position"] = pane->get_position();
    pane->set_meta("pane_transform", state);
}

void UIPaneTransform::set_channel(Control *pane, int channel, double value) {
    if (!pane->has_meta("pane_transform")) return;
    Dictionary state = pane->get_meta("pane_transform");
    if (channel < 3) {
        Vector3 v = state["translation"];
        v[channel] = value;
        state["translation"] = v;
    } else if (channel < 6) {
        Vector3 v = state["rotation"];
        v[channel - 3] = value;
        state["rotation"] = v;
    } else if (channel < 8) {
        Vector2 v = state["scale"];
        v[channel - 6] = value;
        state["scale"] = v;
    }
    pane->set_meta("pane_transform", state);
}

void UIPaneTransform::resolve(Control *layout) {
    resolve_pane(layout, Transform3D(), Vector2(), Vector2(), Vector2());
}

Transform2D UIPaneTransform::projected_transform(Control *pane) {
    Control *layout = pane;
    while (layout->has_meta("pane_transform")) {
        layout = Object::cast_to<Control>(layout->get_parent());
        if (!layout) return pane->get_global_transform();
    }
    return layout->get_global_transform() * Transform2D(pane->get_meta("pane_projected_transform", Transform2D()));
}
