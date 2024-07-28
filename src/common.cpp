#include "common.h"

using namespace godot;

void TRS::_bind_methods()
{
    GETTER_SETTER_BIND(TRS, Translation, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRS, Rotation, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRS, Scale, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

void BoundingBox::_bind_methods()
{
    GETTER_SETTER_BIND(BoundingBox, Min, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoundingBox, Max, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

void RGBA::_bind_methods()
{
    GETTER_SETTER_BIND(RGBA, R, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(RGBA, G, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(RGBA, B, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(RGBA, A, Variant::FLOAT, PROPERTY_HINT_NONE)
}