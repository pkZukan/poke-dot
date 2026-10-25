shader_type spatial;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, repeat_enable;
uniform sampler2D AOMap : hint_default_white, repeat_enable;
uniform sampler2D SSSMaskMap : hint_default_white, repeat_enable;
uniform sampler2D LayerMaskMap : hint_default_black, repeat_enable;
uniform sampler2D OpacityMap1 : hint_default_black, repeat_enable;
uniform sampler2D LayerMaskMap1 : hint_default_black, repeat_enable;
uniform sampler2D LayerMaskMap2 : hint_default_black, repeat_enable;
uniform sampler2D LayerMaskMap3 : hint_default_black, repeat_enable;
uniform sampler2D ColorTableMap : source_color, hint_default_white, repeat_enable;

uniform int sampler_repeat_u[11];
uniform int sampler_repeat_v[11];
uniform vec4 sampler_border_color[11];
#include "trinity_sampler.gdshaderinc"
#include "trinity_sss.gdshaderinc"
#include "trinity_material.gdshaderinc"
#define TRINITY_CUSTOM_LIGHT
#include "trinity_light.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableNormalMap = false;
uniform bool EnableRoughnessMap = false;
uniform bool EnableAOMap = false;
uniform bool EnableSSSMaskMap = false;
uniform bool EnableColorTableMap = false;
uniform bool EnableVertexSubLayer = false;
uniform bool EnableCustomLayer = false;
uniform bool EnableUVScaleOffsetNormal = false;
uniform bool EnableLayerMaskUVTransform = false;
uniform bool EnableDeferredRendering = false;
uniform bool EnableEmissionFresnelLayer = false;
uniform bool EnableTransmission = false;

uniform int NumMaterialLayer = 1;
uniform int NumRequiredUV = 1;
uniform int ReceiveShadow = 1;
uniform int CastShadow = 1;
uniform int CategoryLabel = 0;
uniform int EnableGlobalWeather = 0;
uniform int EnableAdditionalLight = 1;
uniform int LightReceiveType = 2;
uniform int UVIndexLayerMask = -1;
uniform int UVIndexLayer5 = 0;

uniform float NormalHeight = 1.0;
uniform float Roughness = 0.2;
uniform float SSSMaskScale = 1.0;
uniform float SSSMaskOffset = 0.0;
uniform float EmissionIntensity = 0.0;

uniform vec4 BaseColor = vec4(1.0);
uniform vec4 EmissionColor = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetNormal = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetLayerMask = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset2 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset3 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset5 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVCenterRotationLayerMask = vec4(0.0);

uniform float Thickness = 0.01;
uniform float ThicknessRemapMin = 0.0;
uniform float ThicknessRemapMax = 16.0;
uniform float TransmissionMultiplier = 1.5;

uniform vec4 BaseColorLayer6 = vec4(1.0);
uniform vec4 BaseColorLayer7 = vec4(1.0);

uniform float BaseColorIndex6 = 1.0;
uniform float BaseColorIndex7 = 1.0;
uniform float LayerMaskScale1 = 1.0;
uniform float RoughnessLayer1 = 0.2;
uniform float EmissionIntensityLayer1 = 0.0;

uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 EmissionColorLayer1 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer1 = vec4(1.0);

uniform float BaseColorIndex1 = 1.0;
uniform float FresnelAngleBias1 = 0.0;
uniform float FresnelAlphaMin1 = 0.0;
uniform float FresnelAlphaMax1 = 0.0;
uniform float FresnelPower1 = 0.0;

uniform vec4 FresnelColor1 = vec4(1.0);

uniform float LayerMaskScale2 = 1.0;
uniform float RoughnessLayer2 = 0.2;
uniform float EmissionIntensityLayer2 = 0.0;

uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 EmissionColorLayer2 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer2 = vec4(1.0);

uniform float BaseColorIndex2 = 1.0;
uniform float FresnelAngleBias2 = 0.0;
uniform float FresnelAlphaMin2 = 0.0;
uniform float FresnelAlphaMax2 = 0.0;
uniform float FresnelPower2 = 0.0;

uniform vec4 FresnelColor2 = vec4(1.0);

uniform float LayerMaskScale3 = 1.0;
uniform float RoughnessLayer3 = 0.2;
uniform float EmissionIntensityLayer3 = 0.0;

uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 EmissionColorLayer3 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer3 = vec4(1.0);

uniform float BaseColorIndex3 = 1.0;
uniform float FresnelAngleBias3 = 0.0;
uniform float FresnelAlphaMin3 = 0.0;
uniform float FresnelAlphaMax3 = 0.0;
uniform float FresnelPower3 = 0.0;

