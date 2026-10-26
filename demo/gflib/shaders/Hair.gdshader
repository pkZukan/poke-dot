shader_type spatial;
render_mode cull_disabled;

uniform sampler2D BaseColorMap : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D NormalMap : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D HairFlowMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D MetallicMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D DiscardMaskMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D ColorTableMap : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;

uniform int sampler_repeat_u[7];
uniform int sampler_repeat_v[7];
uniform vec4 sampler_border_color[7];
#include "trinity_sampler.gdshaderinc"
#define TRINITY_CUSTOM_LIGHT
#include "trinity_light.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableNormalMap;
uniform bool EnableFlowMap;
uniform bool EnableScatterMap;
uniform bool EnableRoughnessMap;
uniform bool EnableAlphaTest;
uniform bool EnableDiscardMaskMap;
uniform bool EnableColorTableMap;

uniform vec4 BaseColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);

uniform float BaseColorIndex1 = 1.0;
uniform float DiscardValue = 0.5;
uniform float DiscardMaskValue = 0.5;
uniform float NormalHeight = 1.0;
uniform float HairHighlightShift;
uniform float Metallic = 1.0;
uniform float Roughness = 0.5;

varying vec3 hair_direction;

varying float hair_scatter;

vec2 hair_uv(vec2 uv, vec4 transform) {
    vec2 result = (uv - transform.zw) * transform.xy;
    return vec2(result.x, 1.0 - result.y);
}

float hair_fresnel(float cosine) {
    // Index of refraction 1.55, as in the original fiber model.
    float f0 = (0.55 / 2.55) * (0.55 / 2.55);
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosine, 0.0, 1.0), 5.0);
}

float hair_gaussian(float width, float angle) {
    return exp(-0.5 * angle * angle / (width * width)) / (2.50663 * width);
}

vec3 hair_specular(vec3 color, float roughness, vec3 direction, vec3 light_dir, vec3 view_dir) {
    float sin_l = clamp(dot(direction, light_dir), -1.0, 1.0);
    float sin_v = clamp(dot(direction, view_dir), -1.0, 1.0);
    float cos_l = sqrt(max(0.0, 1.0 - sin_l * sin_l));
    float cos_v = sqrt(max(0.0, 1.0 - sin_v * sin_v));
    float cos_d = max(sqrt(max(0.0, (1.0 + cos_l * cos_v + sin_v * sin_l) * 0.5)), 0.0001);
    vec3 lp = light_dir - sin_l * direction;
    vec3 vp = view_dir - sin_v * direction;
    float cos_phi = dot(lp, vp) * inversesqrt(dot(lp, lp) * dot(vp, vp) + 0.0001);
    float half_phi = sqrt(clamp(0.5 + 0.5 * cos_phi, 0.0, 1.0));
    float width = clamp(mix(0.1, 0.75, roughness), 0.001, 1.0);
    width *= width;
    float theta = sin_l + sin_v;
    float primary = hair_gaussian(width, theta + 0.071) * 0.25 * half_phi
        * hair_fresnel(sqrt(clamp(0.5 + 0.5 * dot(view_dir, light_dir), 0.0, 1.0)));
    float f = hair_fresnel(cos_d * 0.5);
    vec3 absorption = pow(max(color, vec3(0.0)), vec3(0.8 / cos_d));
    vec3 secondary = hair_gaussian(width * 2.0, theta - 0.142)
        * (1.0 - f) * (1.0 - f) * f * absorption * exp(17.0 * cos_phi - 16.78);
    // Retail's TT lobe is disabled; retain R and TRT only.
    return vec3(primary) + secondary;
}

vec3 hair_diffuse(vec3 color, float nl, float scatter) {
    float wrap = clamp((nl + 1.0) * 0.25, 0.0, 1.0);
    return sqrt(max(color, vec3(0.0))) * mix(wrap, 1.0 - abs(nl), 0.33) * scatter / PI;
}

void fragment() {
    vec2 uv = hair_uv(UV, UVScaleOffset);
    if (EnableDiscardMaskMap && sample_trinity(DiscardMaskMap, hair_uv(UV2, UVScaleOffset1), 5).r < DiscardMaskValue) discard;
    vec4 base = BaseColor;
    if (EnableColorTableMap) {
        int index = clamp(int(BaseColorIndex1 - 1.0), 0, textureSize(ColorTableMap, 0).x - 1);
        base = texelFetch(ColorTableMap, ivec2(index, 0), 0);
    }
    // A base texture replaces constant alpha, while tinting RGB only.
    if (EnableBaseColorMap) base = sample_trinity(BaseColorMap, uv, 0) * vec4(base.rgb, 1.0);
    if (EnableAlphaTest && base.a < DiscardValue) discard;
    vec3 normal_ts = vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) {
        normal_ts.xy = (sample_trinity(NormalMap, uv, 1).rg * 2.0 - 1.0) * NormalHeight;
        normal_ts.z = sqrt(max(0.0, 1.0 - dot(normal_ts.xy, normal_ts.xy)));
        NORMAL_MAP = normal_ts * 0.5 + 0.5;
    }
    vec3 flow = vec3(0.0, 1.0, 0.0);
    if (EnableFlowMap) flow = sample_trinity(HairFlowMap, uv, 2).xyz * 2.0 - 1.0;
    float face = FRONT_FACING ? 1.0 : -1.0;
    vec3 shifted = face * (flow + normal_ts * HairHighlightShift);
    if (dot(shifted, shifted) < 0.000001) shifted = vec3(0.0, face, 0.0);
    // Retail builds the strand direction from the unflipped geometric normal;
    // Godot has already flipped NORMAL on back faces.
    hair_direction = normalize(mat3(TANGENT, BINORMAL, NORMAL * face) * shifted);
    hair_scatter = EnableScatterMap ? sample_trinity(MetallicMap, uv, 3).r : Metallic;
    ALBEDO = base.rgb;
    ROUGHNESS = EnableRoughnessMap ? sample_trinity(RoughnessMap, uv, 4).r : Roughness;
    METALLIC = 0.0; // Scatter must never turn hair into a metal in Godot.
    SPECULAR = 0.5;
    TRINITY_LIGHT_FRAGMENT;
}

void light() {
    float attenuation = TRINITY_ATTENUATION(1);
    float nl = dot(NORMAL, TRINITY_LIGHT);
    vec3 direction = normalize(hair_direction);
    // Godot multiplies DIFFUSE_LIGHT by ALBEDO after light(); this fiber
    // model already contains sqrt(albedo), so compensate for that multiply.
    DIFFUSE_LIGHT += hair_diffuse(ALBEDO, nl, hair_scatter) / max(ALBEDO, vec3(0.000001)) * max(nl, 0.0) * LIGHT_COLOR * attenuation;
    SPECULAR_LIGHT += hair_specular(ALBEDO, ROUGHNESS, direction, TRINITY_LIGHT, VIEW) * max(nl, 0.0) * LIGHT_COLOR * attenuation;
}
