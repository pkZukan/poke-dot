
#ifdef CHARACTER_TRANSPARENT
#include "trinity_blend.gdshaderinc"
uniform int RefractionMode;
uniform bool IgnoreAlbedoAlpha;
uniform float FresnelAlphaMin = 1.0;
uniform float FresnelAlphaMax = 1.0;
uniform float FresnelAngleBias;
#endif
uniform bool EnableHolocastEffect;
uniform vec4 HolocastColor = vec4(1.0);
uniform float ScanGradationTiling = 1.0;
uniform float ScanlineTiling = 1.0;
uniform float ScanlineDepth;
uniform float ScanlineSpeed;
// Fixed holocast light direction in degrees, applied in model space.
uniform float LightPitch;
uniform float LightYaw;
uniform float LightRoll;
varying vec3 character_world_position;
void vertex() { character_world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz; }
uniform sampler2D BaseColorMap  : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D NormalMap     : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D OcclusionMap   : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D SpecularMaskMap  : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D ShadowingColorMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D ShadowingColorMaskMap         : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D RimLightMaskMap        : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D LayerMaskMap  : hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D ParallaxMap   : hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D HighlightMaskMap : source_color, hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D EyelidShadowMaskMap      : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D OpacityMap1 : hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D ColorTableMap : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D UpperEyelidColorMap : source_color, hint_default_transparent, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D LowerEyelidColorMap : source_color, hint_default_transparent, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D DiscardMaskMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D IkLayerMaskMap1 : hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D IkLayerMaskMap2 : hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D IkLayerMaskMap3 : hint_default_black, filter_linear_mipmap_anisotropic, repeat_enable;
// A probe cubemap in retail; it is not one of the sample_trinity 2D slots.
uniform samplerCube LocalReflectionMap : hint_default_white;

// Shader slots are resolved by texture name in the material loader.
uniform int sampler_repeat_u[19];
uniform int sampler_repeat_v[19];
uniform vec4 sampler_border_color[19];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_ibl.gdshaderinc"
#define TRINITY_CUSTOM_LIGHT
#include "trinity_light.gdshaderinc"

//Shader strings (as bool)
uniform bool EnableBaseColorMap;
uniform bool EnableNormalMap;
uniform bool EnableAOMap;
uniform bool EnableAlphaTest;
uniform bool BaseColorMultiply;
uniform bool EnableHairSpecular;
uniform bool EnableEmissionLayer;
uniform bool EnableLerpBaseColorEmission;
uniform bool EnableEyeOptions;
uniform bool EnableHighlight;
uniform bool EnableParallaxMap;
uniform bool RequireEyelidShadowMap;
uniform bool EnableUVScaleOffsetNormal;

//Shader strings (as int)
uniform int NumMaterialLayer = 5;

uniform bool EnableSSSMaskMap;
uniform bool EnableColorTableMap;
uniform bool EnableLayerMaskUVTransform;
uniform vec4 UVScaleOffsetLayerMask = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVCenterRotationLayerMask;
uniform bool EnableVertexSubLayer;
uniform bool EnableCustomLayer;
uniform vec4 BaseColorSubLayer1 = vec4(1.0);
uniform vec4 BaseColorSubLayer2 = vec4(1.0);
uniform vec4 BaseColorSubLayer3 = vec4(1.0);
uniform vec4 BaseColorSubLayer4 = vec4(1.0);
uniform float SubLayerMaskScale1 = 1.0;
uniform float SubLayerMaskScale2 = 1.0;
uniform float SubLayerMaskScale3 = 1.0;
uniform float SubLayerMaskScale4 = 1.0;
uniform vec4 BaseColorLayer7 = vec4(1.0);
uniform int BaseColorIndex1 = 1;
uniform int BaseColorIndex2 = 1;
uniform int BaseColorIndex3 = 1;
uniform int BaseColorIndex4 = 1;
uniform int BaseColorIndex6 = 1;
uniform int ColorTableDivideNumber = 1;
uniform float DiscardValue_R;
uniform float DiscardValue_G;
uniform float DiscardValue_B;
uniform float DiscardValue_A;
uniform float SpecularShiftValue;
uniform float SecondSpecularWidth;
uniform float SecondSpecularStrength;
uniform vec4 BaseColorLayer8 = vec4(1.0);
uniform vec4 BaseColorLayer9 = vec4(1.0);
uniform vec4 BaseColorLayer10 = vec4(1.0);
uniform float MetallicLayer7;
uniform float MetallicLayer8;
uniform vec4 UVScaleOffset3 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset4 = vec4(1.0, 1.0, 0.0, 0.0);
uniform float UVRotation3;
uniform float UVRotation4;

