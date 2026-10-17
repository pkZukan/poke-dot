shader_type spatial;

// Float uniforms
uniform float AOIntensity = 1.0;
uniform float AOIntensityFresnel = 1.0;
uniform float EmissionIntensity;
uniform float EmissionIntensityLayer1;
uniform float EmissionIntensityLayer2;
uniform float EmissionIntensityLayer3;
uniform float EmissionIntensityLayer4;
uniform float EmissionIntensityLayer5;
uniform float FresnelAlphaMax = 1.0;
uniform float FresnelAlphaMin;
uniform float FresnelAngleBias;
uniform float FresnelPower = 5.0;
uniform float LayerMaskScale1;
uniform float LayerMaskScale2;
uniform float LayerMaskScale3;
uniform float LayerMaskScale4;
uniform float MetallicLayer1;
uniform float MetallicLayer2;
uniform float MetallicLayer3;
uniform float MetallicLayer4;
uniform float MetallicLayer5;
uniform float NormalHeight = 1.0;
uniform float Reflectance = 0.04;
uniform float RoughnessLayer1;
uniform float RoughnessLayer2;
uniform float RoughnessLayer3;
uniform float RoughnessLayer4;
uniform float RoughnessLayer5;
uniform float SSSMaskOffset;

// Float4 uniforms
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 BaseColorLayer5 = vec4(1.0);
uniform vec4 BaseColorLayer6 = vec4(1.0);
uniform vec4 BaseColorLayer7 = vec4(1.0);
uniform vec4 BaseColorLayer8 = vec4(1.0);
uniform vec4 BaseColorLayer9 = vec4(1.0);
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 EmissionColorLayer1 = vec4(1.0);
uniform vec4 EmissionColorLayer2 = vec4(1.0);
uniform vec4 EmissionColorLayer3 = vec4(1.0);
uniform vec4 EmissionColorLayer4 = vec4(1.0);
uniform vec4 EmissionColorLayer5 = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetNormal = vec4(1.0, 1.0, 0.0, 0.0);

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableJewel;
uniform int ReceiveShadow;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D MetallicMap : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, repeat_enable;
uniform sampler2D AOMap : hint_default_white, repeat_enable;
uniform sampler2D EmissionColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D LayerMaskMap : hint_default_white, repeat_enable;
uniform sampler2D HighlightMaskMap : hint_default_black, repeat_enable;
uniform int sampler_repeat_u[8];
uniform int sampler_repeat_v[8];
uniform vec4 sampler_border_color[8];
#include "trinity_sampler.gdshaderinc"
uniform bool EnableBaseColorMap = true;
uniform bool EnableNormalMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableEmissionColorMap;
uniform bool EnableAOMap;
uniform bool EnableVertexColor;
uniform int NumMaterialLayer = 1;
uniform float Metallic;
uniform float Roughness = 0.2;