uniform vec4 FresnelColor3 = vec4(1.0);

uniform float LayerMaskScale4 = 1.0;
uniform float RoughnessLayer4 = 0.2;
uniform float EmissionIntensityLayer4 = 0.0;

uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 EmissionColorLayer4 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer4 = vec4(1.0);

uniform float BaseColorIndex4 = 1.0;
uniform float FresnelAngleBias4 = 0.0;
uniform float FresnelAlphaMin4 = 0.0;
uniform float FresnelAlphaMax4 = 0.0;
uniform float FresnelPower4 = 0.0;

uniform vec4 FresnelColor4 = vec4(1.0);

uniform float LayerMaskScale5 = 1.0;
uniform float RoughnessLayer5 = 0.2;
uniform float EmissionIntensityLayer5 = 0.0;

uniform vec4 BaseColorLayer5 = vec4(1.0);
uniform vec4 EmissionColorLayer5 = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 SubsurfaceColorLayer5 = vec4(1.0);

uniform float BaseColorIndex5 = 1.0;
uniform float FresnelAngleBias5 = 0.0;
uniform float FresnelAlphaMin5 = 0.0;
uniform float FresnelAlphaMax5 = 0.0;
uniform float FresnelPower5 = 0.0;

uniform vec4 FresnelColor5 = vec4(1.0);

uniform float SubLayerMaskScale1 = 1.0;
uniform float BaseColorSubIndex1 = 1.0;

uniform vec4 BaseColorSubLayer1 = vec4(1.0);
uniform float SubLayerMaskScale2 = 1.0;
uniform float BaseColorSubIndex2 = 1.0;

uniform vec4 BaseColorSubLayer2 = vec4(1.0);
uniform float SubLayerMaskScale3 = 1.0;
uniform float BaseColorSubIndex3 = 1.0;

uniform vec4 BaseColorSubLayer3 = vec4(1.0);
uniform float SubLayerMaskScale4 = 1.0;
uniform float BaseColorSubIndex4 = 1.0;

uniform vec4 BaseColorSubLayer4 = vec4(1.0);
varying vec3 trinity_sss_color;
varying vec3 trinity_transmittance;
varying float trinity_sss_f0;
varying float trinity_sss_f90;
varying float trinity_sss_ao;
varying vec3 trinity_world_position;
void vertex() { trinity_world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz; }

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

vec3 table_color(vec3 fallback, float index) {
    if (!EnableColorTableMap) return fallback;
    return texelFetch(ColorTableMap, ivec2(int(index - 1.0), 0), 0).rgb;
}

vec3 layer_emission(vec3 color, vec3 fresnel_color, float angle_bias, float fmin, float fmax, float power, float nv) {
    if (!EnableEmissionFresnelLayer) return color;
    float f = fmin + (fmax - fmin) * pow(1.0 - clamp(nv - angle_bias, 0.0, 0.99999), power);
    return mix(color, fresnel_color, f);
}

