shader_type spatial;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D MetallicMap : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, repeat_enable;
uniform sampler2D AOMap : hint_default_white, repeat_enable;
uniform sampler2D EmissionColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D LayerMaskMap : hint_default_black, repeat_enable;

uniform int sampler_repeat_u[7];
uniform int sampler_repeat_v[7];
uniform vec4 sampler_border_color[7];
#include "trinity_sampler.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableNormalMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableEmissionColorMap;
uniform bool EnableAOMap;
uniform bool EnableAlphaTest;
uniform bool EnableLerpBaseColorEmission;
uniform bool EnableUVScaleOffsetNormal;
uniform bool EnableVertexBaseColor;
uniform bool EnableLayerMaskUVTransform;

uniform int NumMaterialLayer = 5;
uniform int NumRequiredUV = 1;
uniform int LayerMaskSource = 1;
uniform int LayerMaskSwizzle = 0;
uniform int LayerBaseMaskSource = 1;
uniform int IridescenceType = 1;
uniform int IridescenceBlendType = 0;
uniform int IridescenceInterpolateType = 0;
uniform int UVIndexLayerMask = -1;
uniform int ReceiveShadow = 1;
uniform int CastShadow = 1;
uniform int CategoryLabel = 0;
uniform int EnableGlobalWeather = 0;
uniform int EnableJewel = 0;

uniform float DiscardValue = 0.0;
uniform float Metallic = 0.0;
uniform float Roughness = 0.2;
uniform float Reflectance = 0.04;
uniform float NormalHeight = 1.0;
uniform float EmissionIntensity = 0.0;
uniform float IridescenceAOBlend = 1.0;
uniform float IridescenceBlend = 1.0;
uniform float IridescenceCenter = 1.0;
uniform float IridescenceEnd = 1.0;
uniform float IridescenceFresnel = 1.0;
uniform float IridescencePower = 1.0;

uniform vec4 BaseColor = vec4(1.0);
uniform vec4 EmissionColor = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetNormal = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetLayerMask = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVCenterRotationLayerMask;
uniform vec4 IridescenceColor1 = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 IridescenceColor2 = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 IridescenceColor3 = vec4(0.5, 0.5, 0.5, 1.0);

uniform float LayerMaskScale1 = 1.0;
uniform float IridescenceBlend1 = 1.0;
uniform float MetallicLayer1 = 0.0;
uniform float RoughnessLayer1 = 0.2;
uniform float EmissionIntensityLayer1 = 0.0;

uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 EmissionColorLayer1 = vec4(0.0, 0.0, 0.0, 1.0);

uniform float LayerMaskScale2 = 1.0;
uniform float IridescenceBlend2 = 1.0;
uniform float MetallicLayer2 = 0.0;
uniform float RoughnessLayer2 = 0.2;
uniform float EmissionIntensityLayer2 = 0.0;

uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 EmissionColorLayer2 = vec4(0.0, 0.0, 0.0, 1.0);

uniform float LayerMaskScale3 = 1.0;
uniform float IridescenceBlend3 = 1.0;
uniform float MetallicLayer3 = 0.0;
uniform float RoughnessLayer3 = 0.2;
uniform float EmissionIntensityLayer3 = 0.0;

uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 EmissionColorLayer3 = vec4(0.0, 0.0, 0.0, 1.0);

uniform float LayerMaskScale4 = 1.0;
uniform float IridescenceBlend4 = 1.0;
uniform float MetallicLayer4 = 0.0;
uniform float RoughnessLayer4 = 0.2;
uniform float EmissionIntensityLayer4 = 0.0;

uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 EmissionColorLayer4 = vec4(0.0, 0.0, 0.0, 1.0);

vec2 transform_uv(vec2 uv, vec4 st) {
    vec2 v = (uv - st.zw) * st.xy;
    return vec2(v.x, 1.0 - v.y);
}

