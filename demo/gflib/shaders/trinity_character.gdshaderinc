
#ifdef CHARACTER_TRANSPARENT
uniform int RefractionMode;
uniform bool IgnoreAlbedoAlpha;
uniform float FresnelAlphaMin = 1.0;
uniform float FresnelAlphaMax = 1.0;
uniform float FresnelAngleBias;
uniform bool EnableHolocastEffect;
uniform vec4 HolocastColor = vec4(1.0);
uniform float ScanGradationTiling = 1.0;
uniform float ScanlineTiling = 1.0;
uniform float ScanlineDepth;
uniform float ScanlineSpeed;
varying vec3 character_world_position;
void vertex() { character_world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz; }
#endif
uniform sampler2D BaseColorMap  : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap     : hint_normal, repeat_enable;
uniform sampler2D OcclusionMap   : hint_default_white, repeat_enable;
uniform sampler2D SpecularMaskMap  : hint_default_white, repeat_enable;
uniform sampler2D ShadowingColorMap : hint_default_white, repeat_enable;
uniform sampler2D ShadowingColorMaskMap         : hint_default_white, repeat_enable;
uniform sampler2D RimLightMaskMap        : hint_default_white, repeat_enable;
uniform sampler2D LocalReflectionMap        : hint_default_white, repeat_enable;
uniform sampler2D LayerMaskMap  : hint_default_black, repeat_enable;
uniform sampler2D ParallaxMap   : hint_default_black, repeat_enable;
uniform sampler2D HighlightMaskMap : source_color, hint_default_black, repeat_enable;
uniform sampler2D EyelidShadowMaskMap      : hint_default_white, repeat_enable;
uniform sampler2D OpacityMap1 : hint_default_black, repeat_enable;

// Shader slots are resolved by texture name in the material loader.
uniform int sampler_repeat_u[13];
uniform int sampler_repeat_v[13];
uniform vec4 sampler_border_color[13];
#include "trinity_sampler.gdshaderinc"

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

float adjust_contrast(float value, float contrast) {
    return clamp((value - 0.5) * (1.0 + 2.0 * contrast) + 0.5, 0.0, 1.0);
}

