shader_type spatial;
render_mode blend_premul_alpha, cull_disabled;

uniform sampler2D BaseColorMap : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D BaseColorMap2 : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D NormalMap : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D MetallicMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D EmissionColorMap : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D FlowMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D ParallaxMap : hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;

uniform int sampler_repeat_u[9];
uniform int sampler_repeat_v[9];
uniform vec4 sampler_border_color[9];
#include "trinity_sampler.gdshaderinc"
#include "trinity_refraction.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_blend.gdshaderinc"
#include "trinity_light.gdshaderinc"

uniform float DepthFadeDistance = 1.0;
uniform float DiscardValue = 0.5;
uniform float DissolveEdgeIntensity = 0.0;
uniform float DissolveEdgeWidth = 0.0;
uniform float EmissionIntensity = 1.0;
uniform float FresnelAlphaMax = 1.0;
uniform float FresnelAlphaMin = 0.0;
uniform float Metallic = 0.0;
uniform float Reflectance = 0.04;
uniform float NormalHeight = 1.0;
uniform float RimIntensity = 0.0;
uniform float RimPower = 1.0;
uniform float Roughness = 0.5;
uniform float UVRotation = 0.0;
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 DissolveEdgeColor = vec4(1.0);
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 RimColor = vec4(1.0);
uniform vec4 TransmittanceColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform int AlphaProcess;
uniform int ColorProcess = 1;
uniform int Texture0AlphaChannel;
uniform int Texture0ColorChannel;
uniform int Texture1AlphaChannel;
uniform int Texture1AlphaOp;
uniform int Texture1ColorChannel;
uniform int Texture1ColorOp;
uniform int Texture2AlphaChannel;
uniform int Texture2AlphaOp;
uniform int Texture2ColorChannel;
uniform int Texture2ColorOp;
uniform int VertexAlphaChannel;
uniform int VertexAlphaOp;
uniform int VertexColorChannel;
uniform int VertexColorOp;
uniform bool EnableBaseColorMap = true;
uniform bool EnableBaseColorMap1;
uniform bool EnableBaseColorMap2;
uniform bool EnableNormalMap;
uniform bool EnableParallaxMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableEmissionColorMap;
uniform bool EnableAlphaTest;
uniform bool EnableVertexColor;
uniform bool EnableLighting;
uniform bool EnableDepthFade;
uniform bool EnableFlowMap;
uniform bool EnableSphereMap;
uniform int RefractionMode;
uniform int UVIndexLayer1;
uniform int UVIndexLayer2;
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset2 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset4 = vec4(1.0, 1.0, 0.0, 0.0);
uniform float UVRotation1;
uniform float UVRotation2;
uniform float FlowScaleX;
uniform float FlowScaleY;
uniform float ParallaxHeight;
uniform float IsTexture1SphereMap;
uniform float IsTexture2SphereMap;
uniform float IndexOfRefraction = 1.333;
uniform float Thickness = 0.1;

varying vec2 sphere_uv;

void vertex() {
    // View-space sphere mapping of the reflected view ray.
    vec3 r = reflect(normalize((MODELVIEW_MATRIX * vec4(VERTEX, 1.0)).xyz), normalize(MODELVIEW_NORMAL_MATRIX * NORMAL));
    sphere_uv = r.xy / max(length(r + vec3(0.0, 0.0, 1.0)), 0.00001) * 0.5 + 0.5;
}

vec2 effect_uv(vec2 uv, vec4 transform, float rotation) {
    float c = cos(rotation), s = sin(rotation);
    vec2 centered = uv - vec2(0.5);
    vec2 result = (mat2(vec2(c, -s), vec2(s, c)) * centered - transform.zw) * transform.xy + vec2(0.5);
    return vec2(result.x, 1.0 - result.y);
}

vec4 effect_channels(vec4 value, int color_channel, int alpha_channel) {
    vec3 rgb = color_channel == 1 ? vec3(1.0) - value.rgb : (color_channel == 2 ? vec3(1.0) : value.rgb);
    float a = value.a;
    if (alpha_channel == 1) a = 1.0 - value.a;
    if (alpha_channel == 2) a = value.r;
    if (alpha_channel == 3) a = 1.0 - value.r;
    if (alpha_channel == 4) a = 1.0;
    return vec4(rgb, a);
}

vec4 effect_blend(vec4 previous, vec4 current, int color_op, int alpha_op) {
    vec3 rgb = previous.rgb;
    if (color_op == 0) rgb *= current.rgb;
    if (color_op == 1) rgb += current.rgb;
    if (color_op == 2) rgb -= current.rgb;
    float alpha = previous.a;
    if (alpha_op == 0) alpha *= current.a;
    if (alpha_op == 1) alpha += current.a;
    if (alpha_op == 2) alpha -= current.a;
    return vec4(rgb, alpha);
}

vec4 effect_constants(vec4 value) {
    vec3 rgb = BaseColor.rgb;
    if (ColorProcess == 1) rgb *= value.rgb;
    if (ColorProcess == 2) rgb = mix(BaseColorLayer1.rgb, BaseColor.rgb, value.rgb);
    if (ColorProcess == 3) rgb = BaseColor.rgb * value.rgb + BaseColorLayer1.rgb;
    return vec4(rgb, value.a);
}

