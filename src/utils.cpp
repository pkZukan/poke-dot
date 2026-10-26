#include "utils.h"
#include <cstring>

using namespace godot;

void Utils::_bind_methods()
{
    ClassDB::bind_static_method("Utils", D_METHOD("half_to_float", "half"), &Utils::half_to_float);
}

godot::Vector3 Utils::toGodotVec3(const Titan::Math::Vec3* vec)
{
    return Vector3(vec->x(), vec->y(), vec->z());
}

godot::Vector4 Utils::toGodotVec4(const Titan::Math::Vec4* vec)
{
    return Vector4(vec->x(), vec->y(), vec->z(), vec->w());
}

godot::Vector2i Utils::toGodotVec2i(const Titan::Math::Vec2i* vec)
{
    return Vector2i(vec->x(), vec->y());
}

godot::Vector3i Utils::toGodotVec3i(const Titan::Math::Vec3i* vec)
{
    return Vector3i(vec->x(), vec->y(), vec->z());
}

godot::Transform3D Utils::toGodotTransform(const Titan::Math::Transform* tran)
{
    Vector3 scale = toGodotVec3(&tran->scale());
    Vector3 translate = toGodotVec3(&tran->translate());

    Quaternion quat(tran->rotate().x(), tran->rotate().y(), tran->rotate().z(), tran->rotate().w());

    Basis basis(quat);
    basis.scale_local(scale);

    return Transform3D(basis, translate);
}

godot::Transform3D Utils::toGodotTransform(const Titan::Math::SRT* srt)
{
    Vector3 scale = toGodotVec3(srt->VecScale());
    Vector3 translate = toGodotVec3(srt->VecTranslate());
    Vector3 euler_rot = toGodotVec3(srt->VecRot());

    Basis basis = Basis::from_euler(euler_rot);
    basis.scale_local(scale);

    return Transform3D(basis, translate);
}

godot::String Utils::toGodotString(const flatbuffers::String* str)
{
    if (str == nullptr)
    {
        return String();
    }
    return String::utf8(str->c_str(), str->size());
}

float Utils::half_to_float(uint16_t half)
{
    uint32_t s = half >> 15;
    uint32_t e = half >> 10 & Mask(5);
    uint32_t f = half & Mask(10);

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

    // Reinterpret the assembled IEEE-754 bits; returning the integer would convert its value.
    uint32_t bits = s << 31 | e << 23 | f;
    float result;
    memcpy(&result, &bits, sizeof(result));
    return result;
}

// Round-to-nearest-even; overflow becomes infinity, tiny values become half denormals.
uint16_t Utils::float_to_half(float value)
{
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));

    uint16_t sign = uint16_t(bits >> 16 & 0x8000);
    uint32_t e = bits >> 23 & Mask(8);
    uint32_t f = bits & Mask(23);

    if (e == 255)
    {
        return sign | 0x7C00 | (f ? 0x200 : 0);
    }

    int exponent = int(e) - 127 + 15;

    if (exponent >= 31)
    {
        return sign | 0x7C00;
    }

    if (exponent <= 0)
    {
        if (exponent < -10)
        {
            return sign;
        }

        f |= Bit(23);
        uint32_t shift = uint32_t(14 - exponent);
        uint32_t half = f >> shift;
        uint32_t rest = f & (Bit(shift) - 1);
        uint32_t halfway = Bit(shift - 1);
        if (rest > halfway || (rest == halfway && (half & 1)))
        {
            half++;
        }
        return sign | uint16_t(half);
    }

    uint32_t half = uint32_t(exponent) << 10 | f >> 13;
    uint32_t rest = f & Mask(13);
    if (rest > Bit(12) || (rest == Bit(12) && (half & 1)))
    {
        half++; // a carry into the exponent is still correct, up to infinity
    }
    return sign | uint16_t(half);
}

String Utils::read_null_terminated_string(Ref<StreamPeerBuffer> sp, uint64_t end_pos)
{
    if (sp.is_null())
    {
        return String();
    }

    const PackedByteArray data = sp->get_data_array();
    const uint8_t* bytes = data.ptr();

    uint64_t pos = sp->get_position();
    uint64_t limit = MIN(end_pos, (uint64_t)data.size());

    if (pos >= limit)
    {
        return String();
    }

    uint64_t end = pos;
    while (end < limit && bytes[end] != 0x00)
    {
        end++;
    }

    String result = String::utf8((const char*)bytes + pos, end - pos);

    sp->seek(end < limit ? end + 1 : end);

    return result;
}

TreeItem* Utils::FindTreeItemByName(TreeItem* current, const String& target_name)
{
    if (current == nullptr)
    {
        return nullptr;
    }

    if (current->get_text(0) == target_name)
    {
        return current;
    }

    TreeItem* child = current->get_first_child();
    while (child != nullptr)
    {
        TreeItem* result = FindTreeItemByName(child, target_name);
        if (result != nullptr)
        {
            return result;
        }

        child = child->get_next();
    }

    return nullptr;
}
