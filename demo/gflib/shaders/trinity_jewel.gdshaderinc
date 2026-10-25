// Shared jewel material equations. POKEMON_JEWEL selects the retail fixed
// Pokemon constants and layered/eyelid material path.
uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D PackedMap : hint_default_white, repeat_enable;
uniform sampler2D EmissionNormalMap : hint_normal, repeat_enable;
uniform sampler2D EmissionColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D FresnelNoiseMap : hint_default_white, repeat_enable;
uniform sampler2D FresnelMaskMap : hint_default_white, repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white, repeat_enable;
uniform sampler2D LayerMaskMap : hint_default_black, repeat_enable;
uniform sampler2D OpacityMap1 : hint_default_black, repeat_enable;
uniform sampler2D UpperEyelidColorMap : source_color, hint_default_transparent, repeat_enable;
uniform sampler2D LowerEyelidColorMap : source_color, hint_default_transparent, repeat_enable;
uniform int sampler_repeat_u[12];
uniform int sampler_repeat_v[12];
uniform vec4 sampler_border_color[12];
#include "trinity_sampler.gdshaderinc"
#include "trinity_refraction.gdshaderinc"
#include "trinity_blend.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_light.gdshaderinc"
uniform int RefractionMode;
uniform bool EnableNormalMap;
uniform bool EnableAlphaTest;
uniform float DiscardValue = 0.5;
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetEmissionNormal = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 EmissionFresnelColor = vec4(1.0);
uniform vec4 FresnelUVSpeed;
#ifndef POKEMON_JEWEL
uniform bool EnableBaseColorMap = true;
uniform bool EnablePackedMap;
uniform bool EnableEmissionNormalMap;
uniform int RefractionSource;
uniform float NormalHeight = 1.0;
uniform float EmissionNormalHeight = 1.0;
uniform float Metallic;
uniform float Roughness = 0.2;
uniform float EmissionIntensity = 1.0;
uniform float EmissionColorFresnel = 6.0;
uniform float EmissionMaskFresnel = 0.6;
uniform float JewelBaseColorBrightness = 0.4;
uniform float IndexOfRefraction = 1.333;
uniform float Thickness = 0.4;
uniform float FresnelAlphaMin = 0.3;
uniform float FresnelAlphaMax = 0.8;
uniform float FresnelAngleBias = 0.6;
uniform float RefractionDepthBias;
#else
uniform int NumMaterialLayer = 1;
uniform int EyelidType;
uniform bool IsEye;
uniform bool EnableLayerMaskUVTransform;
uniform vec4 UVScaleOffsetLayerMask = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVCenterRotationLayerMask;
uniform float UVRotation;
uniform vec4 UVScaleOffset3 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset4 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset5 = vec4(1.0, 1.0, 0.0, 0.0);
uniform float UVRotation3;
uniform float UVRotation4;
uniform float LayerMaskScale5 = 1.0;
uniform int UVIndexLayer5;
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 BaseColorLayer5 = vec4(1.0);
uniform float LayerMaskScale1 = 1.0;
uniform float LayerMaskScale2 = 1.0;
uniform float LayerMaskScale3 = 1.0;
uniform float LayerMaskScale4 = 1.0;
#endif
vec2 jewel_uv(vec2 uv, vec4 transform) {
    vec2 result = (uv - transform.zw) * transform.xy;
    return vec2(result.x, 1.0 - result.y);
}
vec2 jewel_srt(vec2 uv, vec4 transform, float rotation) {
    float c = cos(rotation), s = sin(rotation);
    vec2 result = (mat2(vec2(c, -s), vec2(s, c)) * (uv - vec2(0.5)) - transform.zw) * transform.xy + vec2(0.5);
    return vec2(result.x, 1.0 - result.y);
}
vec3 jewel_normal(vec4 texel, float strength) {
    vec2 xy = (texel.rg * 2.0 - 1.0) * strength;
    return vec3(xy, sqrt(max(0.0, 1.0 - dot(xy, xy))));
}
vec3 jewel_overlay(vec3 texture_color, vec3 tint) {
    // Preserve the source's unusual reversed overlay branch.
    return mix(2.0 * texture_color * tint, vec3(1.0) - 2.0 * (vec3(1.0) - texture_color) * (vec3(1.0) - tint), lessThan(tint, vec3(0.5)));
}
void fragment() {
    vec2 primary = jewel_uv(UV, UVScaleOffset);
#ifdef POKEMON_JEWEL
    if (IsEye) primary = jewel_srt(UV, UVScaleOffset, UVRotation);
    vec4 base = sample_trinity(BaseColorMap, primary, 0);
    if (EnableAlphaTest && base.a < DiscardValue) discard;
    vec3 base0 = base.rgb;
    if (NumMaterialLayer > 1) {
        vec2 mask_uv = primary;
        if (EnableLayerMaskUVTransform) mask_uv = material_layer_mask_uv(vec2(primary.x, 1.0 - primary.y), UVScaleOffsetLayerMask, UVCenterRotationLayerMask);
        vec4 weights = sample_trinity(LayerMaskMap, mask_uv, 8) * vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4);
        if (NumMaterialLayer < 3) weights.g = 0.0;
        if (NumMaterialLayer < 4) weights.b = 0.0;
        if (NumMaterialLayer < 5) weights.a = 0.0;
        float total = clamp(1.0 - dot(weights, vec4(1.0)), 0.0, 1.0);
        base.rgb *= total;
        for (int i = 0; i < 4; i++) {
            vec3 color = i == 0 ? BaseColorLayer1.rgb : (i == 1 ? BaseColorLayer2.rgb : (i == 2 ? BaseColorLayer3.rgb : BaseColorLayer4.rgb));
            if (!(i == 2 && IsEye)) color *= base0;
            base.rgb = mix(base.rgb, color, weights[i]);
            total = mix(total, 1.0, weights[i]);
        }
        base.rgb /= max(total, 0.00001);
    }
    if (NumMaterialLayer > 5) {
        vec2 uv5 = jewel_uv(UVIndexLayer5 == 1 ? UV2 : UV, UVScaleOffset5);
        base.rgb = mix(base.rgb, BaseColorLayer5.rgb, sample_trinity(OpacityMap1, uv5, 9).r * LayerMaskScale5);
    }
    if (IsEye && (EyelidType == 1 || EyelidType == 3)) {
        vec4 lid = sample_trinity(UpperEyelidColorMap, jewel_srt(UV, UVScaleOffset3, UVRotation3), 10);
        base.rgb = mix(base.rgb, lid.rgb, lid.a);
    }
    if (IsEye && (EyelidType == 2 || EyelidType == 3)) {
        vec4 lid = sample_trinity(LowerEyelidColorMap, jewel_srt(UV, UVScaleOffset4, UVRotation4), 11);
        base.rgb = mix(base.rgb, lid.rgb, lid.a);
    }
    vec3 normal_ts = EnableNormalMap ? jewel_normal(sample_trinity(NormalMap, jewel_uv(UV, vec4(3.0, 3.0, 0.0, 0.0)), 1), 1.3) : vec3(0.0, 0.0, 1.0);
    vec3 emission_ts = jewel_normal(sample_trinity(EmissionNormalMap, jewel_uv(UV, UVScaleOffsetEmissionNormal), 3), 1.3);
    float metallic = 0.0;
    // Retail uses -5 for its custom IBL roughness; Godot's PBR range is [0,1].
    float roughness = 0.0;
    float ao = 1.0;
    float color_power = 6.0, mask_power = 0.6, intensity = 10.0, brightness = 0.4;
    float alpha_min = 0.3, alpha_max = 0.8, angle_bias = 0.6;
    float thickness = 0.4, ior = 1.333;