//Float params
uniform float SaturationPower = 1.0;
uniform float BaseColorDarkness;
uniform float NormalHeight = 1.0;
uniform float Metallic;
uniform float OcclusionStrength = 1.0;
uniform float SpecularOffset;
uniform float SpecularIntensity;
uniform float SpecularContrast;
uniform float ShadowStrength;
uniform float EmissionIntensity;
uniform float DiscardValue;
uniform float ShadingBias;
uniform float ShadowingColorMaskMapValue;
uniform float HueShiftBias;
uniform float ShadowingBias;
uniform float HalfLambertBias;
uniform float ShadowingGIGain;
uniform float RimLightOffset;
uniform float RimLightContrast;
uniform float RimLightIntensity;
uniform float BackRimLightIntensity;
uniform float ReflectionsBlur;
uniform float DiffusionLevels;
uniform float LayerMaskScale1 = 1.0;
uniform float SpecularLayer1Intensity;
uniform float SpecularLayer1Offset;
uniform float SpecularLayer1Contrast;
uniform float MetallicLayer1;
uniform float LayerMaskScale2 = 1.0;
uniform float SpecularLayer2Intensity;
uniform float SpecularLayer2Offset;
uniform float SpecularLayer2Contrast;
uniform float MetallicLayer2;
uniform float LayerMaskScale3 = 1.0;
uniform float LayerMaskScale4 = 1.0;
uniform float SpecularLayer4Intensity;
uniform float SpecularLayer4Offset;
uniform float SpecularLayer4Contrast;
uniform float MetallicLayer4;
uniform float MidAreaShift;
uniform float MidAreaContrast;
uniform float MidAreaHueOffset;
uniform float DarkAreaShift;
uniform float DarkAreaContrast;
uniform float DarkAreaHueOffset;
uniform float HueShiftAreaValue;
uniform float ShadowingShift;
uniform float ShadowingContrast;
uniform float ParallaxHeight;
uniform float ParallaxIOR = 1.0;
uniform float EmissionIntensityLayer5;
uniform float UVRotation2;

//Float4 params
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 UVCenter0;
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetNormal = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 ShadowingColor;
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 ShadowingColorLayer1;
uniform vec4 ShadowingColorLayer2;
uniform vec4 ShadowingColorLayer3;
uniform vec4 ShadowingColorLayer4;

//int params
uniform int CastShadow;
uniform int ReceiveShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableDistanceDither;
uniform int UVIndexAO = -1;
uniform int EnableAdditionalLight;
uniform int UVIndexLayerMask = -1;

uniform float SpecularLayer3Intensity = 0.0;
uniform float SpecularLayer3Offset = 0.0;
uniform float SpecularLayer3Contrast = 0.0;
uniform float MetallicLayer3 = 0.0;
uniform vec4 EmissionColorLayer5 = vec4(1.0);
uniform vec4 UVCenter1 = vec4(1.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 BaseColorLayer6 = vec4(1.0);
uniform vec4 UVScaleOffset2 = vec4(1.0, 1.0, 0.0, 0.0);
uniform int EyelidType = 0;
uniform int EnableIrisRefraction = 0;
uniform int NumRequiredUV = 1;
uniform float EmissionIntensityLayer1 = 0.0;
uniform float EmissionIntensityLayer2 = 0.0;
uniform float EmissionIntensityLayer3 = 0.0;
uniform float EmissionIntensityLayer4 = 0.0;
uniform float AuraColorScale = 0.0;
uniform float AuraColorScrollSpeed = 0.0;
uniform float AuraNoiseScale = 0.0;
uniform float AuraNoiseScrollSpeed = 0.0;
uniform float AuraIntensity = 0.0;
uniform float AuraRimPower = 0.0;
uniform vec4 EmissionColorLayer1 = vec4(1.0);
uniform vec4 EmissionColorLayer2 = vec4(1.0);
uniform vec4 EmissionColorLayer3 = vec4(1.0);
uniform vec4 EmissionColorLayer4 = vec4(1.0);
uniform bool EnableAuraEffect = false;
uniform float LayerMaskScale5 = 1.0;
uniform float MetallicLayer5;
uniform float SpecularLayer5Intensity;
uniform float SpecularLayer5Offset;
uniform float SpecularLayer5Contrast;
uniform vec4 BaseColorLayer5 = vec4(1.0);
uniform vec4 ShadowingColorLayer5;
uniform vec4 UVScaleOffset5 = vec4(1.0, 1.0, 0.0, 0.0);
uniform int UVIndexLayer5;

// Rendering metadata (category, weather, dirt/jewel and distance dither)
// is retained for the importer; no corresponding scene systems exist here.
vec2 transform_uv(vec2 uv, vec4 scale_offset, vec2 center) {
    vec2 result = (uv - center - scale_offset.zw) * scale_offset.xy + center;
    return vec2(result.x, 1.0 - result.y);
}
vec4 layer_weights(vec4 mask) {
    vec4 weights = mask * vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4);
    if (NumMaterialLayer < 2) weights.r = 0.0;
    if (NumMaterialLayer < 3) weights.g = 0.0;
    if (NumMaterialLayer < 4) weights.b = 0.0;
    if (NumMaterialLayer < 5) weights.a = 0.0;
    return weights;
}
vec4 blend_colors(vec4 base, vec4 a, vec4 b, vec4 c, vec4 d, vec4 weights) {
    return mix(mix(mix(mix(base, a, weights.r), b, weights.g), c, weights.b), d, weights.a);
}

