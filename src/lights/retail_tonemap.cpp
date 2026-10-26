#include "retail_tonemap.h"
#include "utils.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <algorithm>
#include <cmath>

namespace godot
{

namespace
{

// Constants as compiled into the retail shader (its c1 bank), in RGB order.
const float SCENE_TO_AP0[3][3] = {{0.0791461766f, 0.068936035f, 0.0319202989f},
                                  {0.0161626134f, 0.146416128f, 0.0174170863f},
                                  {0.00315791974f, 0.0200779196f, 0.156726718f}};
const float AP0_TO_AP1[3][3] = {{1.45143926f, -0.236510754f, -0.214928567f},
                                {-0.0765537769f, 1.17622972f, -0.0996759236f},
                                {0.00831614807f, -0.00603244966f, 0.997716308f}};
const float AP1_TO_AP0[3][3] = {{0.695452213f, 0.140678704f, 0.163869068f},
                                {0.0447945632f, 0.859671116f, 0.0955343172f},
                                {-0.00552588282f, 0.00402521016f, 1.00150073f}};
const float AP1_TO_XYZ[3][3] = {{0.662454188f, 0.134004205f, 0.156187683f},
                                {0.272228718f, 0.674081743f, 0.0536895171f},
                                {-0.00557464967f, 0.0040607336f, 1.01033914f}};
const float XYZ_TO_AP1[3][3] = {{1.6410234f, -0.324803293f, -0.236424699f},
                                {-0.663662851f, 1.61533165f, 0.0167563483f},
                                {0.0117218941f, -0.00828444213f, 0.988394856f}};
const float D60_TO_D65[3][3] = {{0.987223983f, -0.00611326983f, 0.0159533005f},
                                {-0.00759836007f, 1.00186002f, 0.00533019984f},
                                {0.00307257008f, -0.00509594986f, 1.08168006f}};
const float XYZ_TO_SRGB[3][3] = {{3.2409699f, -1.5373832f, -0.498610765f},
                                 {-0.969243646f, 1.8759675f, 0.0415550582f},
                                 {0.0556300804f, -0.203976959f, 1.05697155f}};
const float AP1_Y[3] = {0.272228986f, 0.674081981f, 0.0536894985f};

Vector3 mul(const float m[3][3], const Vector3& v)
{
    return Vector3(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z, m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                   m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

Vector3 desaturate(const Vector3& v, float amount)
{
    float y = AP1_Y[0] * v.x + AP1_Y[1] * v.y + AP1_Y[2] * v.z;
    return Vector3(y, y, y) + (v - Vector3(y, y, y)) * amount;
}

float film_curve(float x)
{
    return x * (x * 2.785085f + 0.107772f) / (x * (x * 2.936045f + 0.887121975f) + 0.806889f);
}

float srgb_to_linear(float c)
{
    return c < 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

float linear_to_srgb(float c)
{
    c = std::max(c, 0.0f);
    return c < 0.0031308f ? c * 12.92f : 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
}

}

float RetailToneCurve::Segment::eval(float x) const
{
    float t = (x - offset_x) * scale_x;
    float y = t > 0 ? std::exp(ln_a + b * std::log(t)) : 0.0f;
    return y * scale_y + offset_y;
}

// Hable's CalcDirectParamsFromUser + CreateCurve. The retail shader evaluates the
// W-normalized curve on its input directly.
RetailToneCurve RetailToneCurve::create(double toe_strength, double toe_length, double shoulder_strength,
                                        double shoulder_length, double shoulder_angle, double gamma)
{
    auto saturate = [](double v)
    {
        return std::clamp(v, 0.0, 1.0);
    };

    toe_length = std::pow(saturate(toe_length), 2.2);
    toe_strength = saturate(toe_strength);
    shoulder_angle = saturate(shoulder_angle);
    shoulder_length = std::max(1e-5, saturate(shoulder_length));
    shoulder_strength = std::max(0.0, shoulder_strength);

    double x0 = toe_length * 0.5;
    double y0 = (1.0 - toe_strength) * x0;
    double remaining_y = 1.0 - y0;
    double y1_offset = (1.0 - shoulder_length) * remaining_y;
    double x1 = x0 + y1_offset;
    double y1 = y0 + y1_offset;
    double w = x0 + remaining_y + std::exp2(shoulder_strength) - 1.0;
    double overshoot_x = 2.0 * shoulder_angle * shoulder_strength; // (2W * angle * strength) / W
    double overshoot_y = 0.5 * shoulder_angle * shoulder_strength;

    x0 /= w;
    x1 /= w;

    double m = x1 == x0 ? 1.0 : (y1 - y0) / (x1 - x0);
    double b = y0 - x0 * m;
    auto derivative = [&](double x)
    {
        return gamma * m * std::pow(m * x + b, gamma - 1.0);
    };
    double toe_m = derivative(x0);
    double shoulder_m = derivative(x1);

    y0 = std::max(1e-5, std::pow(y0, gamma));
    y1 = std::max(1e-5, std::pow(y1, gamma));
    overshoot_y = std::pow(1.0 + overshoot_y, gamma) - 1.0;

    RetailToneCurve curve;
    curve.enabled = true;
    curve.x0 = float(x0);
    curve.x1 = float(x1);

    Segment& toe = curve.segments[0];
    toe.b = float(toe_m * x0 / y0);
    toe.ln_a = float(std::log(y0) - toe.b * std::log(x0));

    Segment& mid = curve.segments[1];
    mid.offset_x = float(-(b / m));
    mid.ln_a = float(gamma * std::log(m));
    mid.b = float(gamma);

    Segment& shoulder = curve.segments[2];
    double sx = 1.0 + overshoot_x - x1;
    double sy = 1.0 + overshoot_y - y1;
    shoulder.b = float(shoulder_m * sx / sy);
    shoulder.ln_a = float(std::log(sy) - shoulder.b * std::log(sx));
    shoulder.offset_x = float(1.0 + overshoot_x);
    shoulder.offset_y = float(1.0 + overshoot_y);
    shoulder.scale_x = -1;
    shoulder.scale_y = -1;

    float inv_scale = 1.0f / shoulder.eval(1.0f);

    for (Segment& segment : curve.segments)
    {
        segment.offset_y *= inv_scale;
        segment.scale_y *= inv_scale;
    }

    return curve;
}

float RetailToneCurve::eval(float x) const
{
    if (!enabled)
    {
        return x;
    }

    return segments[x < x0 ? 0 : (x < x1 ? 1 : 2)].eval(x);
}

Vector3 retail_aces(const Vector3& color)
{
    Vector3 ap0 = mul(AP1_TO_AP0, mul(AP0_TO_AP1, mul(SCENE_TO_AP0, color / 0.18f)));
    float r = ap0.x, g = ap0.y, b = ap0.z;

    // RRT glow module.
    float hi = std::max({r, g, b}), lo = std::min({r, g, b});
    float saturation = (std::max(hi, 1e-4f) - std::max(lo, 1e-4f)) / std::max(hi, 0.01f);
    float yc = (r + g + b + 1.75f * std::sqrt(std::max(0.0f, b * (b - g) + g * (g - r) + r * (r - b)))) / 3.0f;
    float s = (saturation - 0.4f) * 5.0f;
    float sign = float((s > 0) - (s < 0));
    float t = std::max(0.0f, 1.0f - std::abs(s) * 0.5f);
    float gain = 0.05f * 0.5f * (1.0f + sign * (1.0f - t * t));
    float glow = yc <= 0.08f * 2.0f / 3.0f ? gain : (yc >= 0.16f ? 0.0f : gain * (0.08f / yc - 0.5f));
    r *= 1.0f + glow;
    g *= 1.0f + glow;
    b *= 1.0f + glow;

    // RRT red modifier.
    float hue = (r == g && g == b) ? 0.0f : std::atan2(std::sqrt(3.0f) * (g - b), 2.0f * r - g - b) * 57.2957795f;
    if (hue < 0)
    {
        hue += 360.0f;
    }
    if (hue > 180.0f)
    {
        hue -= 360.0f;
    }
    float weight = std::clamp(1.0f - std::abs(hue * (2.0f / 135.0f)), 0.0f, 1.0f);
    weight = weight * weight * (3.0f - 2.0f * weight);
    r += weight * weight * saturation * (0.03f - r) * 0.18f;

    Vector3 working = mul(AP0_TO_AP1, Vector3(r, g, b)).maxf(0.0f);
    working = desaturate(working, 0.96f);
    working = Vector3(film_curve(working.x), film_curve(working.y), film_curve(working.z));

    // ODT: dark to dim surround in xyY, then desaturate.
    Vector3 xyz = mul(AP1_TO_XYZ, working);
    float dim_y = xyz.y > 0 ? std::exp2(std::log2(std::min(xyz.y, 65504.0f)) * 0.9811f) : 0.0f;
    float sum = std::max(xyz.x + xyz.y + xyz.z, 1e-4f);
    float cx = xyz.x / sum, cy = xyz.y / sum;
    float k = dim_y / std::max(cy, 1e-4f);
    Vector3 ap1 = desaturate(mul(XYZ_TO_AP1, Vector3(cx * k, dim_y, (1.0f - cx - cy) * k)), 0.93f);

    return mul(XYZ_TO_SRGB, mul(D60_TO_D65, mul(AP1_TO_XYZ, ap1)));
}

Ref<ImageTexture3D> build_retail_tonemap_lut(const RetailToneCurve& curve, int size, float headroom)
{
    TypedArray<Image> slices;
    PackedByteArray pixels;
    pixels.resize(size * size * 4 * sizeof(uint16_t));
    float scale = 1.0f / float(size - 1);
    const uint16_t one = Utils::float_to_half(1.0f);

    for (int z = 0; z < size; ++z)
    {
        uint16_t* out = reinterpret_cast<uint16_t*>(pixels.ptrw());

        for (int y = 0; y < size; ++y)
        {
            for (int x = 0; x < size; ++x)
            {
                Vector3 scene(srgb_to_linear(x * scale), srgb_to_linear(y * scale), srgb_to_linear(z * scale));
                Vector3 display = retail_aces(scene * headroom);
                *out++ = Utils::float_to_half(linear_to_srgb(curve.eval(display.x)));
                *out++ = Utils::float_to_half(linear_to_srgb(curve.eval(display.y)));
                *out++ = Utils::float_to_half(linear_to_srgb(curve.eval(display.z)));
                *out++ = one;
            }
        }

        slices.push_back(Image::create_from_data(size, size, false, Image::FORMAT_RGBAH, pixels));
    }

    Ref<ImageTexture3D> lut;
    lut.instantiate();
    lut->create(Image::FORMAT_RGBAH, size, size, size, false, slices);
    return lut;
}

}