void fragment() {
    vec2 primary_uv = transform_uv(UV, UVScaleOffset);
    vec4 base = EnableBaseColorMap ? sample_trinity(BaseColorMap, primary_uv, 0) : BaseColor;
    vec2 normal_uv = primary_uv;
    if (EnableUVScaleOffsetNormal) normal_uv = transform_uv(UV, UVScaleOffsetNormal);
    vec3 normal_ts = EnableNormalMap ? unpack_normal(sample_trinity(NormalMap, normal_uv, 1), NormalHeight) : vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) NORMAL_MAP = normal_ts * 0.5 + 0.5;
    vec3 normal_view = normalize(mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL)) * normal_ts);
    normal_view *= FRONT_FACING ? 1.0 : -1.0;
    float roughness = EnableRoughnessMap ? sample_trinity(RoughnessMap, primary_uv, 2).r : Roughness;
    float ao = EnableAOMap ? sample_trinity(AOMap, vec2(UV.x, 1.0 - UV.y), 3).r : 1.0;
    float sss_map = 1.0;
    if (EnableSSSMaskMap) sss_map = sample_trinity(SSSMaskMap, primary_uv, 4).r;
    else sss_map = length(fwidth(normal_view)) / max(length(fwidth(trinity_world_position * 1000.0)), 0.00000001);
    float sss_mask = clamp(sss_map * SSSMaskScale + SSSMaskOffset, 0.0, 1.0);
    float sss0 = sss_mask;
    vec3 subsurface = SubsurfaceColor.rgb;
    float reflectance = 1.0;
    bool pokemon = LightReceiveType == 3;
    vec4 emission = pokemon ? vec4(EmissionColor.rgb, 1.0) * EmissionIntensity : vec4(0.0);
    bool attenuate = pokemon;
    vec4 mask = vec4(0.0);
    vec3 base0 = base.rgb;
    float weight = 1.0;
    if (NumMaterialLayer > 1) {
        vec2 mask_uv = primary_uv;
        if (UVIndexLayerMask >= 0) {
            vec2 raw = select_uv(UV, UV2, UVIndexLayerMask);
            mask_uv = vec2(raw.x, 1.0 - raw.y);
        }
        if (EnableLayerMaskUVTransform) {
            mask_uv = material_layer_mask_uv(vec2(mask_uv.x, 1.0 - mask_uv.y), UVScaleOffsetLayerMask, UVCenterRotationLayerMask);
        }
        mask = sample_trinity(LayerMaskMap, mask_uv, 5) * vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4);
        if (NumMaterialLayer < 3) mask.g = 0.0;
        if (NumMaterialLayer < 4) mask.b = 0.0;
        if (NumMaterialLayer < 5) mask.a = 0.0;
        weight = clamp(1.0 - dot(mask, vec4(1.0)), 0.0, 1.0);
        base.rgb *= weight;
        if (!EnableRoughnessMap) roughness *= weight;
        if (pokemon) {
            sss_mask *= weight;
            emission *= weight;
            subsurface *= weight;
        }
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
        vec3 layer = base0 * table_color(BaseColorLayer1.rgb, BaseColorIndex1);
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer1.rgb, 1.0) * EmissionIntensityLayer1;
        layer_emission_value.rgb = layer_emission(EmissionColorLayer1.rgb, FresnelColor1.rgb, FresnelAngleBias1, FresnelAlphaMin1, FresnelAlphaMax1, FresnelPower1, nv) * EmissionIntensityLayer1;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer1, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            layer_sss *= inv_intensity;
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer1, factor);
        if (pokemon) {
            reflectance = mix(reflectance, layer_reflectance, factor);
            sss_mask = mix(sss_mask, layer_sss, factor);
            emission = mix(emission, layer_emission_value, factor);
            subsurface = mix(subsurface, SubsurfaceColorLayer1.rgb, factor);
        }
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 2) {
        float factor = mask.g;
        vec3 layer = base0 * table_color(BaseColorLayer2.rgb, BaseColorIndex2);
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer2.rgb, 1.0) * EmissionIntensityLayer2;
        layer_emission_value.rgb = layer_emission(EmissionColorLayer2.rgb, FresnelColor2.rgb, FresnelAngleBias2, FresnelAlphaMin2, FresnelAlphaMax2, FresnelPower2, nv) * EmissionIntensityLayer2;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer2, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            layer_sss *= inv_intensity;
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer2, factor);
        if (pokemon) {
            reflectance = mix(reflectance, layer_reflectance, factor);
            sss_mask = mix(sss_mask, layer_sss, factor);
            emission = mix(emission, layer_emission_value, factor);
            subsurface = mix(subsurface, SubsurfaceColorLayer2.rgb, factor);
        }
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 3) {
        float factor = mask.b;
        vec3 layer = table_color(BaseColorLayer3.rgb, BaseColorIndex3);
        if (!EnableColorTableMap) layer *= base0;
        if (EnableVertexSubLayer) {
            layer = mix(layer, table_color(BaseColorSubLayer1.rgb, BaseColorSubIndex1), SubLayerMaskScale1 * COLOR.r);
            layer = mix(layer, table_color(BaseColorSubLayer2.rgb, BaseColorSubIndex2), SubLayerMaskScale2 * COLOR.g);
            layer = mix(layer, table_color(BaseColorSubLayer3.rgb, BaseColorSubIndex3), SubLayerMaskScale3 * COLOR.b);
            layer = mix(layer, table_color(BaseColorSubLayer4.rgb, BaseColorSubIndex4), SubLayerMaskScale4 * COLOR.a);
        }
        // Retail's facial-expression layer multiplies by base0 again.
        layer *= base0;
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer3.rgb, 1.0) * EmissionIntensityLayer3;
        layer_emission_value.rgb = layer_emission(EmissionColorLayer3.rgb, FresnelColor3.rgb, FresnelAngleBias3, FresnelAlphaMin3, FresnelAlphaMax3, FresnelPower3, nv) * EmissionIntensityLayer3;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer3, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            layer_sss *= inv_intensity;
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer3, factor);
        if (pokemon) {
            reflectance = mix(reflectance, layer_reflectance, factor);
            sss_mask = mix(sss_mask, layer_sss, factor);
            emission = mix(emission, layer_emission_value, factor);
            subsurface = mix(subsurface, SubsurfaceColorLayer3.rgb, factor);
        }
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 4) {
        float factor = mask.a;
        vec3 layer = base0 * table_color(BaseColorLayer4.rgb, BaseColorIndex4);
        float layer_sss = sss0;
        float layer_reflectance = 1.0;
        vec4 layer_emission_value = vec4(EmissionColorLayer4.rgb, 1.0) * EmissionIntensityLayer4;
        layer_emission_value.rgb = layer_emission(EmissionColorLayer4.rgb, FresnelColor4.rgb, FresnelAngleBias4, FresnelAlphaMin4, FresnelAlphaMax4, FresnelPower4, nv) * EmissionIntensityLayer4;
        if (attenuate) {
            float inv_intensity = max(1.0 - EmissionIntensityLayer4, 0.0);
            layer *= inv_intensity;
            layer_reflectance *= inv_intensity;
            layer_sss *= inv_intensity;
        }
        base.rgb = mix(base.rgb, layer, factor);
        if (!EnableRoughnessMap) roughness = mix(roughness, RoughnessLayer4, factor);
        if (pokemon) {
            reflectance = mix(reflectance, layer_reflectance, factor);
            sss_mask = mix(sss_mask, layer_sss, factor);
            emission = mix(emission, layer_emission_value, factor);
            subsurface = mix(subsurface, SubsurfaceColorLayer4.rgb, factor);
        }
        weight = mix(weight, 1.0, factor);
    }
    if (NumMaterialLayer > 1) {
        float inv_weight = 1.0 / max(weight, 0.00001);
        base.rgb *= inv_weight;
        if (!EnableRoughnessMap) roughness *= inv_weight;
        if (pokemon) {
            reflectance *= inv_weight;
            emission *= inv_weight;
            sss_mask *= inv_weight;
            subsurface *= inv_weight;
        }
    }
    if (NumMaterialLayer > 5) {
        vec2 raw = select_uv(UV, UV2, UVIndexLayer5);
        float factor = sample_trinity(OpacityMap1, transform_uv(raw, UVScaleOffset5), 6).r * LayerMaskScale5;
        vec3 layer = BaseColorLayer5.rgb;
        float layer_sss = sss0;
        if (pokemon) {
            layer *= max(1.0 - EmissionIntensityLayer5, 0.0);
            layer_sss *= max(1.0 - EmissionIntensityLayer5, 0.0);
            vec3 color = layer_emission(EmissionColorLayer5.rgb, FresnelColor5.rgb, FresnelAngleBias5, FresnelAlphaMin5, FresnelAlphaMax5, FresnelPower5, nv);
            emission = mix(emission, vec4(color, 1.0) * EmissionIntensityLayer5, factor);
            sss_mask = mix(sss_mask, layer_sss, factor);
            subsurface = mix(subsurface, SubsurfaceColorLayer5.rgb, factor);
        }
        base.rgb = mix(base.rgb, layer, factor);
        roughness = mix(roughness, RoughnessLayer5, factor);
    }
    if (EnableCustomLayer) {
        base.rgb = mix(base.rgb, table_color(BaseColorLayer5.rgb, BaseColorIndex5), sample_trinity(LayerMaskMap1, transform_uv(UV, UVScaleOffset1), 7).r);
        base.rgb = mix(base.rgb, table_color(BaseColorLayer6.rgb, BaseColorIndex6), sample_trinity(LayerMaskMap2, transform_uv(UV, UVScaleOffset2), 8).r);
        base.rgb = mix(base.rgb, table_color(BaseColorLayer7.rgb, BaseColorIndex7), sample_trinity(LayerMaskMap3, transform_uv(UV, UVScaleOffset3), 9).r);
    }
    trinity_transmittance = vec3(0.0);
    if (EnableDeferredRendering) {
        trinity_sss_color = sss_mask * subsurface;
        EMISSION = emission.rgb;
        if (EnableTransmission) {
            float thickness = ThicknessRemapMin + Thickness * (1.0 - (EnableSSSMaskMap ? sss_map : 1.0)) * (ThicknessRemapMax - ThicknessRemapMin);
            trinity_transmittance = trinity_transmission(TransmissionMultiplier * subsurface, thickness);
        }
    } else {
        float emission_mask = min(1.0, emission.a);
        vec3 sss_emission = emission.rgb + sss_mask * SubsurfaceColor.rgb;
        EMISSION = sss_emission * emission_mask;
        trinity_sss_color = sss_emission * (1.0 - emission_mask);
    }
    trinity_sss_f0 = reflectance * 0.04;
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
    vec3 profile = trinity_diffusion_profile(nl, trinity_sss_color);
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
