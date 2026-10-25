shader_type spatial;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D NormalMap1 : hint_normal, repeat_enable;
uniform sampler2D EmissionNormalMap : hint_normal, repeat_enable;
uniform sampler2D EmissionColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D AOMap : hint_default_white, repeat_enable;
uniform sampler2D FresnelMaskMap : hint_default_white, repeat_enable;
uniform sampler2D FresnelNoiseMap : hint_default_white, repeat_enable;

uniform int sampler_repeat_u[9];
uniform int sampler_repeat_v[9];
uniform vec4 sampler_border_color[9];
#include "trinity_sampler.gdshaderinc"
#include "trinity_light.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableEmissionColorMap;
uniform bool EnableEmissionNormalMap;
uniform bool EnableFresnelTexture;
uniform bool EnableFresnelEmission;
uniform bool EnableAOMap;
uniform bool EnableEmissionMaskMap;
uniform bool EnableNormalMap;
uniform bool EnableFresnelEmissionColorBlend;

uniform float Metallic;
uniform float Roughness = 0.2;
uniform float NormalHeight = 1.0;
uniform float NormalHeight1 = 1.0;
uniform float EmissionNormalHeight = 1.0;
uniform float BaseColorMapSaturation = 1.0;
uniform float EmissionIntensity;
uniform float EmissionIntensityLayer1;
uniform float EmissionColorFresnel = 1.0;
uniform float EmissionMaskFresnel = 1.0;
uniform float FresnelAlphaMin;
uniform float FresnelAlphaMax = 1.0;
uniform float FresnelAngleBias;
uniform float LayerMaskScale1 = 1.0;

uniform vec4 BaseColor = vec4(1.0);
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 EmissionBlendEndColor = vec4(1.0);
uniform vec4 EmissionFresnelColor = vec4(1.0);
uniform vec4 ParallaxUVIntensity;
uniform vec4 FresnelUVSpeed;
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetEmissionNormal = vec4(1.0, 1.0, 0.0, 0.0);

uniform int UVIndexAO = -1;
uniform int UVIndexLayer1 = -1;
uniform int UVIndexLayer2 = -1;
uniform int NumRequiredUV = 1;
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableAdditionalLight;

vec2 transform_uv(vec2 uv, vec4 st) {
    vec2 v = (uv - st.zw) * st.xy;
    return vec2(v.x, 1.0 - v.y);
}
vec2 map_uv(vec2 uv0, vec2 uv1, vec2 primary, int index) {
    vec2 raw = index != 0 && NumRequiredUV > 1 ? uv1 : uv0;
    return index < 0 ? primary : vec2(raw.x, 1.0 - raw.y);
}
vec3 unpack_normal(vec4 encoded, float height) {
    vec2 xy = (encoded.rg * 2.0 - 1.0) * height;
    return vec3(xy, sqrt(1.0 - clamp(dot(xy, xy), 0.0, 1.0)));
}
vec3 blend_reoriented(vec3 base, vec3 detail) {
    vec3 t = base + vec3(0.0, 0.0, 1.0);
    vec3 u = detail * vec3(-1.0, -1.0, 1.0);
    return t / max(t.z, 0.00001) * dot(t, u) - u;
}
void fragment() {
    vec2 primary = transform_uv(UV, UVScaleOffset);
    vec3 view_ts = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
    float z = abs(view_ts.z) < 0.00001 ? 0.00001 : view_ts.z;
    vec2 parallax = view_ts.xy / z * 0.5 * vec2(1.0, -1.0);
    vec4 base = BaseColor;
    if (EnableBaseColorMap) {
        base *= sample_trinity(BaseColorMap, primary + parallax * ParallaxUVIntensity.xy, 0);
        base.rgb = mix(vec3(dot(base.rgb, vec3(0.299, 0.587, 0.114))), base.rgb, BaseColorMapSaturation);
    }
    vec3 normal_ts = vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) {
        normal_ts = unpack_normal(sample_trinity(NormalMap, map_uv(UV, UV2, primary, UVIndexLayer1), 2), NormalHeight);
        vec3 detail = unpack_normal(sample_trinity(NormalMap1, primary, 3), NormalHeight1);
        normal_ts = mix(normal_ts, blend_reoriented(detail, normal_ts), NormalHeight);
        NORMAL_MAP = normal_ts * 0.5 + 0.5;
    }
    mat3 tangent_to_view = mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
    vec3 surface_normal = normalize(tangent_to_view * normal_ts);
    vec3 emission_normal = surface_normal;
    if (EnableEmissionNormalMap) {
        vec3 n = unpack_normal(sample_trinity(EmissionNormalMap, transform_uv(UV, UVScaleOffsetEmissionNormal), 4), EmissionNormalHeight);
        emission_normal = normalize(tangent_to_view * n);
    }
    float ao = EnableAOMap ? sample_trinity(AOMap, map_uv(UV, UV2, primary, UVIndexAO), 6).r : 1.0;
    vec3 emission = (EnableEmissionColorMap ? sample_trinity(EmissionColorMap, primary, 5).rgb : EmissionColor.rgb) * EmissionIntensity;
    if (EnableEmissionMaskMap) emission *= sample_trinity(FresnelMaskMap, map_uv(UV, UV2, primary, UVIndexLayer2), 7).r;
    float nv_emission = clamp(dot(emission_normal, VIEW), 0.00001, 1.0);
    float edge = 0.04 + 0.98 * pow(1.0 - nv_emission, EmissionColorFresnel);
    if (EnableFresnelEmission) {
        vec3 blend = EmissionColor.rgb * edge;
        vec3 low = vec3(1.0) - 2.0 * (vec3(1.0) - emission) * (vec3(1.0) - blend);
        vec3 high = 2.0 * emission * blend;
        // Retail's branch condition is intentionally preserved.
        emission = mix(low, high, step(vec3(0.5), blend));
        float noise = sample_trinity(FresnelNoiseMap, primary + TIME * FresnelUVSpeed.xy, 8).r;
        emission *= EmissionFresnelColor.rgb * noise * pow(1.0 - nv_emission, EmissionMaskFresnel);
    }
    if (EnableFresnelEmissionColorBlend) emission = mix(EmissionColor.rgb, EmissionBlendEndColor.rgb, edge) * EmissionIntensity;
    emission = max(emission, vec3(0.0));
    if (EnableFresnelTexture) {
        float nv = clamp(dot(surface_normal, VIEW), 0.00001, 1.0);
        float f = FresnelAlphaMin + (FresnelAlphaMax - FresnelAlphaMin) * pow(1.0 - max(nv - FresnelAngleBias, 0.0), 5.0);
        vec2 layer_uv = transform_uv(primary + parallax * ParallaxUVIntensity.xy, UVScaleOffset1);
        layer_uv.y = 1.0 - layer_uv.y;
        vec3 color = sample_trinity(BaseColorMap1, layer_uv, 1).rgb * ao * BaseColorLayer1.rgb * EmissionIntensityLayer1;
        color = mix(emission, color, LayerMaskScale1);
        emission = mix(color, emission, f);
    }
    ALBEDO = base.rgb;
    METALLIC = Metallic;
    ROUGHNESS = Roughness;
    SPECULAR = 0.5;
    AO = ao;
    EMISSION = emission;
    TRINITY_LIGHT_FRAGMENT;
}
