shader_type spatial;
render_mode blend_premul_alpha, depth_draw_opaque, cull_back;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, repeat_enable;
uniform sampler2D AOMap : hint_default_white, repeat_enable;
uniform sampler2D SSSMaskMap : hint_default_white, repeat_enable;
uniform sampler2D LayerMaskMap : hint_default_black, repeat_enable;
uniform sampler2D EmissionColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D ParallaxMap : hint_default_white, repeat_enable;
uniform sampler2D LightMap : source_color, hint_default_white, repeat_enable;

uniform int sampler_repeat_u[9];
uniform int sampler_repeat_v[9];
uniform vec4 sampler_border_color[9];
#include "trinity_sampler.gdshaderinc"
#include "trinity_sss.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_ibl.gdshaderinc"
#include "trinity_blend.gdshaderinc"
#define TRINITY_CUSTOM_LIGHT
#include "trinity_light.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableNormalMap = false;
uniform bool EnableRoughnessMap = false;
uniform bool EnableAOMap = false;
uniform bool EnableSSSMaskMap = false;
uniform bool EnableParallaxMap = false;
uniform bool EnableEmissionColorMap = false;
uniform bool EnableAlphaTest = false;
uniform bool EnableLightMap = false;

uniform int NumMaterialLayer = 1;
uniform int NumRequiredUV = 1;
uniform int ReceiveShadow = 1;
uniform int CastShadow = 1;
uniform int CategoryLabel = 0;
uniform int EnableGlobalWeather = 0;
uniform int EnableAdditionalLight = 1;
uniform int RefractionMode = 0;

uniform float NormalHeight = 1.0;
uniform float Roughness = 0.2;
uniform float Reflectance = 0.04;
uniform float SSSMaskScale = 1.0;
uniform float SSSMaskOffset = 0.0;
uniform float EmissionIntensity = 0.0;
uniform float DiscardValue = 0.0;
uniform float ParallaxHeight = 0.01;
uniform float ObjectAlpha = 1.0;

uniform vec4 BaseColor = vec4(1.0);
uniform vec4 EmissionColor = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetNormal = vec4(1.0, 1.0, 0.0, 0.0);

uniform float FresnelAlphaMin = 0.0;
uniform float FresnelAlphaMax = 1.0;
uniform float FresnelAngleBias = 0.0;
uniform float LayerMaskScale1 = 1.0;
uniform float RoughnessLayer1 = 0.2;
uniform float EmissionIntensityLayer1 = 0.0;

uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 EmissionColorLayer1 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer1 = vec4(1.0);

uniform float LayerMaskScale2 = 1.0;
uniform float RoughnessLayer2 = 0.2;
uniform float EmissionIntensityLayer2 = 0.0;

uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 EmissionColorLayer2 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer2 = vec4(1.0);

uniform float LayerMaskScale3 = 1.0;
uniform float RoughnessLayer3 = 0.2;
uniform float EmissionIntensityLayer3 = 0.0;

uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 EmissionColorLayer3 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer3 = vec4(1.0);

uniform float LayerMaskScale4 = 1.0;
uniform float RoughnessLayer4 = 0.2;
uniform float EmissionIntensityLayer4 = 0.0;

uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 EmissionColorLayer4 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer4 = vec4(1.0);

varying vec3 trinity_sss_color;
varying vec3 trinity_transmittance;

varying float trinity_sss_f0;
varying float trinity_sss_f90;
varying float trinity_sss_ao;

vec2 transform_uv(vec2 uv, vec4 st) {
    vec2 v = (uv - st.zw) * st.xy;
    return vec2(v.x, 1.0 - v.y);
}

vec2 select_uv(vec2 uv0, vec2 uv1, int index) {
    return index != 0 && NumRequiredUV > 1 ? uv1 : uv0;
}

vec3 unpack_normal(vec4 encoded, float height) {
    vec2 xy = (encoded.rg * 2.0 - 1.0) * height;
    return vec3(xy, sqrt(1.0 - clamp(dot(xy, xy), 0.0, 1.0)));
}

