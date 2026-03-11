#include "anim_common.h"

using namespace godot;

void TRAnimationInfo::_bind_methods()
{
    GETTER_SETTER_BIND(TRAnimationInfo, does_loop, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationInfo, animation_count, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRAnimationInfo, animation_rate, Variant::INT, PROPERTY_HINT_NONE)
}