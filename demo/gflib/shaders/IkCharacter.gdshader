shader_type spatial;
render_mode cull_back;

uniform sampler2D BaseColorMap  : source_color, hint_default_white;
uniform sampler2D NormalMap     : hint_normal;
uniform sampler2D OcclusionMap   : hint_default_black;
uniform sampler2D SpecularMaskMap  : hint_default_white;
uniform sampler2D ShadowingColorMap : hint_default_white;
uniform sampler2D ShadowingColorMaskMap         : hint_default_white;
uniform sampler2D RimLightMaskMap        : hint_default_white;
uniform sampler2D LocalReflectionMap        : hint_default_white;
uniform sampler2D LayerMaskMap  : hint_default_black;
uniform sampler2D ParallaxMap   : hint_default_black;
uniform sampler2D HighlightMaskMap : source_color, hint_default_black;
uniform sampler2D EyelidShadowMaskMap      : hint_default_white;

//Hacky way to set sampler data because we cant access slots directly
uniform int sampler_repeat_u[12];
uniform int sampler_repeat_v[12];
const int UV_WRAP = 0;
const int UV_CLAMP = 1;
const int UV_MIRROR = 6;
const int UV_MIRROR_ONCE = 7;

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
uniform int NumMaterialLayer;

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
uniform float ParallaxIOR;
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
uniform int UVIndexAO;
uniform int EnableAdditionalLight;
uniform int UVIndexLayerMask;

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
uniform int NumRequiredUV = 0;
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

// Rendering metadata (category, weather, dirt/jewel and distance dither)
// is retained for the importer; no corresponding scene systems exist here.
float wrap_axis(float value, int mode) {
    if (mode == UV_CLAMP) return clamp(value, 0.0, 1.0);
    if (mode == UV_MIRROR) return 1.0 - abs(mod(value, 2.0) - 1.0);
    if (mode == UV_MIRROR_ONCE) return clamp(abs(value), 0.0, 1.0);
    return fract(value);
}
vec2 wrap_uv(vec2 uv, int slot) {
    return vec2(wrap_axis(uv.x, sampler_repeat_u[slot]), wrap_axis(uv.y, sampler_repeat_v[slot]));
}
vec2 transform_uv(vec2 uv, vec4 scale_offset, vec2 center) {
    vec2 result = scale_offset.xy * (uv - center) + center + scale_offset.zw;
    return vec2(result.x, 1.0 - result.y);
}
vec4 layer_weights(vec4 mask) {
    vec4 weights = clamp(mask * vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4), vec4(0.0), vec4(1.0));
    if (NumMaterialLayer < 2) weights.r = 0.0;
    if (NumMaterialLayer < 3) weights.g = 0.0;
    if (NumMaterialLayer < 4) weights.b = 0.0;
    if (NumMaterialLayer < 5) weights.a = 0.0;
    return weights;
}
vec4 blend_colors(vec4 base, vec4 a, vec4 b, vec4 c, vec4 d, vec4 weights) {
    return mix(mix(mix(mix(base, a, weights.r), b, weights.g), c, weights.b), d, weights.a);
}

// The original programs are unavailable: lighting below is a Godot approximation.
varying vec3 shadow_tint;
varying float surface_specular;
varying float surface_gloss;
varying float rim_mask;