void fragment() {
    vec2 primary_uv = transform_uv(UV, UVScaleOffset);
    if (EnableParallaxMap) {
        vec3 view_ts = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
        float z = abs(view_ts.z) < 0.00001 ? 0.00001 : view_ts.z;
        primary_uv += view_ts.xy / z * sample_trinity(ParallaxMap, primary_uv, 7).r * ParallaxHeight;
    }
    vec4 base = EnableBaseColorMap ? sample_trinity(BaseColorMap, primary_uv, 0) : BaseColor;
    if (EnableBaseColorMap) base.a *= BaseColor.a;
    if (EnableAlphaTest && base.a < DiscardValue) discard;
    vec2 normal_uv = primary_uv;
    normal_uv = transform_uv(UV, UVScaleOffsetNormal);
    vec3 normal_ts = EnableNormalMap ? unpack_normal(sample_trinity(NormalMap, normal_uv, 1), NormalHeight) : vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) NORMAL_MAP = normal_ts * 0.5 + 0.5;
    vec3 normal_view = normalize(mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL)) * normal_ts);
    normal_view *= FRONT_FACING ? 1.0 : -1.0;
    float roughness = EnableRoughnessMap ? sample_trinity(RoughnessMap, primary_uv, 2).r : Roughness;
    float ao = EnableAOMap ? sample_trinity(AOMap, vec2(UV.x, 1.0 - UV.y), 3).r : 1.0;
    float sss_map = 1.0;
    sss_map = sample_trinity(SSSMaskMap, primary_uv, 4).r;
    float sss_mask = clamp(sss_map * SSSMaskScale + SSSMaskOffset, 0.0, 1.0);
    float sss0 = sss_mask;
    vec3 subsurface = SubsurfaceColor.rgb;
    float reflectance = 1.0;
    vec4 emission = (EnableEmissionColorMap ? sample_trinity(EmissionColorMap, primary_uv, 6) : EmissionColor) * EmissionIntensity;
    bool attenuate = !EnableEmissionColorMap && !EnableLightMap;
    vec4 mask = vec4(0.0);
    vec3 base0 = base.rgb;
    float weight = 1.0;
    if (NumMaterialLayer > 1) {
        vec2 mask_uv = primary_uv;
        mask = sample_trinity(LayerMaskMap, mask_uv, 5) * vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4);
        if (NumMaterialLayer < 3) mask.g = 0.0;
        if (NumMaterialLayer < 4) mask.b = 0.0;
        if (NumMaterialLayer < 5) mask.a = 0.0;
        weight = clamp(1.0 - dot(mask, vec4(1.0)), 0.0, 1.0);
        base.rgb *= weight;
        if (!EnableRoughnessMap) roughness *= weight;
        reflectance *= weight;
        subsurface *= weight;
        if (!EnableEmissionColorMap) emission *= weight;
    }
    if (attenuate) {
        float inv_intensity = max(1.0 - EmissionIntensity, 0.0);
        base.rgb *= inv_intensity;
        sss_mask *= inv_intensity;
        reflectance *= inv_intensity;
    }
    float nv = clamp(dot(normal_view, VIEW), 0.00001, 1.0);
    if (NumMaterialLayer > 1) {
        float factor = mask.r;
        vec3 layer = base0 * BaseColorLayer1.rgb;
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer1.rgb, 1.0) * EmissionIntensityLayer1;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer1, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            sss_mask *= inv_intensity; // Retail layer 1 modifies the accumulated mask.
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer1, factor);
        reflectance = mix(reflectance, layer_reflectance, factor);
        sss_mask = mix(sss_mask, layer_sss, factor);
        emission = mix(emission, layer_emission_value, factor);
        subsurface = mix(subsurface, SubsurfaceColorLayer1.rgb, factor);
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 2) {
        float factor = mask.g;
        vec3 layer = base0 * BaseColorLayer2.rgb;
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer2.rgb, 1.0) * EmissionIntensityLayer2;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer2, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            layer_sss *= inv_intensity;
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer2, factor);
        reflectance = mix(reflectance, layer_reflectance, factor);
        sss_mask = mix(sss_mask, layer_sss, factor);
        emission = mix(emission, layer_emission_value, factor);
        subsurface = mix(subsurface, SubsurfaceColorLayer2.rgb, factor);
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 3) {
        float factor = mask.b;
        vec3 layer = base0 * BaseColorLayer3.rgb;
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer3.rgb, 1.0) * EmissionIntensityLayer3;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer3, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            layer_sss *= inv_intensity;
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer3, factor);
        reflectance = mix(reflectance, layer_reflectance, factor);
        sss_mask = mix(sss_mask, layer_sss, factor);
        emission = mix(emission, layer_emission_value, factor);
        subsurface = mix(subsurface, SubsurfaceColorLayer3.rgb, factor);
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 4) {
        float factor = mask.a;
        vec3 layer = base0 * BaseColorLayer4.rgb;
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer4.rgb, 1.0) * EmissionIntensityLayer4;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer4, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            layer_sss *= inv_intensity;
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer4, factor);
        reflectance = mix(reflectance, layer_reflectance, factor);
        sss_mask = mix(sss_mask, layer_sss, factor);
        emission = mix(emission, layer_emission_value, factor);
        subsurface = mix(subsurface, SubsurfaceColorLayer4.rgb, factor);
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 1) {
        float inv_weight = 1.0 / max(weight, 0.00001);
        base.rgb *= inv_weight;
        if (!EnableRoughnessMap) roughness *= inv_weight;
        reflectance *= inv_weight;
        emission *= inv_weight;
        sss_mask *= inv_weight;
        subsurface *= inv_weight;
    }
    trinity_sss_color = sss_mask * subsurface;
    trinity_transmittance = vec3(0.0);
    EMISSION = emission.rgb;
    trinity_sss_f0 = reflectance * Reflectance;
    float opacity = base.a * ObjectAlpha;
    if (RefractionMode == 1) {
        float f = FresnelAlphaMin + (FresnelAlphaMax - FresnelAlphaMin) * pow(1.0 - max(nv - FresnelAngleBias, 0.0), 5.0);
        opacity *= f;
    }
    vec2 blend_state = trinity_blend(opacity, 1.0);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
    if (EnableLightMap) {
        // Single-slice fallback: the game blends four scene-weighted array slices.
        vec3 lightmap = sample_trinity(LightMap, vec2(UV2.x, 1.0 - UV2.y), 8).rgb * TrinityLightMapGain;
        EMISSION = lightmap * base.rgb * ao;
        ao *= clamp(dot(lightmap, vec3(0.299, 0.587, 0.114)), 0.0, 1.0);
        trinity_sss_color = vec3(0.0);
    }
    trinity_sss_f90 = reflectance;
    
    ALBEDO = base.rgb;
    ROUGHNESS = roughness;
    METALLIC = 0.0;
    SPECULAR = material_specular(trinity_sss_f0);
    AO = ao;
    AO_LIGHT_AFFECT = 0.0; // Direct AO is applied in light(); transmission stays unoccluded.
    trinity_sss_ao = ao;
}