vec2 transform_uv(vec2 uv, vec4 st) {
    vec2 result = (uv - st.zw) * st.xy;
    return vec2(result.x, 1.0 - result.y);
}
float diffuse_weight(float intensity) {
    return EnableEmissionColorMap ? 1.0 : max(1.0 - intensity, 0.0);
}
void fragment() {
    vec2 uv = transform_uv(UV, UVScaleOffset);
    vec4 base = BaseColor;
    if (EnableBaseColorMap) {
        base = sample_trinity(BaseColorMap, uv, 0);
        base.a *= BaseColor.a;
    }
    vec3 base0 = base.rgb;
    vec3 normal_ts = vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) {
        vec2 xy = (sample_trinity(NormalMap, transform_uv(UV, UVScaleOffsetNormal), 1).rg * 2.0 - 1.0) * NormalHeight;
        normal_ts = vec3(xy, sqrt(1.0 - clamp(dot(xy, xy), 0.0, 1.0)));
        NORMAL_MAP = normal_ts * 0.5 + 0.5;
    }
    float metallic = EnableMetallicMap ? sample_trinity(MetallicMap, uv, 2).r : Metallic;
    float roughness = EnableRoughnessMap ? sample_trinity(RoughnessMap, uv, 3).r : Roughness;
    float ao = EnableAOMap ? sample_trinity(AOMap, vec2(UV.x, 1.0 - UV.y), 4).r : 1.0;
    vec3 emission = (EnableEmissionColorMap ? sample_trinity(EmissionColorMap, uv, 5).rgb : EmissionColor.rgb) * EmissionIntensity;
    vec4 mask = NumMaterialLayer > 1 ? sample_trinity(LayerMaskMap, uv, 6) : vec4(0.0);
    if (NumMaterialLayer < 3) mask.g = 0.0;
    if (NumMaterialLayer < 4) mask.b = 0.0;
    if (NumMaterialLayer < 5) mask.a = 0.0;
    float total = NumMaterialLayer > 1 ? clamp(1.0 - dot(mask, vec4(1.0)), 0.0, 1.0) : 1.0;
    base.rgb *= total * diffuse_weight(EmissionIntensity);
    float reflectance = total * diffuse_weight(EmissionIntensity);
    if (!EnableMetallicMap) metallic *= total;
    if (!EnableRoughnessMap) roughness *= total;
    if (!EnableEmissionColorMap) emission *= total;
    vec3 edge_base = BaseColorLayer5.rgb * total * diffuse_weight(EmissionIntensity);
    // Retail uses layer-1 emission intensity for the base Fresnel color too.
    vec3 edge_emission = BaseColorLayer5.rgb * EmissionIntensityLayer1;
    for (int i = 0; i < 4; i++) {
        vec3 color = i == 0 ? BaseColorLayer1.rgb : (i == 1 ? BaseColorLayer2.rgb : (i == 2 ? BaseColorLayer3.rgb : BaseColorLayer4.rgb));
        vec3 e = i == 0 ? EmissionColorLayer1.rgb : (i == 1 ? EmissionColorLayer2.rgb : (i == 2 ? EmissionColorLayer3.rgb : EmissionColorLayer4.rgb));
        float intensity = i == 0 ? EmissionIntensityLayer1 : (i == 1 ? EmissionIntensityLayer2 : (i == 2 ? EmissionIntensityLayer3 : EmissionIntensityLayer4));
        float m = i == 0 ? MetallicLayer1 : (i == 1 ? MetallicLayer2 : (i == 2 ? MetallicLayer3 : MetallicLayer4));
        float r = i == 0 ? RoughnessLayer1 : (i == 1 ? RoughnessLayer2 : (i == 2 ? RoughnessLayer3 : RoughnessLayer4));
        vec3 edge_color = i == 0 ? BaseColorLayer6.rgb : (i == 1 ? BaseColorLayer7.rgb : (i == 2 ? BaseColorLayer8.rgb : BaseColorLayer9.rgb));
        float weight = mask[i];
        base.rgb = mix(base.rgb, base0 * color * diffuse_weight(intensity), weight);
        reflectance = mix(reflectance, diffuse_weight(intensity), weight);
        if (!EnableMetallicMap) metallic = mix(metallic, m, weight);
        if (!EnableRoughnessMap) roughness = mix(roughness, r, weight);
        if (!EnableEmissionColorMap) emission = mix(emission, e * intensity, weight);
        edge_base = mix(edge_base, edge_color * diffuse_weight(intensity), weight);
        edge_emission = mix(edge_emission, edge_color * intensity, weight);
        total = mix(total, 1.0, weight);
    }
    float inv_weight = 1.0 / max(total, 0.00001);
    base.rgb *= inv_weight;
    edge_base *= inv_weight;
    edge_emission *= inv_weight;
    if (!EnableMetallicMap) metallic *= inv_weight;
    if (!EnableRoughnessMap) roughness *= inv_weight;
    if (!EnableEmissionColorMap) emission *= inv_weight;
    vec3 normal_view = normalize(TANGENT * normal_ts.x + BINORMAL * normal_ts.y + NORMAL * normal_ts.z);
    float nv = clamp(dot(normal_view, VIEW), 0.00001, 1.0);
    float bias = FresnelAngleBias * (EnableVertexColor ? COLOR.r : 1.0);
    float fresnel = FresnelAlphaMin + (FresnelAlphaMax - FresnelAlphaMin) * pow(1.0 - max(nv - bias, 0.0), FresnelPower);
    base.rgb = mix(base.rgb, edge_base, fresnel);
    emission = mix(emission, edge_emission, fresnel);
    metallic = mix(metallic, MetallicLayer5, fresnel);
    roughness = mix(roughness, RoughnessLayer5, fresnel);
    ao = 1.0 - (1.0 - ao) * mix(AOIntensity, AOIntensityFresnel, fresnel);
    float highlight = sample_trinity(HighlightMaskMap, uv, 7).r;
    base.rgb = mix(base.rgb, EmissionColorLayer5.rgb * diffuse_weight(EmissionIntensityLayer5), highlight);
    emission = mix(emission, EmissionColorLayer5.rgb * EmissionIntensityLayer5, highlight);
    reflectance = mix(reflectance, diffuse_weight(EmissionIntensityLayer5), highlight);
    ALBEDO = base.rgb;
    METALLIC = metallic;
    ROUGHNESS = roughness;
    SPECULAR = clamp(reflectance * Reflectance / 0.08, 0.0, 1.0);
    EMISSION = emission;
    AO = ao;
    SSS_STRENGTH = clamp(SSSMaskOffset * fresnel, 0.0, 1.0);
}
