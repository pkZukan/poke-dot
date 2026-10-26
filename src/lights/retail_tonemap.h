#pragma once

#include <godot_cpp/classes/image_texture3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot
{

struct RetailToneCurve
{
    struct Segment
    {
        float offset_x = 0, offset_y = 0, scale_x = 1, scale_y = 1, ln_a = 0, b = 1;
        float eval(float x) const;
    };

    bool enabled = false;
    float x0 = 0, x1 = 0;
    Segment segments[3];

    static RetailToneCurve create(double toe_strength, double toe_length, double shoulder_strength,
                                  double shoulder_length, double shoulder_angle, double gamma);
    float eval(float x) const;
};

Vector3 retail_aces(const Vector3& color);
Ref<ImageTexture3D> build_retail_tonemap_lut(const RetailToneCurve& curve, int size, float headroom);

}
