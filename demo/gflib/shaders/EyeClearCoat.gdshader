shader_type spatial;
render_mode blend_premul_alpha, cull_disabled, depth_draw_never;

uniform sampler2D BaseColorMap1 : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D NormalMap1 : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D UpperEyelidColorMap : source_color, hint_default_transparent, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D LowerEyelidColorMap : source_color, hint_default_transparent, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D HighlightMaskMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;

uniform int sampler_repeat_u[5];
uniform int sampler_repeat_v[5];
uniform vec4 sampler_border_color[5];
#include "trinity_sampler.gdshaderinc"
#include "trinity_blend.gdshaderinc"
#define TRINITY_CUSTOM_LIGHT
#include "trinity_light.gdshaderinc"

uniform int EyelidType;
uniform int UVTransformMode;
uniform bool EnableBaseColorMap1 = true;
uniform bool EnableNormalMap1 = true;
uniform bool EnableUVScaleOffsetNormal;
uniform bool EnableHighlight;
uniform bool RequireHighlightMap;
uniform vec4 BaseColorClearCoat = vec4(1.0);
uniform float MetallicClearCoat;
uniform float RoughnessClearCoat = 0.1;
uniform float NormalHeight1 = 1.0;
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset3 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset4 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetNormal1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform float UVRotation;
uniform float UVRotation3;
uniform float UVRotation4;
uniform vec4 EmissionColorLayer7 = vec4(1.0);
uniform float EmissionIntensityLayer7;

vec2 coat_uv(vec2 uv, vec4 transform, float rotation) {
    vec2 result;
    if (UVTransformMode == 1) {
        result = uv - transform.zw;
    } else {
        vec2 centered = uv - vec2(0.5);
        float c = cos(rotation), s = sin(rotation);
        centered = mat2(vec2(c, -s), vec2(s, c)) * centered;
        result = (centered - transform.zw) * transform.xy + vec2(0.5);
    }
    return vec2(result.x, 1.0 - result.y);
}

void fragment() {
    if ((EyelidType == 1 || EyelidType == 3) && sample_trinity(UpperEyelidColorMap, coat_uv(UV, UVScaleOffset3, UVRotation3), 2).a > 0.0) discard;
    if ((EyelidType == 2 || EyelidType == 3) && sample_trinity(LowerEyelidColorMap, coat_uv(UV, UVScaleOffset4, UVRotation4), 3).a > 0.0) discard;
    vec2 uv = coat_uv(UV, UVScaleOffset, UVRotation);
    vec4 base = BaseColorClearCoat;
    if (EnableBaseColorMap1) base.rgb *= sample_trinity(BaseColorMap1, uv, 0).rgb;
    if (EnableNormalMap1) {
        vec2 normal_uv = uv;
        if (EnableUVScaleOffsetNormal) {
            normal_uv = (UV - UVScaleOffsetNormal1.zw) * UVScaleOffsetNormal1.xy;
            normal_uv.y = 1.0 - normal_uv.y;
        }
        vec2 xy = (sample_trinity(NormalMap1, normal_uv, 1).rg * 2.0 - 1.0) * NormalHeight1;
        NORMAL_MAP = vec3(xy, sqrt(max(0.0, 1.0 - dot(xy, xy)))) * 0.5 + 0.5;
    }
    ALBEDO = base.rgb;
    METALLIC = MetallicClearCoat;
    ROUGHNESS = RoughnessClearCoat;
    SPECULAR = 0.5; // Godot's dielectric F0 = 0.04 at this setting.
    vec2 blend_state = trinity_blend(base.a, 1.0);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
    EMISSION = vec3(0.0);
    if (EnableHighlight && RequireHighlightMap) {
        // Highlight mask intentionally omits the UV rotation.
        float mask = sample_trinity(HighlightMaskMap, coat_uv(UV, UVScaleOffset1, 0.0), 4).r;
        EMISSION = mask * EmissionIntensityLayer7 * EmissionColorLayer7.rgb;
    }
    TRINITY_LIGHT_FRAGMENT;
}

void light() {
    float attenuation = TRINITY_ATTENUATION(1);
    float nl = max(dot(NORMAL, TRINITY_LIGHT), 0.0);
    DIFFUSE_LIGHT += nl * LIGHT_COLOR * attenuation / PI;
    if (EnableHighlight && nl > 0.0) {
        vec3 half_vector = normalize(VIEW + TRINITY_LIGHT);
        float nv = max(dot(NORMAL, VIEW), 0.00001);
        float nh = clamp(dot(NORMAL, half_vector), 0.0, 1.0);
        float lh = clamp(dot(TRINITY_LIGHT, half_vector), 0.0, 1.0);
        float exponent = pow(2048.0, 1.0 - ROUGHNESS);
        vec3 f0 = mix(vec3(0.04), ALBEDO, METALLIC);
        vec3 fresnel = f0 + (vec3(1.0) - f0) * pow(1.0 - lh, 5.0);
        float k = 2.0 / sqrt(PI * (exponent + 2.0));
        float visibility = 1.0 / (mix(nl, 1.0, k) * mix(nv, 1.0, k));
        float distribution = (exponent + 2.0) * 0.5 * pow(nh, exponent);
        SPECULAR_LIGHT += distribution * fresnel * visibility * nl * LIGHT_COLOR * attenuation / PI;
    }
}