varying vec3 shadow_tint;
varying float surface_specular;
varying float surface_gloss;
varying float rim_mask;
varying float surface_offset;
varying float surface_contrast;
varying float surface_mask;
varying float surface_ao;
varying float surface_emission;
varying float surface_metallic;
varying vec3 reflection_color;
varying float reflection_fresnel;
varying vec3 hair_binormal;
varying float hair_strength;

float adjust_contrast(float value, float contrast) {
    return clamp((value - 0.5) * (1.0 + 2.0 * contrast) + 0.5, 0.0, 1.0);
}

vec2 get_uv(vec2 uv, int slot) {
    vec4 transform = slot == 1 && EnableUVScaleOffsetNormal ? UVScaleOffsetNormal : UVScaleOffset;
    vec2 center = slot == 1 && EnableUVScaleOffsetNormal ? vec2(0.0) : ((EnableEyeOptions || EnableParallaxMap) ? UVCenter0.xy : vec2(0.0));
    if (EnableEyeOptions && slot == 10) { transform = UVScaleOffset1; center = UVCenter1.xy; }
    return transform_uv(uv, transform, center);
}
// Eyelid maps rotate about the UV center before the scale/offset.
vec2 eye_srt(vec2 uv, vec4 transform, float rotation) {
    float c = cos(rotation), s = sin(rotation);
    vec2 v = uv - vec2(0.5);
    vec2 result = (vec2(c * v.x + s * v.y, c * v.y - s * v.x) - transform.zw) * transform.xy + vec2(0.5);
    return vec2(result.x, 1.0 - result.y);
}
vec3 color_table(int index, float row) {
    return sample_trinity(ColorTableMap, vec2((float(index) - 0.5) / float(max(ColorTableDivideNumber, 1)), row), 12).rgb;
}
float blend_values(float base, vec4 layers, vec4 weights) {
    return mix(mix(mix(mix(base, layers.r, weights.r), layers.g, weights.g), layers.b, weights.b), layers.a, weights.a);
}
// HSV hue shift from the character shader; RGB-axis rotation changes saturation.
vec3 hue_rotate(vec3 color, float degrees) {
    vec4 k = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(color.bg, k.wz), vec4(color.gb, k.xy), step(color.b, color.g));
    vec4 q = mix(vec4(p.xyw, color.r), vec4(color.r, p.yzx), step(p.x, color.r));
    float d = q.x - min(q.w, q.y);
    float e = 1e-10;
    vec3 hsv = vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), d == 0.0 ? q.x : q.x + e);
    hsv.x = fract(hsv.x + degrees / 360.0);
    vec3 rgb = abs(fract(hsv.xxx + vec3(1.0, 2.0 / 3.0, 1.0 / 3.0)) * 6.0 - vec3(3.0));
    return hsv.z * mix(vec3(1.0), clamp(rgb - vec3(1.0), vec3(0.0), vec3(1.0)), hsv.y);
}
void fragment() {
    vec2 primary_uv = get_uv(UV, 0);
    vec2 normal_uv = EnableUVScaleOffsetNormal ? transform_uv(UV, UVScaleOffsetNormal, vec2(0.0)) : primary_uv;
    vec3 normal_ts = vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) {
        vec2 xy = (sample_trinity(NormalMap, normal_uv, 1).rg * 2.0 - 1.0) * NormalHeight;
        normal_ts = vec3(xy, sqrt(1.0 - clamp(dot(xy, xy), 0.0, 1.0)));
    }
    if (EnableParallaxMap) {
        vec3 view_ts = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
        vec3 ray = -view_ts;
        if (EnableIrisRefraction != 0) ray = refract(ray, EnableIrisRefraction == 2 ? normal_ts : vec3(0.0, 0.0, 1.0), 1.0 / max(ParallaxIOR, 0.00001));
        ray = normalize(ray);
        float bias = clamp(abs(dot(VIEW, NORMAL)), 0.0, 1.0);
        float steps = mix(12.0, 2.0, bias);
        float step_size = 1.0 / steps;
        float z = abs(ray.z) < 0.00001 ? -0.00001 : ray.z;
        vec2 delta = ray.xy * vec2(-1.0, 1.0) / z * ParallaxHeight * step_size;
        vec2 derivative = abs(dFdx(primary_uv) + dFdy(primary_uv));
        delta *= derivative / max(length(derivative), 0.00001);
        delta *= 1.0 - pow(1.0 - bias, 5.0);
        vec2 current = vec2(1.0);
        vec2 previous = vec2(1.0, 1.1);
        vec2 offset = vec2(0.0);
        for (int i = 0; i < 14; i++) {
            if (i >= int(floor(steps) + 2.0)) break;
            current.x = sample_trinity(ParallaxMap, primary_uv + offset, 8).r;
            if (current.x >= current.y) {
                float diff = current.x - current.y;
                float old_diff = previous.x - previous.y;
                offset -= delta * diff / max(diff - old_diff, 0.00001);
                break;
            }
            previous = current;
            current.y -= step_size;
            offset += delta;
        }
        primary_uv += offset;
    }
    vec2 layer_source = NumRequiredUV > 1 && UVIndexLayerMask == 1 ? UV2 : UV;
    vec2 layer_uv = UVIndexLayerMask < 0 ? primary_uv : vec2(layer_source.x, 1.0 - layer_source.y);
    if (EnableLayerMaskUVTransform) layer_uv = material_layer_mask_uv(vec2(layer_uv.x, 1.0 - layer_uv.y), UVScaleOffsetLayerMask, UVCenterRotationLayerMask);
    vec4 weights = vec4(0.0);
    if (NumMaterialLayer > 1) weights = layer_weights(sample_trinity(LayerMaskMap, layer_uv, 7));
    vec4 texel = vec4(1.0);
    if (EnableBaseColorMap) texel = sample_trinity(BaseColorMap, primary_uv, 0);
    float total = clamp(1.0 - dot(weights, vec4(1.0)), 0.0, 1.0);
    vec4 base = BaseColor * texel;
    base.rgb *= total;
    vec3 shadow_sample = mix(ShadowingColor.rgb, sample_trinity(ShadowingColorMap, primary_uv, 4).rgb,
        sample_trinity(ShadowingColorMaskMap, primary_uv, 5).r * ShadowingColorMaskMapValue);
    shadow_tint = shadow_sample * total;
    if (EnableLerpBaseColorEmission) {
        base.rgb *= max(1.0 - EmissionIntensity, 0.0);
        shadow_tint *= max(1.0 - EmissionIntensity, 0.0);
    }
    for (int i = 0; i < 4; i++) {
        vec4 tint = i == 0 ? BaseColorLayer1 : (i == 1 ? BaseColorLayer2 : (i == 2 ? BaseColorLayer3 : BaseColorLayer4));
        vec3 shade = i == 0 ? ShadowingColorLayer1.rgb : (i == 1 ? ShadowingColorLayer2.rgb : (i == 2 ? ShadowingColorLayer3.rgb : ShadowingColorLayer4.rgb));
        if (EnableColorTableMap) {
            // Row 0.25 holds the layer colors, row 0.75 their shadowing colors.
            int index = i == 0 ? BaseColorIndex1 : (i == 1 ? BaseColorIndex2 : (i == 2 ? BaseColorIndex3 : BaseColorIndex4));
            tint.rgb = color_table(index, 0.25);
            shade = color_table(index, 0.75);
        }
        if (i == 2 && EnableVertexSubLayer) {
            // Vertex color channels select sub-colors for the third layer.
            tint.rgb = mix(tint.rgb, BaseColorSubLayer1.rgb, COLOR.r * SubLayerMaskScale1);
            tint.rgb = mix(tint.rgb, BaseColorSubLayer2.rgb, COLOR.g * SubLayerMaskScale2);
            tint.rgb = mix(tint.rgb, BaseColorSubLayer3.rgb, COLOR.b * SubLayerMaskScale3);
            tint.rgb = mix(tint.rgb, BaseColorSubLayer4.rgb, COLOR.a * SubLayerMaskScale4);
        }
        if (BaseColorMultiply) tint *= texel;
        if (EnableLerpBaseColorEmission && EnableEmissionLayer) {
            float intensity = i == 0 ? EmissionIntensityLayer1 : (i == 1 ? EmissionIntensityLayer2 : (i == 2 ? EmissionIntensityLayer3 : EmissionIntensityLayer4));
            float inv_intensity = max(1.0 - intensity * weights[i], 0.0);
            tint.rgb *= inv_intensity;
            shade *= inv_intensity;
        }
        base.rgb = mix(base.rgb, tint.rgb, weights[i]);
        shadow_tint = mix(shadow_tint, shade, weights[i]);
    }
    if (EnableCustomLayer) {
        base.rgb = mix(base.rgb, BaseColorLayer5.rgb, sample_trinity(IkLayerMaskMap1, transform_uv(UV, UVScaleOffset1, vec2(0.0)), 16).r);
        base.rgb = mix(base.rgb, BaseColorLayer6.rgb, sample_trinity(IkLayerMaskMap2, transform_uv(UV, UVScaleOffset2, vec2(0.0)), 17).r);
        base.rgb = mix(base.rgb, BaseColorLayer7.rgb, sample_trinity(IkLayerMaskMap3, transform_uv(UV, UVScaleOffset3, vec2(0.0)), 18).r);
    }
    float layer5_weight = 0.0;
    if (NumMaterialLayer > 5) {
        vec2 uv5_source = NumRequiredUV > 1 && UVIndexLayer5 != 0 ? UV2 : UV;
        layer5_weight = sample_trinity(OpacityMap1, transform_uv(uv5_source, UVScaleOffset5, vec2(0.0)), 11).r * LayerMaskScale5;
        vec3 tint = BaseColorLayer5.rgb * (BaseColorMultiply ? texel.rgb : vec3(1.0));
        vec3 shade = ShadowingColorLayer5.rgb;
        if (EnableLerpBaseColorEmission && EnableEmissionLayer) {
            tint *= max(1.0 - EmissionIntensityLayer5, 0.0);
            shade *= max(1.0 - EmissionIntensityLayer5, 0.0);
        }
        base.rgb = mix(base.rgb, tint, layer5_weight);
        shadow_tint = mix(shadow_tint, shade, layer5_weight);
    }
    float luminance = dot(base.rgb, vec3(0.2126729, 0.7151522, 0.0721750));
    base.rgb = max(mix(vec3(luminance), base.rgb, SaturationPower) * (1.0 - BaseColorDarkness), vec3(0.0));
    if (EnableHairSpecular) {
        // Hair replaces the alpha test with a channel-weighted mask on the second UV set.
        vec4 cut = sample_trinity(DiscardMaskMap, transform_uv(UV2, UVScaleOffset, vec2(0.0)), 15);
        float keep = mix(mix(mix(mix(1.0, cut.r, DiscardValue_R), cut.g, DiscardValue_G), cut.b, DiscardValue_B), cut.a, DiscardValue_A);
        if (keep < 0.5) discard;
    } else if (EnableAlphaTest && base.a < DiscardValue) discard;

    if (EnableEyeOptions && (RequireEyelidShadowMap || EyelidType == 1 || EyelidType == 3)) {
        float eyelid = sample_trinity(EyelidShadowMaskMap, eye_srt(UV, UVScaleOffset2, UVRotation2), 10).r;
        vec3 lid_color = EnableColorTableMap ? color_table(BaseColorIndex6, 0.25) : BaseColorLayer6.rgb;
        vec3 lid_shade = EnableColorTableMap ? color_table(BaseColorIndex6, 0.75) : BaseColorLayer6.rgb;
        base.rgb *= mix(vec3(1.0), lid_color, eyelid);
        shadow_tint *= mix(vec3(1.0), lid_shade, eyelid);
    }
    float upper_lid = 0.0, lower_lid = 0.0;
    if (EnableEyeOptions && (EyelidType == 1 || EyelidType == 3)) {
        vec4 lid = sample_trinity(UpperEyelidColorMap, eye_srt(UV, UVScaleOffset3, UVRotation3), 13);
        upper_lid = lid.a;
        base.rgb = mix(base.rgb, lid.rgb * BaseColorLayer7.rgb, lid.a);
        shadow_tint = mix(shadow_tint, lid.rgb * BaseColorLayer9.rgb, lid.a);
    }
    if (EnableEyeOptions && (EyelidType == 2 || EyelidType == 3)) {
        vec4 lid = sample_trinity(LowerEyelidColorMap, eye_srt(UV, UVScaleOffset4, UVRotation4), 14);
        lower_lid = lid.a;
        base.rgb = mix(base.rgb, lid.rgb * BaseColorLayer8.rgb, lid.a);
        shadow_tint = mix(shadow_tint, lid.rgb * BaseColorLayer10.rgb, lid.a);
    }
    vec2 ao_source = NumRequiredUV > 1 && UVIndexAO == 1 ? UV2 : UV;
    vec2 ao_uv = UVIndexAO < 0 ? primary_uv : vec2(ao_source.x, 1.0 - ao_source.y);
    AO = pow(max(sample_trinity(OcclusionMap, ao_uv, 2).r, 0.00001), max(OcclusionStrength, 0.0));
    if (EnableNormalMap) NORMAL_MAP = normal_ts * 0.5 + 0.5;
    surface_ao = AO;
    vec3 spec_mask = sample_trinity(SpecularMaskMap, primary_uv, 3).rgb;
    float intensity = blend_values(SpecularIntensity, vec4(SpecularLayer1Intensity, SpecularLayer2Intensity, SpecularLayer3Intensity, SpecularLayer4Intensity), weights);
    float offset = blend_values(SpecularOffset, vec4(SpecularLayer1Offset, SpecularLayer2Offset, SpecularLayer3Offset, SpecularLayer4Offset), weights);
    float contrast = blend_values(SpecularContrast, vec4(SpecularLayer1Contrast, SpecularLayer2Contrast, SpecularLayer3Contrast, SpecularLayer4Contrast), weights);
    intensity = mix(intensity, SpecularLayer5Intensity, layer5_weight);
    offset = mix(offset, SpecularLayer5Offset, layer5_weight);
    contrast = mix(contrast, SpecularLayer5Contrast, layer5_weight);
    surface_gloss = clamp((spec_mask.r - 0.5) * (1.0 + contrast) + 0.5 + offset, 0.0, 1.0);
    surface_specular = max(intensity, 0.0);
    surface_offset = offset;
    surface_contrast = contrast;
    surface_mask = spec_mask.r;
    surface_metallic = clamp(blend_values(Metallic, vec4(MetallicLayer1, MetallicLayer2, MetallicLayer3, MetallicLayer4), weights), 0.0, 1.0);
    surface_metallic = mix(surface_metallic, MetallicLayer5, layer5_weight);
    surface_metallic = mix(mix(surface_metallic, MetallicLayer7, upper_lid), MetallicLayer8, lower_lid);
    // Retail metallic only scales the lit color by the probe reflection (see
    // light()); Godot's metallic would darken diffuse and add IBL specular.
    METALLIC = 0.0;
    ROUGHNESS = 1.0; // Stylized highlights are evaluated explicitly in light().
    SPECULAR = 0.0;
    rim_mask = sample_trinity(RimLightMaskMap, primary_uv, 6).r;
    vec3 emission = EmissionColor.rgb * EmissionIntensity;
    if (EnableEmissionLayer) {
        vec3 layer_emission = blend_colors(vec4(emission, 1.0), EmissionColorLayer1 * EmissionIntensityLayer1, EmissionColorLayer2 * EmissionIntensityLayer2, EmissionColorLayer3 * EmissionIntensityLayer3, EmissionColorLayer4 * EmissionIntensityLayer4, weights).rgb;
        emission = mix(layer_emission, EmissionColorLayer5.rgb * EmissionIntensityLayer5, layer5_weight);
    }
    surface_emission = EnableEmissionLayer ? blend_values(EmissionIntensity,
        vec4(EmissionIntensityLayer1, EmissionIntensityLayer2, EmissionIntensityLayer3, EmissionIntensityLayer4), weights) : EmissionIntensity;
    if (EnableEmissionLayer) surface_emission = mix(surface_emission, EmissionIntensityLayer5, layer5_weight);
    if (EnableEyeOptions && EnableHighlight) {
        float highlight = sample_trinity(HighlightMaskMap, get_uv(UV, 10), 9).r;
        base.rgb = mix(base.rgb, EmissionColorLayer5.rgb * EmissionIntensityLayer5, highlight);
        shadow_tint = mix(shadow_tint, EmissionColorLayer5.rgb * EmissionIntensityLayer5, highlight);
    }
    
    vec3 normal_view = normalize(mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL)) * normal_ts);
    IRRADIANCE = trinity_irradiance(normal_view, INV_VIEW_MATRIX);
    vec3 reflected = (INV_VIEW_MATRIX * vec4(reflect(-VIEW, normal_view), 0.0)).xyz;
    reflection_color = textureLod(LocalReflectionMap, reflected, ReflectionsBlur).rgb;
    reflection_fresnel = clamp(max(smoothstep(0.3, 0.7, clamp(dot(normal_view, VIEW), 0.0, 1.0)), 0.1), 0.0, 1.0);
    hair_binormal = normalize(BINORMAL);
    hair_strength = EnableHairSpecular ? spec_mask.r * SecondSpecularStrength : 0.0;
    if (EnableAuraEffect) {
        float wave = 0.5 + 0.5 * sin(UV.y * AuraColorScale * 2.0 * PI + TIME * AuraColorScrollSpeed);
        float noise = 0.5 + 0.5 * sin(dot(UV, vec2(12.9898, 78.233)) * AuraNoiseScale + TIME * AuraNoiseScrollSpeed);
        float edge = pow(clamp(1.0 - dot(NORMAL, VIEW), 0.0, 1.0), max(AuraRimPower, 0.001));
        emission += EmissionColor.rgb * wave * noise * edge * AuraIntensity;
    }
    ALBEDO = base.rgb;
    EMISSION = emission;

    if (EnableHolocastEffect) {
        // Retail holocast replaces toon lighting with an unlit scanline glow,
        // shaded by a fixed model-space light direction.
        float pitch = radians(LightPitch), yaw = radians(LightYaw), roll = radians(LightRoll);
        float skew = -sin(roll) * sin(pitch);
        vec3 holo_light = vec3(cos(yaw) * skew - sin(yaw) * cos(pitch), -cos(roll) * sin(pitch), cos(pitch) * cos(yaw) + sin(yaw) * skew);
        holo_light = normalize(mat3(VIEW_MATRIX) * (mat3(MODEL_MATRIX) * holo_light));
        float diffuse = dot(normal_view, holo_light) * 0.5 + 0.5;
        diffuse = clamp(mix(diffuse, diffuse * diffuse, HalfLambertBias), 0.0, 1.0);
        float shading_width = max(ShadingBias * ShadingBias * ShadowingBias, 0.00001);
        float shaded_weight = clamp((0.5 * ShadowingBias + 0.5 * shading_width - diffuse) / shading_width, 0.0, 1.0);
        vec3 shaded = base.rgb * clamp(shadow_tint + vec3(AO), vec3(0.0), vec3(1.0)) * mix(vec3(1.0), shadow_tint, shaded_weight);
        float edge = pow(1.0 - clamp(dot(normal_view, VIEW), 0.0, 1.0), 3.0);
        float luma = max(dot(shaded, vec3(0.2126729, 0.7151522, 0.0721750)), edge);
        float scroll = -TIME * ScanlineSpeed;
        vec4 phase = character_world_position.y * vec4(ScanGradationTiling, ScanGradationTiling, ScanlineTiling, ScanlineTiling);
        vec4 seed = sin(sin(phase + vec4(0.5, 0.75, 4.0, 7.8) * scroll) * vec4(20.0, 25.0, 2.1, 3.3));
        float gradation = mix(ScanlineDepth, 1.0, ceil(seed.x) + ceil(seed.y));
        float scan = mix(gradation, gradation * (1.0 - clamp(2.0 * seed.z + seed.w - 1.0, 0.0, 1.0)), 0.5);
        EMISSION = EmissionColor.rgb * EmissionIntensity + luma * scan * HolocastColor.rgb;
        ALBEDO = vec3(0.0);
    }