#else
    vec4 base = BaseColor;
    if (EnableBaseColorMap) base *= sample_trinity(BaseColorMap, primary, 0);
    vec3 normal_ts = jewel_normal(sample_trinity(NormalMap, primary, 1), NormalHeight);
    vec3 emission_ts = EnableEmissionNormalMap ? jewel_normal(sample_trinity(EmissionNormalMap, jewel_uv(UV, UVScaleOffsetEmissionNormal), 3), EmissionNormalHeight) : normal_ts;
    vec3 packed = sample_trinity(PackedMap, primary, 2).rgb;
    float metallic = EnablePackedMap ? packed.b : Metallic;
    float roughness = EnablePackedMap ? packed.g : Roughness;
    float ao = EnablePackedMap ? packed.r : 1.0;
    float color_power = EmissionColorFresnel, mask_power = EmissionMaskFresnel, intensity = EmissionIntensity, brightness = JewelBaseColorBrightness;
    float alpha_min = FresnelAlphaMin, alpha_max = FresnelAlphaMax, angle_bias = FresnelAngleBias;
    float thickness = Thickness, ior = IndexOfRefraction;
#endif
    mat3 tangent_basis = mat3(TANGENT, BINORMAL, NORMAL);
    // Godot already flips NORMAL on back faces, giving the retail T*x + B*y - N*z.
    vec3 normal_view = normalize(tangent_basis * normal_ts);
    vec3 emission_normal = normalize(tangent_basis * emission_ts);
    float nv = clamp(dot(emission_normal, VIEW), 0.00001, 1.0);
    float edge = max(1.0 - nv, 0.0);
    float fresnel = 0.04 + 0.98 * pow(edge, max(color_power, 0.00001));
    vec3 emission = jewel_overlay(sample_trinity(EmissionColorMap, primary, 4).rgb, EmissionColor.rgb * fresnel);
    float noise = sample_trinity(FresnelNoiseMap, SCREEN_UV + FresnelUVSpeed.xy * TIME, 5).r;
    float mask = 1.0;
