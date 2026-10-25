shader_type spatial;
render_mode blend_premul_alpha, cull_disabled;
uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D NormalMap1 : hint_normal, repeat_enable;
uniform sampler2D FlowMap : hint_default_white, repeat_enable;
uniform sampler2D FoamMaskMap : hint_default_white, repeat_enable;
uniform int sampler_repeat_u[6];
uniform int sampler_repeat_v[6];
uniform vec4 sampler_border_color[6];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_refraction.gdshaderinc"
#include "trinity_blend.gdshaderinc"
#include "trinity_light.gdshaderinc"
uniform bool EnableBaseColorMap = true;
uniform bool EnableBaseColorMap1;
uniform bool EnableNormalMap;
uniform bool EnableNormalMap1;
uniform bool EnableFlowMap;
uniform bool EnableDepthFade;
uniform bool EnableVertexFoamMask;
uniform bool EnableVertexAlpha;
uniform bool EnableConservativeFoamFade;
uniform int NumMaterialLayer = 1;
uniform int UVIndexLayer1;
uniform int RefractionMode;
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 ScrollUVSpeed;
uniform vec4 ScrollUVSpeed1;
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 WaterFogColorNear = vec4(1.0);
uniform vec4 WaterFogColorFar = vec4(1.0);
uniform vec4 FoamColor = vec4(1.0);
uniform float NormalHeight = 1.0;
uniform float NormalHeight1 = 1.0;
uniform float LayerMaskScale1 = 1.0;
uniform float Metallic;
uniform float Roughness = 0.2;
uniform float EmissionIntensity;
uniform float FlowScaleX = 1.0;
uniform float FlowScaleY = 1.0;
uniform float FlowPower = 1.0;
uniform float WaterFogNearLength = 0.32;
uniform float WaterFogFarLength = 50.0;
uniform float WaterFogPower = 1.0;
uniform float DepthFadeDistance = 1.0;
uniform float SoftEdgeRatio = 0.1;
uniform float FoamEdgeRatio = 0.2;
uniform float FoamMaskIntensity = 1.0;
uniform float FoamMaskUVScale = 1.0;
uniform float FoamFlowPower = 1.0;
uniform float FoamWaveInfluence;
uniform float FoamNormalInfluence;
uniform float FoamSharpness = 1.0;
uniform float WaterOpaqueDistanceStart;
uniform float WaterOpaqueDistance = 1000.0;
uniform float WaterOpaquePower = 1.0;
uniform float IndexOfRefraction = 1.333;
uniform float Thickness = 0.2;

float water_opacity(float opacity, float depth) {
    float opaque = pow(clamp((depth - WaterOpaqueDistanceStart) / max(WaterOpaqueDistance - WaterOpaqueDistanceStart, 0.00001), 0.0, 1.0), max(WaterOpaquePower, 0.00001));
    return mix(opacity, 1.0, opaque);
}
vec4 water_sample(sampler2D tex, vec2 uv, vec4 offsets, float blend, int slot) {
    return mix(sample_trinity(tex, uv + offsets.xy, slot), sample_trinity(tex, uv + offsets.zw, slot), blend);
}