vec2 get_uv(vec2 uv, int slot) {
    vec4 transform = slot == 1 && EnableUVScaleOffsetNormal ? UVScaleOffsetNormal : UVScaleOffset;
    vec2 center = UVCenter0.xy;
    if (EnableEyeOptions && slot == 10) { transform = UVScaleOffset1; center = UVCenter1.xy; }
    if (EnableEyeOptions && slot == 11) {
        transform = UVScaleOffset2;
        float angle = radians(UVRotation2);
        vec2 v = uv - vec2(0.5);
        uv = vec2(cos(angle)*v.x - sin(angle)*v.y, sin(angle)*v.x + cos(angle)*v.y) + vec2(0.5);
    }
    return wrap_uv(transform_uv(uv, transform, center), slot);
}
float blend_values(float base, vec4 layers, vec4 weights) {
    return mix(mix(mix(mix(base, layers.r, weights.r), layers.g, weights.g), layers.b, weights.b), layers.a, weights.a);
}
vec3 hue_rotate(vec3 color, float degrees) {
    vec3 axis = normalize(vec3(1.0));
    float angle = radians(degrees);
    return max(color * cos(angle) + cross(axis, color) * sin(angle) + axis * dot(axis, color) * (1.0 - cos(angle)), vec3(0.0));
}
void fragment() {
    vec2 surface_uv = UV;
    if (EnableEyeOptions && EnableParallaxMap) {
        vec3 view_tangent = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
        float height = texture(ParallaxMap, get_uv(UV, 9)).r;
        float refraction = EnableIrisRefraction != 0 ? 1.0 / max(ParallaxIOR, 1.0) : 1.0;
        surface_uv -= view_tangent.xy / max(abs(view_tangent.z), 0.2) * height * ParallaxHeight * refraction;
    }
    vec2 layer_uv = NumRequiredUV > 1 && UVIndexLayerMask == 1 ? UV2 : surface_uv;
    vec4 weights = vec4(0.0);
    if (NumMaterialLayer > 1) weights = layer_weights(texture(LayerMaskMap, get_uv(layer_uv, 8)));
    vec4 texel = vec4(1.0);
    if (EnableBaseColorMap) texel = texture(BaseColorMap, get_uv(surface_uv, 0));
    vec4 tint = blend_colors(BaseColor, BaseColorLayer1, BaseColorLayer2, BaseColorLayer3, BaseColorLayer4, weights);
    vec4 base = BaseColorMultiply ? texel * tint : tint;
    if (!BaseColorMultiply && EnableBaseColorMap && NumMaterialLayer <= 1) base *= texel;
    float luminance = dot(base.rgb, vec3(0.2126, 0.7152, 0.0722));
    base.rgb = max(mix(vec3(luminance), base.rgb, SaturationPower) * (1.0 - BaseColorDarkness), vec3(0.0));
    if (EnableAlphaTest && base.a < DiscardValue) discard;

    shadow_tint = blend_colors(ShadowingColor, ShadowingColorLayer1, ShadowingColorLayer2, ShadowingColorLayer3, ShadowingColorLayer4, weights).rgb;
    float shadow_mask = clamp(texture(ShadowingColorMaskMap, get_uv(surface_uv, 5)).r + ShadowingColorMaskMapValue, 0.0, 1.0);
    shadow_tint = mix(shadow_tint, texture(ShadowingColorMap, get_uv(surface_uv, 4)).rgb, shadow_mask);
    if (EnableEyeOptions && RequireEyelidShadowMap) {
        float eyelid = texture(EyelidShadowMaskMap, get_uv(UV, 11)).r;
        base.rgb *= mix(BaseColorLayer6.rgb, vec3(1.0), eyelid);
    }
    vec2 ao_uv = NumRequiredUV > 1 && UVIndexAO == 1 ? UV2 : surface_uv;
    AO = EnableAOMap ? mix(1.0, texture(OcclusionMap, get_uv(ao_uv, 2)).r, clamp(OcclusionStrength, 0.0, 1.0)) : 1.0;
    if (EnableNormalMap) {
        vec2 xy = texture(NormalMap, get_uv(UV, 1)).rg * 2.0 - 1.0;
        NORMAL_MAP = vec3(xy, sqrt(max(1.0 - dot(xy, xy), 0.0))) * 0.5 + 0.5;
        NORMAL_MAP_DEPTH = NormalHeight;
    }
    vec3 spec_mask = texture(SpecularMaskMap, get_uv(surface_uv, 3)).rgb;
    float intensity = blend_values(SpecularIntensity, vec4(SpecularLayer1Intensity, SpecularLayer2Intensity, SpecularLayer3Intensity, SpecularLayer4Intensity), weights);
    float offset = blend_values(SpecularOffset, vec4(SpecularLayer1Offset, SpecularLayer2Offset, SpecularLayer3Offset, SpecularLayer4Offset), weights);
    float contrast = blend_values(SpecularContrast, vec4(SpecularLayer1Contrast, SpecularLayer2Contrast, SpecularLayer3Contrast, SpecularLayer4Contrast), weights);
    surface_gloss = clamp((spec_mask.r - 0.5) * (1.0 + contrast) + 0.5 + offset, 0.0, 1.0);
    surface_specular = max(intensity, 0.0);
    METALLIC = clamp(blend_values(Metallic, vec4(MetallicLayer1, MetallicLayer2, MetallicLayer3, MetallicLayer4), weights), 0.0, 1.0);
    ROUGHNESS = clamp(1.0 - (EnableHairSpecular ? spec_mask.g : surface_gloss) + ReflectionsBlur, 0.04, 1.0);
    SPECULAR = clamp(surface_specular / 0.08, 0.0, 1.0);
    rim_mask = texture(RimLightMaskMap, get_uv(surface_uv, 6)).r;
    vec3 emission = EmissionColor.rgb * EmissionIntensity;
    if (EnableEmissionLayer) {
        vec3 layer_emission = blend_colors(vec4(emission, 1.0), EmissionColorLayer1 * EmissionIntensityLayer1, EmissionColorLayer2 * EmissionIntensityLayer2, EmissionColorLayer3 * EmissionIntensityLayer3, EmissionColorLayer4 * EmissionIntensityLayer4, weights).rgb;
        emission = EnableLerpBaseColorEmission ? base.rgb * layer_emission : layer_emission;
    }
    if (EnableEyeOptions && EnableHighlight) emission += texture(HighlightMaskMap, get_uv(UV, 10)).rgb * EmissionColorLayer5.rgb * EmissionIntensityLayer5;
    
    vec3 reflected = reflect(-VIEW, NORMAL);
    vec2 reflection_uv = vec2(atan(reflected.z, reflected.x) / (2.0 * PI) + 0.5, acos(clamp(reflected.y, -1.0, 1.0)) / PI);
    emission += textureLod(LocalReflectionMap, reflection_uv, max(ReflectionsBlur, 0.0) * 8.0).rgb * METALLIC * surface_specular;
    if (EnableAuraEffect) {
        float wave = 0.5 + 0.5 * sin(UV.y * AuraColorScale * 2.0 * PI + TIME * AuraColorScrollSpeed);
        float noise = 0.5 + 0.5 * sin(dot(UV, vec2(12.9898, 78.233)) * AuraNoiseScale + TIME * AuraNoiseScrollSpeed);
        float edge = pow(clamp(1.0 - dot(NORMAL, VIEW), 0.0, 1.0), max(AuraRimPower, 0.001));
        emission += EmissionColor.rgb * wave * noise * edge * AuraIntensity;
    }
    ALBEDO = base.rgb * COLOR.rgb;
    EMISSION = emission;
}
void light() {
    float light_enabled = LIGHT_IS_DIRECTIONAL || EnableAdditionalLight != 0 ? 1.0 : 0.0;
    float ndotl = dot(NORMAL, LIGHT);
    float diffuse = clamp((ndotl + HalfLambertBias) / max(1.0 + HalfLambertBias, 0.001), 0.0, 1.0);
    diffuse = pow(diffuse, max(ShadingBias, 0.001));
    if (DiffusionLevels > 1.0) diffuse = floor(diffuse * DiffusionLevels) / DiffusionLevels;
    float shadow = clamp((diffuse + ShadowingShift) * (1.0 + ShadowingContrast) * ShadowingBias, 0.0, 1.0);
    float mid_area = clamp(1.0 - abs(diffuse - 0.5 - MidAreaShift) * (2.0 + MidAreaContrast), 0.0, 1.0);
    float dark_area = clamp((1.0 - diffuse + DarkAreaShift) * (1.0 + DarkAreaContrast), 0.0, 1.0);
    vec3 tint = hue_rotate(shadow_tint, (MidAreaHueOffset * mid_area + DarkAreaHueOffset * dark_area) * HueShiftBias * HueShiftAreaValue);
    vec3 shaded = mix(ALBEDO, mix(tint, ALBEDO, shadow), clamp(ShadowStrength, 0.0, 1.0));
    float attenuation = (ReceiveShadow != 0 ? ATTENUATION : 1.0) * light_enabled;
    DIFFUSE_LIGHT += shaded * LIGHT_COLOR * (diffuse + (1.0 - diffuse) * ShadowingGIGain) * attenuation / PI;
    vec3 halfway = normalize(LIGHT + VIEW);
    float spec = smoothstep(0.0, 1.0, pow(max(dot(NORMAL, halfway), 0.0), mix(4.0, 12.0, min(surface_gloss, 0.6)))) * clamp(ndotl, 0.0, 1.0);
    float rim = pow(clamp(1.0 - dot(NORMAL, VIEW) + RimLightOffset, 0.0, 1.0), clamp(RimLightContrast, 0.5, 3.0));
    SPECULAR_LIGHT += LIGHT_COLOR * (spec * surface_specular * 0.3 + rim_mask * rim * 0.5 * (RimLightIntensity * max(ndotl, 0.0) + BackRimLightIntensity * max(-ndotl, 0.0))) * attenuation;
}
