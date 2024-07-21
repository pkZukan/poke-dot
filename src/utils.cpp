#include "utils.h"

using namespace godot;

void Utils::_bind_methods()
{
    ClassDB::bind_static_method("Utils", D_METHOD("half_to_float", "half"), &Utils::half_to_float);
}

godot::Vector3 Utils::toGodotVec3(const Titan::Model::Vec3 *vec)
{
    return Vector3(vec->x(), vec->y(), vec->z());
}

godot::String Utils::toGodotString(const flatbuffers::String *str)
{
    return String(str->c_str());
}



float Utils::half_to_float(uint16_t half)
{
    uint32_t s = half >> 15;
    uint32_t e = half >> 10 & Mask( 5);
    uint32_t f = half       & Mask(10);

    f <<= 23 - 10;

    switch (e)
    {
        case 0:
            if (f != 0)
            {
                e = 1 + (127 - 15);
                while (f < Bit(23))
                {
                    f <<= 1;
                    e -= 1;
                }

                f &= Mask(23);
            }
            break;

        default:
            e += 127 - 15; 
            break;

        case 31:
            e = 255; 
            break;
    }

    return s << 31 | e << 23 | f;
}