#ifdef CHARACTER_TRANSPARENT
    float opacity = IgnoreAlbedoAlpha ? BaseColor.a : base.a;
    float premultiplied = 1.0;
    // Thin refraction premultiplies its own output by the Fresnel opacity.
    if (RefractionMode == 1 && FRONT_FACING) {
        float nv = max(dot(normal_view, VIEW) - FresnelAngleBias, 0.0);
        opacity *= FresnelAlphaMin + (FresnelAlphaMax - FresnelAlphaMin) * pow(1.0 - clamp(nv, 0.0, 1.0), 5.0);
        premultiplied = clamp(opacity, 0.0, 1.0);
    }
    vec2 blend_state = trinity_blend(clamp(opacity, 0.0, 1.0), premultiplied);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
#endif
    TRINITY_LIGHT_FRAGMENT;
}
void light() {
    // Holocast output is fully emissive; it receives no scene lighting.
    if (!EnableHolocastEffect) {
        float light_enabled = LIGHT_IS_DIRECTIONAL || EnableAdditionalLight != 0 ? 1.0 : 0.0;
        float ndotl = dot(NORMAL, TRINITY_LIGHT);
        float diffuse = ndotl * 0.5 + 0.5;
        diffuse = clamp(mix(diffuse, diffuse * diffuse, HalfLambertBias), 0.0, 1.0);
        float attenuation = TRINITY_ATTENUATION(ReceiveShadow);
        float shadow = clamp(attenuation + ShadowStrength * ShadowStrength, 0.0, 1.0);
        vec3 light_color = LIGHT_COLOR / PI * light_enabled;
        // Godot LIGHT_COLOR already contains PI. Ikkaku's character shader then
        // divides its source directional RGB by PI again (normalized_light_color).
        if (LIGHT_IS_DIRECTIONAL) light_color /= PI;
        float shading_width = max(ShadingBias * ShadingBias * ShadowingBias, 0.00001);
        float shaded_weight = clamp((0.5 * ShadowingBias + 0.5 * shading_width - diffuse) / shading_width, 0.0, 1.0);
        vec3 shaded = ALBEDO * clamp(shadow_tint + vec3(surface_ao), vec3(0.0), vec3(1.0));
        if (hair_strength > 0.0 && LIGHT_IS_DIRECTIONAL) {
            // Secondary hair highlight: a band along the strand, from a view vector
            // shifted along world up, brightening the base color.
            vec3 shifted_view = VIEW + SpecularShiftValue * (VIEW_MATRIX * vec4(0.0, 1.0, 0.0, 0.0)).xyz;
            vec3 hair_half = normalize(shifted_view + TRINITY_LIGHT);
            float band = smoothstep(0.7 - 0.5 * SecondSpecularWidth, 1.4 - 0.5 * SecondSpecularWidth, 1.0 - abs(dot(hair_binormal, hair_half)));
            shaded *= 1.0 + band * pow(clamp(dot(NORMAL, hair_half), 0.0, 1.0), 4.0) * hair_strength;
        }
        float brightness = max(light_color.r, max(light_color.g, light_color.b)) * diffuse * surface_ao * shadow;
        float mid_area = adjust_contrast(1.0 - smoothstep(MidAreaShift, 1.0 + MidAreaShift, brightness), MidAreaContrast);
        float dark_area = adjust_contrast(1.0 - smoothstep(DarkAreaShift, 1.0 + DarkAreaShift, brightness), DarkAreaContrast);
        vec3 mid_tint = mix(shaded, hue_rotate(shaded, MidAreaHueOffset), mid_area);
        vec3 dark_tint = hue_rotate(shaded, DarkAreaHueOffset);
        float gradient = adjust_contrast(smoothstep(ShadowingShift, 1.0 + ShadowingShift, attenuation * (ndotl * 0.5 + 0.5)), ShadowingContrast);
        vec3 tint = mix(mix(dark_tint, mid_tint, dark_area), mix(mid_tint, dark_tint, dark_area), gradient);
        tint *= mix(1.0, 0.5, HueShiftAreaValue * mid_area);
        shaded = mix(shaded, mix(shaded, tint, HueShiftBias), clamp(light_color, vec3(0.0), vec3(1.0)));
        shaded *= mix(vec3(1.0), shadow_tint, shaded_weight);
        // Godot multiplies accumulated diffuse by ALBEDO after light().
        // Supply only the shading ratio here, otherwise the base color is squared.
        // Metallic scales the lit color by the probe reflection instead of
        // replacing diffuse with specular.
        vec3 reflection = max(min(mix(clamp(reflection_color, 0.0, 0.25), reflection_color, diffuse) * reflection_fresnel, vec3(1.0)), vec3(DiffusionLevels));
        vec3 reflection_scale = mix(vec3(1.0), reflection, surface_metallic);
        DIFFUSE_LIGHT += shaded / max(ALBEDO, vec3(0.00001)) * light_color * shadow * reflection_scale;

        float fresnel = 1.0 - clamp(dot(normalize(NORMAL), normalize(VIEW)), 0.0, 1.0);
        float rim = adjust_contrast(smoothstep(RimLightOffset, 1.0, fresnel), RimLightContrast);
        float rim_front = rim * smoothstep(-0.4, 0.0, ndotl + fresnel - 1.0) * RimLightIntensity * ndotl * surface_ao;
        float rim_back = rim * smoothstep(-0.4, 0.0, -ndotl + fresnel - 1.0);
        float back_weight = rim_back * BackRimLightIntensity;
        rim_back = mix(back_weight, rim_back, back_weight) * RimLightIntensity;
        rim = mix(rim_front, rim_back, clamp(-ndotl, 0.0, 1.0)) * rim_mask * max(1.0 - surface_emission, 0.0);
        vec3 halfway = normalize(TRINITY_LIGHT + VIEW);
        float spec = pow(clamp(dot(NORMAL, halfway), 0.0, 1.0), 8.0);
        spec = adjust_contrast(smoothstep(surface_offset, 1.0 + surface_offset, spec), surface_contrast);
        vec3 specular = light_color * shadow * (spec * surface_specular * surface_mask * surface_ao);
        // Rim and specular share a bounded highlight; adding both overbrightens it.
        specular = clamp(max(specular, vec3(rim)), vec3(0.0), vec3(1.0));
        SPECULAR_LIGHT += specular * clamp(light_color * shadow, vec3(0.0), vec3(1.0)) * light_color * shadow * reflection_scale;
    }
}