void fragment() {
    vec2 uv = material_uv(UV, UVScaleOffset) + ScrollUVSpeed.xy * TIME;
    vec4 offsets = vec4(0.0); float blend = 0.0;
    if (EnableFlowMap) {
        vec2 flow = (vec2(1.0) - sample_trinity(FlowMap, vec2(UV.x, 1.0 - UV.y), 4).rg * 2.0) * vec2(FlowScaleX, -FlowScaleY);
        vec2 phase = fract(TIME * FlowPower + vec2(0.0, 0.5));
        offsets = vec4(flow * phase.x, flow * phase.y);
        blend = abs(2.0 * phase.x - 1.0);
    }
    vec4 base = BaseColor;
    if (EnableBaseColorMap) base *= water_sample(BaseColorMap, uv, offsets, blend, 0);
    vec3 n = EnableNormalMap ? material_normal(water_sample(NormalMap, uv, offsets, blend, 2), NormalHeight) : vec3(0.0, 0.0, 1.0);
    if (NumMaterialLayer > 1) {
        vec2 uv1 = material_uv(UVIndexLayer1 == 1 ? UV2 : UV, UVScaleOffset1) + ScrollUVSpeed1.xy * TIME;
        if (EnableBaseColorMap1) base.rgb = mix(base.rgb, water_sample(BaseColorMap1, uv1, offsets, blend, 1).rgb * BaseColorLayer1.rgb, LayerMaskScale1);
        if (EnableNormalMap1) n = mix(n, material_normal(water_sample(NormalMap1, uv1, offsets, blend, 3), NormalHeight1), LayerMaskScale1);
    }
    float fog = pow(clamp((-VERTEX.z - WaterFogNearLength) / max(WaterFogFarLength - WaterFogNearLength, 0.00001), 0.0, 1.0), max(WaterFogPower, 0.00001));
    base.rgb *= mix(WaterFogColorNear.rgb, WaterFogColorFar.rgb, fog);
    float roughness = Roughness;
    float opacity = base.a;
    if (EnableDepthFade) {
        float depth_difference = trinity_depth_difference(SCREEN_UV, -VERTEX.z, INV_PROJECTION_MATRIX);
        float vertical_factor = abs((INV_VIEW_MATRIX * vec4(VIEW, 0.0)).y);
        float fade = clamp(depth_difference / max(DepthFadeDistance, 0.00001), 0.0, 1.0);
        float foam_fade = EnableConservativeFoamFade ? clamp(depth_difference * vertical_factor / max(DepthFadeDistance, 0.00001), 0.0, 1.0) : fade;
        if (EnableVertexFoamMask) foam_fade = min(foam_fade, 1.0 - COLOR.r);
        float mask = water_sample(FoamMaskMap, material_uv(UV, vec4(FoamMaskUVScale, FoamMaskUVScale, 0.0, 0.0)), offsets * FoamFlowPower, blend, 5).r;
        mask = clamp(mask * FoamMaskIntensity, 0.0, 1.0);
        // The wave term comes from the unported wave animation and is 1 without it.
        float influence = mix(1.0, normalize(n).z, FoamNormalInfluence);
        float factor = foam_fade * influence;
        mask = 1.0 - clamp((factor - SoftEdgeRatio) / max(FoamEdgeRatio, 0.00001), 0.0, 1.0) * mask;
        float foam = pow(clamp((1.0 - factor) / max(1.0 - SoftEdgeRatio, 0.0001), 0.0, 1.0), max(FoamSharpness, 0.00001));
        base.rgb += max(FoamColor.rgb * foam * mask, vec3(0.0));
        roughness += mask * (1.0 - factor);
        opacity = water_opacity(opacity * fade, depth_difference * vertical_factor);
    }
    NORMAL_MAP = normalize(n) * 0.5 + 0.5;
    METALLIC = Metallic; ROUGHNESS = clamp(roughness, 0.0, 1.0);
    ALBEDO = base.rgb; EMISSION = EmissionColor.rgb * EmissionIntensity;
    if (RefractionMode >= 2) {
        vec3 normal_view = normalize(mat3(TANGENT, BINORMAL, NORMAL) * n);
        vec2 screen = trinity_refract_uv(VERTEX, normal_view, VIEW, IndexOfRefraction, Thickness, PROJECTION_MATRIX);
        EMISSION = EMISSION * opacity + textureLod(trinity_scene_color, screen, 0.0).rgb * (1.0 - opacity);
        ALBEDO *= opacity; opacity = 1.0;
    }
    vec2 blend_state = trinity_blend(opacity * (EnableVertexAlpha ? COLOR.a : 1.0), 1.0);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
    TRINITY_LIGHT_FRAGMENT;
}