float effect_alpha(float alpha) {
    return AlphaProcess == 0 ? BaseColor.a * alpha : clamp((alpha - BaseColor.a) * 4.0, 0.0, 1.0) * BaseColorLayer1.a;
}

void fragment() {
    vec2 primary = effect_uv(UV, UVScaleOffset, UVRotation);
    vec2 color_uv = primary;
    vec3 view_ts = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
    if (EnableParallaxMap) color_uv += vec2(view_ts.x, -view_ts.y) / max(abs(view_ts.z), 0.00001) * sample_trinity(ParallaxMap, primary, 8).r * ParallaxHeight;
    if (EnableFlowMap) {
        vec2 flow_uv = (UV - UVScaleOffset4.zw) * UVScaleOffset4.xy;
        vec2 flow = sample_trinity(FlowMap, vec2(flow_uv.x, 1.0 - flow_uv.y), 7).rg * 2.0 - 1.0;
        color_uv += flow * vec2(FlowScaleX, -FlowScaleY);
    }
    vec4 combined = vec4(1.0);
    if (EnableBaseColorMap) combined = effect_channels(sample_trinity(BaseColorMap, color_uv, 0), Texture0ColorChannel, Texture0AlphaChannel);
    if (EnableBaseColorMap1) {
        vec2 uv1 = effect_uv(UVIndexLayer1 == 1 ? UV2 : UV, UVScaleOffset1, UVRotation1);
        if (EnableSphereMap) uv1 = mix(uv1, sphere_uv, IsTexture1SphereMap);
        combined = effect_blend(combined, effect_channels(sample_trinity(BaseColorMap1, uv1, 1), Texture1ColorChannel, Texture1AlphaChannel), Texture1ColorOp, Texture1AlphaOp);
    }
    if (EnableBaseColorMap2) {
        vec2 uv2 = effect_uv(UVIndexLayer2 == 1 ? UV2 : UV, UVScaleOffset2, UVRotation2);
        if (EnableSphereMap) uv2 = mix(uv2, sphere_uv, IsTexture2SphereMap);
        combined = effect_blend(combined, effect_channels(sample_trinity(BaseColorMap2, uv2, 2), Texture2ColorChannel, Texture2AlphaChannel), Texture2ColorOp, Texture2AlphaOp);
    }
    combined = effect_constants(combined);
    if (EnableVertexColor) combined = effect_blend(combined, effect_channels(COLOR, VertexColorChannel, VertexAlphaChannel), VertexColorOp, VertexAlphaOp);
    float opacity = effect_alpha(combined.a);
    if (EnableAlphaTest && opacity < DiscardValue) discard;
    vec3 n = vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) {
        n.xy = (sample_trinity(NormalMap, primary, 3).rg * 2.0 - 1.0) * NormalHeight;
        n.z = sqrt(max(0.0, 1.0 - dot(n.xy, n.xy)));
        NORMAL_MAP = n * 0.5 + 0.5;
    }
    vec3 normal_view = normalize(mat3(TANGENT, BINORMAL, NORMAL) * n);
    float nv = clamp(dot(normal_view, VIEW), 0.00001, 1.0);
    float edge = 0.0;
    if (DissolveEdgeWidth > 0.0) edge = 1.0 - smoothstep(DiscardValue, DiscardValue + DissolveEdgeWidth, opacity);
    else edge = opacity < DiscardValue ? 1.0 : 0.0; // Retail divides by zero width.
    vec3 emission = DissolveEdgeColor.rgb * DissolveEdgeIntensity * edge + RimColor.rgb * RimIntensity * pow(1.0 - nv, max(RimPower, 0.00001));
    float fresnel_alpha = FresnelAlphaMax > FresnelAlphaMin ? smoothstep(FresnelAlphaMin, FresnelAlphaMax, nv) : step(FresnelAlphaMin, nv);
    opacity *= fresnel_alpha;
    if (EnableDepthFade) opacity *= trinity_depth_fade(SCREEN_UV, -VERTEX.z, DepthFadeDistance, INV_PROJECTION_MATRIX);
    ALBEDO = EnableLighting ? combined.rgb : vec3(0.0);
    METALLIC = EnableLighting ? (EnableMetallicMap ? sample_trinity(MetallicMap, primary, 4).r : Metallic) : 0.0;
    SPECULAR = EnableLighting ? material_specular(Reflectance) : 0.0;
    ROUGHNESS = EnableLighting ? (EnableRoughnessMap ? sample_trinity(RoughnessMap, primary, 5).r : Roughness) : 1.0;
    if (EnableLighting) emission += (EnableEmissionColorMap ? sample_trinity(EmissionColorMap, primary, 6).rgb : EmissionColor.rgb) * EmissionIntensity;
    else emission += combined.rgb * EmissionIntensity;
    if (RefractionMode >= 2) {
        vec2 refracted = trinity_refract_uv(VERTEX, normal_view, VIEW, IndexOfRefraction, Thickness, PROJECTION_MATRIX);
        emission = mix(textureLod(trinity_scene_color, refracted, 0.0).rgb * TransmittanceColor.rgb, emission, opacity);
        ALBEDO *= opacity;
        opacity = 1.0;
    }
    EMISSION = emission;
    vec2 blend_state = trinity_blend(clamp(opacity, 0.0, 1.0), 1.0);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
    TRINITY_LIGHT_FRAGMENT;
}
