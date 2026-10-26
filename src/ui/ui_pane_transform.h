#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot::UIPaneTransform 
{
void initialize(Control *pane, const Vector2 &anchor);
void set_channel(Control *pane, int channel, double value);
void resolve(Control *layout);
Node2D *content(Control *pane);
Transform2D projected_transform(Control *pane);
}