vec3 iridescence_weights(float nv) {
    float edge = pow(1.0 - nv, IridescencePower);
    if (IridescenceType == 1) {
        vec4 alpha = vec4(IridescenceColor1.a, IridescenceColor2.a, IridescenceColor3.a, IridescenceColor3.a - 1.0);
        vec3 center = (alpha.xyz + alpha.wxy) * 0.5;
        vec3 width = alpha.xyz - center;
        vec3 t = vec3(fract((1.0 - nv) * IridescenceCenter));
        t = fract(abs(t - center));
        t = min(t, vec3(1.0) - t);
        if (IridescenceInterpolateType == 1) return vec3(1.0) - clamp((t - width) / max(IridescenceEnd, 0.00001), vec3(0.0), vec3(1.0));
        return vec3(1.0) - smoothstep(vec3(0.0), vec3(max(IridescenceEnd, 0.00001)), t - width);
    }
    float center = abs(IridescenceCenter) < 0.00001 ? 0.00001 : IridescenceCenter;
    float mirrored = edge <= center ? edge / center : 2.0 - edge / center;
    return vec3(smoothstep(IridescenceColor1.a, IridescenceColor2.a, mirrored),
                smoothstep(IridescenceColor2.a, IridescenceColor3.a, mirrored),
                smoothstep(IridescenceColor3.a, IridescenceEnd, mirrored));
}

vec3 iridescence_color(vec3 base, vec3 weights, float rim_weight) {
    vec3 color;
    if (IridescenceType == 1 && IridescenceBlendType == 1) {
        color = IridescenceColor1.rgb * weights.x + IridescenceColor2.rgb * weights.y + IridescenceColor3.rgb * weights.z;
    } else {
        color = mix(base, IridescenceColor1.rgb, weights.x);
        color = mix(color, IridescenceColor2.rgb, weights.y);
        color = mix(color, IridescenceColor3.rgb, weights.z);
    }
    return IridescenceType == 1 ? mix(base, color, rim_weight) : color;
}

float diffuse_weight(float intensity) {
    return EnableLerpBaseColorEmission && !EnableEmissionColorMap ? max(1.0 - intensity, 0.0) : 1.0;
}