vec2 get_uv(vec2 uv, int slot) {
    vec4 transform = slot == 1 && EnableUVScaleOffsetNormal ? UVScaleOffsetNormal : UVScaleOffset;
    vec2 center = slot == 1 && EnableUVScaleOffsetNormal ? vec2(0.0) : ((EnableEyeOptions || EnableParallaxMap) ? UVCenter0.xy : vec2(0.0));
    if (EnableEyeOptions && slot == 10) { transform = UVScaleOffset1; center = UVCenter1.xy; }
    if (EnableEyeOptions && slot == 11) {
        transform = UVScaleOffset2;
        float angle = UVRotation2;
        vec2 v = uv - vec2(0.5);
        uv = vec2(cos(angle)*v.x - sin(angle)*v.y, sin(angle)*v.x + cos(angle)*v.y) + vec2(0.5);
    }
    return transform_uv(uv, transform, center);
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
            current.x = sample_trinity(ParallaxMap, primary_uv + offset, 9).r;
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
    vec4 weights = vec4(0.0);
    if (NumMaterialLayer > 1) weights = layer_weights(sample_trinity(LayerMaskMap, layer_uv, 8));
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
    float layer5_weight = 0.0;
    if (NumMaterialLayer > 5) {
        vec2 uv5_source = NumRequiredUV > 1 && UVIndexLayer5 != 0 ? UV2 : UV;
        layer5_weight = sample_trinity(OpacityMap1, transform_uv(uv5_source, UVScaleOffset5, vec2(0.0)), 12).r * LayerMaskScale5;
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
    if (EnableAlphaTest && base.a < DiscardValue) discard;

    if (EnableEyeOptions && RequireEyelidShadowMap) {
        float eyelid = sample_trinity(EyelidShadowMaskMap, get_uv(UV, 11), 11).r;
        vec3 tint = mix(vec3(1.0), BaseColorLayer6.rgb, eyelid);
        base.rgb *= tint;
        shadow_tint *= tint;
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
    METALLIC = clamp(blend_values(Metallic, vec4(MetallicLayer1, MetallicLayer2, MetallicLayer3, MetallicLayer4), weights), 0.0, 1.0);
    METALLIC = mix(METALLIC, MetallicLayer5, layer5_weight);
    ROUGHNESS = 1.0; // Stylized highlights are evaluated explicitly in light().
    SPECULAR = clamp(surface_specular / 0.08, 0.0, 1.0);
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
        float highlight = sample_trinity(HighlightMaskMap, get_uv(UV, 10), 10).r;
        base.rgb = mix(base.rgb, EmissionColorLayer5.rgb * EmissionIntensityLayer5, highlight);
        shadow_tint = mix(shadow_tint, EmissionColorLayer5.rgb * EmissionIntensityLayer5, highlight);
    }
    
    vec3 reflected = reflect(-VIEW, NORMAL);
    vec2 reflection_uv = vec2(atan(reflected.z, reflected.x) / (2.0 * PI) + 0.5, acos(clamp(reflected.y, -1.0, 1.0)) / PI);
    emission += sample_trinity(LocalReflectionMap, reflection_uv, 7).rgb * METALLIC * surface_specular;
    if (EnableAuraEffect) {
        float wave = 0.5 + 0.5 * sin(UV.y * AuraColorScale * 2.0 * PI + TIME * AuraColorScrollSpeed);
        float noise = 0.5 + 0.5 * sin(dot(UV, vec2(12.9898, 78.233)) * AuraNoiseScale + TIME * AuraNoiseScrollSpeed);
        float edge = pow(clamp(1.0 - dot(NORMAL, VIEW), 0.0, 1.0), max(AuraRimPower, 0.001));
        emission += EmissionColor.rgb * wave * noise * edge * AuraIntensity;
    }
    ALBEDO = base.rgb;
    EMISSION = emission;

#ifdef CHARACTER_TRANSPARENT
    if (!FRONT_FACING) NORMAL = -NORMAL;
    float opacity = IgnoreAlbedoAlpha ? BaseColor.a : base.a;
    // Thin refraction is premultiplied in retail; straight alpha in Godot
    // yields the same framebuffer product. Apply it only to front faces.
    if (RefractionMode == 1 && FRONT_FACING) {
        float nv = max(dot(normalize(mat3(TANGENT, BINORMAL, NORMAL) * normal_ts), VIEW) - FresnelAngleBias, 0.0);
        opacity *= FresnelAlphaMin + (FresnelAlphaMax - FresnelAlphaMin) * pow(1.0 - clamp(nv, 0.0, 1.0), 5.0);
    }
    ALPHA = clamp(opacity, 0.0, 1.0);
    if (EnableHolocastEffect) {
        float edge = pow(clamp(1.0 - dot(normalize(NORMAL), VIEW), 0.0, 1.0), 3.0);
        float luma = max(dot(ALBEDO, vec3(0.2126729, 0.7151522, 0.0721750)), edge);
        float scroll = -TIME * ScanlineSpeed;
        vec4 phase = character_world_position.y * vec4(ScanGradationTiling, ScanGradationTiling, ScanlineTiling, ScanlineTiling);
        vec4 seed = sin(sin(phase + vec4(0.5, 0.75, 4.0, 7.8) * scroll) * vec4(20.0, 25.0, 2.1, 3.3));
        float gradation = mix(ScanlineDepth, 1.0, ceil(seed.x) + ceil(seed.y));
        float scan = 1.0 - clamp(seed.z + seed.w, 0.0, 1.0);
        scan = mix(gradation, gradation * scan, 0.5);
        EMISSION += luma * scan * HolocastColor.rgb;
        ALBEDO = vec3(0.0);
    }
#endif
}
void light() {
    float light_enabled = LIGHT_IS_DIRECTIONAL || EnableAdditionalLight != 0 ? 1.0 : 0.0;
    float ndotl = dot(NORMAL, LIGHT);
    float diffuse = ndotl * 0.5 + 0.5;
    diffuse = clamp(mix(diffuse, diffuse * diffuse, HalfLambertBias), 0.0, 1.0);
    float attenuation = ReceiveShadow > 0 ? ATTENUATION : 1.0;
    float shadow = clamp(attenuation + ShadowStrength * ShadowStrength, 0.0, 1.0);
    vec3 light_color = LIGHT_COLOR / PI * light_enabled;
    // Godot LIGHT_COLOR already contains PI. Ikkaku's character shader then
    // divides its source directional RGB by PI again (normalized_light_color).
    if (LIGHT_IS_DIRECTIONAL) light_color /= PI;
    float shading_width = max(ShadingBias * ShadingBias * ShadowingBias, 0.00001);
    float shaded_weight = clamp((0.5 * ShadowingBias + 0.5 * shading_width - diffuse) / shading_width, 0.0, 1.0);
    vec3 shaded = ALBEDO * clamp(shadow_tint + vec3(surface_ao), vec3(0.0), vec3(1.0));
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
    DIFFUSE_LIGHT += shaded / max(ALBEDO, vec3(0.00001)) * light_color * shadow;

    float fresnel = 1.0 - clamp(dot(normalize(NORMAL), normalize(VIEW)), 0.0, 1.0);
    float rim = adjust_contrast(smoothstep(RimLightOffset, 1.0, fresnel), RimLightContrast);
    float rim_front = rim * smoothstep(-0.4, 0.0, ndotl + fresnel - 1.0) * RimLightIntensity * ndotl * surface_ao;
    float rim_back = rim * smoothstep(-0.4, 0.0, -ndotl + fresnel - 1.0);
    float back_weight = rim_back * BackRimLightIntensity;
    rim_back = mix(back_weight, rim_back, back_weight) * RimLightIntensity;
    rim = mix(rim_front, rim_back, clamp(-ndotl, 0.0, 1.0)) * rim_mask * max(1.0 - surface_emission, 0.0);
    vec3 halfway = normalize(LIGHT + VIEW);
    float spec = pow(clamp(dot(NORMAL, halfway), 0.0, 1.0), 8.0);
    spec = adjust_contrast(smoothstep(surface_offset, 1.0 + surface_offset, spec), surface_contrast);
    vec3 specular = light_color * shadow * (spec * surface_specular * surface_mask * surface_ao);
    // Rim and specular share a bounded highlight; adding both overbrightens it.
    specular = clamp(max(specular, vec3(rim)), vec3(0.0), vec3(1.0));
    SPECULAR_LIGHT += specular * clamp(light_color * shadow, vec3(0.0), vec3(1.0)) * light_color * shadow;
}
