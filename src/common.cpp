#include "common.h"

using namespace godot;

void BoundingBox::_bind_methods()
{
    GETTER_SETTER_BIND(BoundingBox, Min, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoundingBox, Max, Variant::VECTOR3, PROPERTY_HINT_NONE)
}