void fragment() {
    vec2 primary_uv = transform_uv(UV, UVScaleOffset);
    vec4 base = EnableBaseColorMap ? sample_trinity(BaseColorMap, primary_uv, 0) : BaseColor;
    if (EnableAlphaTest && base.a < DiscardValue) discard;
    vec2 normal_uv = EnableUVScaleOffsetNormal ? transform_uv(UV, UVScaleOffsetNormal) : primary_uv;
    vec3 normal_ts = vec3(0.0, 0.0, 1.0);
    if (EnableNormalMap) {
        vec2 xy = (sample_trinity(NormalMap, normal_uv, 1).rg * 2.0 - 1.0) * NormalHeight;
        normal_ts = vec3(xy, sqrt(1.0 - clamp(dot(xy, xy), 0.0, 1.0)));
        NORMAL_MAP = normal_ts * 0.5 + 0.5;
    }
    vec3 normal_view = normalize(mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL)) * normal_ts);
    normal_view *= FRONT_FACING ? 1.0 : -1.0;
    float nv = clamp(dot(normal_view, VIEW), 0.00001, 1.0);
    float metallic = EnableMetallicMap ? sample_trinity(MetallicMap, primary_uv, 2).r : Metallic;
    float roughness = EnableRoughnessMap ? sample_trinity(RoughnessMap, primary_uv, 3).r : Roughness;
    float ao = EnableAOMap ? sample_trinity(AOMap, vec2(UV.x, 1.0 - UV.y), 4).r : 1.0;
    vec3 emission = (EnableEmissionColorMap ? sample_trinity(EmissionColorMap, primary_uv, 5).rgb : EmissionColor.rgb) * EmissionIntensity;
    vec3 spectral_weights = iridescence_weights(nv);
    float edge_weight = pow(1.0 - nv, IridescencePower);
    float rim_weight = mix(1.0, IridescenceFresnel > 0.0 ? edge_weight : 1.0 - edge_weight, abs(IridescenceFresnel));
    float blend = mix(IridescenceBlend, IridescenceBlend * ao, IridescenceAOBlend);
    float reflectance = diffuse_weight(EmissionIntensity);
    if (NumMaterialLayer > 1) {
        vec4 mask = vec4(1.0);
        if (LayerMaskSource == 1) {
            vec2 raw = UVIndexLayerMask != 0 && NumRequiredUV > 1 ? UV2 : UV;
            vec2 mask_uv = UVIndexLayerMask < 0 ? primary_uv : vec2(raw.x, 1.0 - raw.y);
            if (EnableLayerMaskUVTransform) {
                raw = vec2(mask_uv.x, 1.0 - mask_uv.y) - UVCenterRotationLayerMask.xy;
                float angle = radians(UVCenterRotationLayerMask.z);
                raw = mat2(vec2(cos(angle), sin(angle)), vec2(-sin(angle), cos(angle))) * raw;
                vec2 st = (raw - UVScaleOffsetLayerMask.zw) * UVScaleOffsetLayerMask.xy + UVCenterRotationLayerMask.xy;
                mask_uv = vec2(st.x, 1.0 - st.y);
            }
            mask = sample_trinity(LayerMaskMap, mask_uv, 6);
        } else if (LayerMaskSource == 2) mask = COLOR;
        if (LayerMaskSwizzle == 1) mask = vec4(mask.r, 1.0, 1.0, 1.0);
        if (LayerMaskSwizzle == 2) mask = vec4(mask.a, 1.0, 1.0, 1.0);
        mask *= vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4);
        if (NumMaterialLayer < 3) mask.g = 0.0;
        if (NumMaterialLayer < 4) mask.b = 0.0;
        if (NumMaterialLayer < 5) mask.a = 0.0;
        vec3 base0 = base.rgb;
        float weight = LayerBaseMaskSource == 1 ? clamp(1.0 - dot(mask, vec4(1.0)), 0.0, 1.0) : 1.0;
        base.rgb *= weight * reflectance;
        reflectance *= weight;
        base.rgb = mix(base.rgb, iridescence_color(base.rgb, spectral_weights, rim_weight), blend * weight);
        if (!EnableMetallicMap) metallic *= weight;
        if (!EnableRoughnessMap) roughness *= weight;
        if (!EnableEmissionColorMap) emission *= weight;
        for (int i = 0; i < 4; i++) {
            vec3 tint = i == 0 ? BaseColorLayer1.rgb : (i == 1 ? BaseColorLayer2.rgb : (i == 2 ? BaseColorLayer3.rgb : BaseColorLayer4.rgb));
            float iblend = i == 0 ? IridescenceBlend1 : (i == 1 ? IridescenceBlend2 : (i == 2 ? IridescenceBlend3 : IridescenceBlend4));
            float intensity = i == 0 ? EmissionIntensityLayer1 : (i == 1 ? EmissionIntensityLayer2 : (i == 2 ? EmissionIntensityLayer3 : EmissionIntensityLayer4));
            float m = i == 0 ? MetallicLayer1 : (i == 1 ? MetallicLayer2 : (i == 2 ? MetallicLayer3 : MetallicLayer4));
            float r = i == 0 ? RoughnessLayer1 : (i == 1 ? RoughnessLayer2 : (i == 2 ? RoughnessLayer3 : RoughnessLayer4));
            vec3 e = i == 0 ? EmissionColorLayer1.rgb : (i == 1 ? EmissionColorLayer2.rgb : (i == 2 ? EmissionColorLayer3.rgb : EmissionColorLayer4.rgb));
            vec3 layer = base0 * tint;
            // Retail layer 1 attenuates emission before the spectral blend;
            // layers 2..4 attenuate after it.
            if (i == 0) layer *= diffuse_weight(intensity);
            layer = mix(layer, iridescence_color(layer, spectral_weights, rim_weight), blend * iblend * mask[i]);
            if (i != 0) layer *= diffuse_weight(intensity);
            base.rgb = mix(base.rgb, layer, mask[i]);
            reflectance = mix(reflectance, diffuse_weight(intensity), mask[i]);
            if (!EnableMetallicMap) metallic = mix(metallic, m, mask[i]);
            if (!EnableRoughnessMap) roughness = mix(roughness, r, mask[i]);
            if (!EnableEmissionColorMap) emission = mix(emission, e * intensity, mask[i]);
            weight = mix(weight, 1.0, mask[i]);
        }
        float inv_weight = 1.0 / max(weight, 0.00001);
        base.rgb *= inv_weight;
        if (!EnableMetallicMap) metallic *= inv_weight;
        if (!EnableRoughnessMap) roughness *= inv_weight;
        if (!EnableEmissionColorMap) emission *= inv_weight;
    } else base.rgb *= reflectance;
    if (EnableVertexBaseColor) base *= COLOR;
    
    ALBEDO = base.rgb;
    METALLIC = metallic;
    ROUGHNESS = roughness;
    SPECULAR = clamp(Reflectance * reflectance / 0.08, 0.0, 1.0);
    EMISSION = emission;
    AO = ao;
}