#ifndef POKEMON_JEWEL
    vec3 view_ts = VIEW * tangent_basis;
    vec2 parallax = view_ts.xy / max(abs(view_ts.z), 0.00001) * 0.5;
    mask = sample_trinity(FresnelMaskMap, SCREEN_UV + parallax + FresnelUVSpeed.zw * TIME, 6).r;
#endif
    emission *= noise * mask * EmissionFresnelColor.rgb * pow(edge, max(mask_power, 0.00001)) * intensity;
    emission = max(emission, vec3(0.0)) + base.rgb * brightness;
    ALBEDO = base.rgb;
    NORMAL_MAP = normal_ts * 0.5 + 0.5;
    METALLIC = metallic;
    ROUGHNESS = clamp(roughness, 0.0, 1.0);
    AO = ao;
    float opacity = base.a;
    if (RefractionMode == 2) {
        vec2 refracted = trinity_refract_uv(VERTEX, normal_view, VIEW, ior, thickness, PROJECTION_MATRIX);
        vec3 background = textureLod(trinity_scene_color, refracted, 0.0).rgb;
#ifndef POKEMON_JEWEL
        if (RefractionSource == 1) background = sample_trinity(BaseColorMap1, SCREEN_UV + refracted, 7).rgb;
#endif
        float nv_surface = clamp(dot(normal_view, VIEW) - angle_bias, 0.0, 1.0);
        float coverage = alpha_min + (alpha_max - alpha_min) * pow(1.0 - nv_surface, 5.0);
#ifndef POKEMON_JEWEL
        coverage *= base.a;
#endif
        emission = mix(background, emission, coverage);
        ALBEDO *= coverage;
        opacity = 1.0;
    }
    EMISSION = emission;
    vec2 blend_state = trinity_blend(clamp(opacity, 0.0, 1.0), 1.0);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
    TRINITY_LIGHT_FRAGMENT;
}