void light() {
    float nl = dot(NORMAL, TRINITY_LIGHT);
    float attenuation = TRINITY_ATTENUATION(ReceiveShadow);
    vec3 profile = EnableLightMap ? vec3(max(nl, 0.0)) : trinity_diffusion_profile(nl, trinity_sss_color);
    DIFFUSE_LIGHT += profile * LIGHT_COLOR * attenuation * trinity_sss_ao / PI;
    float wrap = 0.2588190451;
    float backlight = clamp((-nl + wrap) / ((1.0 + wrap) * (1.0 + wrap)), 0.0, 1.0);
    DIFFUSE_LIGHT += trinity_transmittance * backlight * LIGHT_COLOR * attenuation / PI;
    float clamped_nl = max(nl, 0.00001);
    float nv = clamp(dot(NORMAL, VIEW), 0.00001, 1.0);
    vec3 halfway = normalize(TRINITY_LIGHT + VIEW);
    float nh = clamp(dot(NORMAL, halfway), 0.0, 1.0);
    float lh = clamp(dot(TRINITY_LIGHT, halfway), 0.0, 1.0);
    SPECULAR_LIGHT += trinity_sss_ggx(clamped_nl, nv, nh, lh, ROUGHNESS, trinity_sss_f0, trinity_sss_f90) * LIGHT_COLOR * attenuation * trinity_sss_ao;
}
