
#ifdef TRANSPARENT_MATERIAL
#include "trinity_refraction.gdshaderinc"
uniform int RefractionMode;
uniform float FresnelAlphaMin = 1.0;
uniform float FresnelAlphaMax = 1.0;
uniform float FresnelAngleBias;
#endif
#ifdef BG_WIND
uniform int UvsetType;
#endif
#ifdef BG_LAYER
uniform bool EnableMetallicMap1;
uniform bool EnableEmissionColorMap1;
uniform float LayerMaskLevelMin;
uniform float LayerMaskLevelMax = 1.0;
#endif
#ifdef EYE_MATERIAL
uniform int EyelidType;
uniform int UVTransformMode;
uniform int EnableIrisRefraction;
uniform bool EnableHighlight;
uniform bool EnableOverrideColor;
uniform bool EnableEyeClearCoat;
uniform bool RequireHighlightMap;
uniform bool RequireEyelidShadowMap;
uniform float ParallaxIOR = 1.333;
uniform float UVRotation;
uniform float UVRotationNormal;
uniform float UVRotation2;
uniform float UVRotation3;
uniform float UVRotation4;
uniform vec4 BaseColorLayer6 = vec4(1.0);
uniform vec4 BaseColorLayer7 = vec4(1.0);
uniform vec4 BaseColorLayer8 = vec4(1.0);
uniform float MetallicLayer7;
uniform float MetallicLayer8;
uniform float RoughnessLayer7 = 0.5;
uniform float RoughnessLayer8 = 0.5;
vec2 eye_uv(vec2 uv, vec4 transform, float rotation) {
    vec2 result;
    if (UVTransformMode == 1) result = uv - transform.zw;
    else {
        float c = cos(rotation), sine = sin(rotation);
        result = (mat2(vec2(c, -sine), vec2(sine, c)) * (uv - vec2(0.5)) - transform.zw) * transform.xy + vec2(0.5);
    }
    return vec2(result.x, 1.0 - result.y);
}
#endif

#ifdef INSIDE_EMISSION_PARALLAX
uniform float InsideEmissionParallaxHeight;
uniform float InsideEmissionParallaxOffset;
uniform float InsideEmissionParallaxIntensity;
uniform float InsideEmissionParallaxF0 = 1.0;
uniform float InsideEmissionParallaxF90 = 1.0;
uniform float InsideEmissionParallaxIntensityMapLatticeU = 1.0;
uniform float InsideEmissionParallaxIntensityMapLatticeV = 1.0;
uniform float InsideEmissionParallaxIntensityMapRangeAvoidEdge;
uniform vec4 UVScaleOffsetInsideEmissionParallaxHeight = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetInsideEmissionParallaxIntensity = vec4(1.0, 1.0, 0.0, 0.0);
uniform int UVIndexInsideEmissionParallaxHeight;
uniform int UVIndexInsideEmissionParallaxIntensity;
uniform float EnableIEPTexture1 = 1.0;
uniform float EnableIEPTexture2 = 1.0;
uniform float EnableIEPTexture3 = 1.0;
uniform float EnableIEPTexture4 = 1.0;
uniform float EmissionRimF0;
uniform float EmissionRimF90 = 1.0;
uniform float EmissionRimThreshold = 0.5;
uniform float EmissionRimCurve = 1.0;
uniform float EmissionRimRate;
uniform vec4 EmissionRimColor = vec4(1.0);
vec2 inside_repeat_uv(vec2 uv, vec2 offset) {
    vec2 lattice = 1.0 / max(vec2(InsideEmissionParallaxIntensityMapLatticeU, InsideEmissionParallaxIntensityMapLatticeV), vec2(0.00001));
    vec2 edge = min(vec2(InsideEmissionParallaxIntensityMapRangeAvoidEdge * 0.01), lattice * 0.49999);
    vec2 minimum = uv - mod(uv, lattice) + edge;
    vec2 span = max(lattice - 2.0 * edge, vec2(0.00001));
    return minimum + mod(uv + offset - minimum, span);
}
#endif
// Retail core material equations; renderer-dependent gaps are in docs/shaders/retail.md.
uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D NormalMap1 : hint_normal, repeat_enable;
uniform sampler2D NormalMap2 : hint_normal, repeat_enable;
uniform sampler2D MetallicMap : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap1 : hint_default_white, repeat_enable;
uniform sampler2D AOMap : hint_default_white, repeat_enable;
uniform sampler2D AOMap1 : hint_default_white, repeat_enable;
uniform sampler2D AOMap2 : hint_default_white, repeat_enable;
uniform sampler2D LayerMaskMap : hint_default_white, repeat_enable;
uniform sampler2D ParallaxMap : hint_default_white, repeat_enable;
uniform sampler2D EmissionColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D LightMap : hint_default_white, repeat_enable;
uniform sampler2D WindMaskMap : hint_default_white, repeat_enable;
uniform sampler2D PackedMap : hint_default_white, repeat_enable;
uniform sampler2D DetailMaskMap : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap2 : hint_default_white, repeat_enable;
uniform sampler2D OpacityMap1 : hint_default_white, repeat_enable;
uniform sampler2D ColorTableMap : source_color, hint_default_white, repeat_enable;
// Variant textures follow the shared ones so sample_trinity slots stay stable.
#ifdef BG_LAYER
uniform sampler2D MetallicMap1 : hint_default_white, repeat_enable;
uniform sampler2D EmissionColorMap1 : source_color, hint_default_white, repeat_enable;
#endif
#ifdef INSIDE_EMISSION_PARALLAX
uniform sampler2D InsideEmissionParallaxIntensityMap : source_color, hint_default_black, repeat_enable;
#endif
#ifdef EYE_MATERIAL
uniform sampler2D UpperEyelidColorMap : source_color, hint_default_transparent, repeat_enable;
uniform sampler2D LowerEyelidColorMap : source_color, hint_default_transparent, repeat_enable;
uniform sampler2D HighlightMaskMap : hint_default_black, repeat_enable;
uniform sampler2D EyelidShadowMaskMap : hint_default_black, repeat_enable;
#endif
#ifdef EYE_MATERIAL
uniform int sampler_repeat_u[25];
uniform int sampler_repeat_v[25];
uniform vec4 sampler_border_color[25];
#else
#ifdef BG_LAYER
uniform int sampler_repeat_u[23];
uniform int sampler_repeat_v[23];
uniform vec4 sampler_border_color[23];
#else
#ifdef INSIDE_EMISSION_PARALLAX
uniform int sampler_repeat_u[22];
uniform int sampler_repeat_v[22];
uniform vec4 sampler_border_color[22];
#else
uniform int sampler_repeat_u[21];
uniform int sampler_repeat_v[21];
uniform vec4 sampler_border_color[21];
#endif
#endif
#endif
#include "trinity_sampler.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableNormalMap;
uniform bool EnableParallaxMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableEmissionColorMap;
uniform bool EnableAOMap;
uniform bool EnableAlphaTest;
uniform bool EnableLightMap;
uniform bool EnableWindMaskMap;
uniform bool EnableBaseColorMap1;
uniform bool EnableNormalMap1;
uniform bool EnableNormalMap2;
uniform bool EnableRoughnessMap1;
uniform bool EnableAOMap1;
uniform bool EnableAOMap2;
uniform bool EnableVertexBaseColor;
uniform bool EnableLerpBaseColorEmission;
uniform bool EnablePackedMap;
uniform bool EnableAlphaPackedMap;
uniform bool EnableEmissionPackedMap;
uniform bool EnableUVScaleOffsetNormal;
uniform bool EnableLayerMaskUVTransform;
uniform bool EnableColorTableMap;
uniform int NumMaterialLayer = 1;
uniform int NumRequiredUV = 1;
uniform int NumDetailLayer = 0;
uniform int LayerMaskSource = 1;
uniform int LayerMaskSwizzle = 0;
uniform int LayerBaseMaskSource = 1;
uniform int LayerBlendType = 0;
uniform int WindReceiverType = 0;
uniform int UVIndexAO = -1;
uniform int UVIndexLayerMask = -1;
uniform int CastShadow = 1;
uniform int ReceiveShadow = 1;
uniform float Metallic = 0.0;
uniform float Roughness = 0.2;
uniform float Reflectance = 0.04;
uniform float NormalHeight = 1.0;
uniform float NormalHeight1 = 1.0;
uniform float NormalHeight2 = 1.0;
uniform float ParallaxHeight = 0.01;
uniform float DiscardValue = 0.0;
uniform float EmissionIntensity = 0.0;
uniform float VertexColorIntensity = 1.0;
uniform float MetallicIntensity = 1.0;
uniform float RoughnessIntensity = 1.0;
uniform float ColorTableDivideNumber = 1.0;
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetNormal = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetLayerMask = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset2 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset3 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset4 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset5 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVCenterRotationLayerMask = vec4(0.0);
uniform int UVIndexLayer1;
uniform float LayerMaskScale1 = 1.0;
uniform float MetallicLayer1;
uniform float RoughnessLayer1 = 0.2;
uniform float EmissionIntensityLayer1;
uniform float BaseColorIndex1 = 1.0;
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 EmissionColorLayer1 = vec4(1.0);
uniform int UVIndexLayer2;
uniform float LayerMaskScale2 = 1.0;
uniform float MetallicLayer2;
uniform float RoughnessLayer2 = 0.2;
uniform float EmissionIntensityLayer2;
uniform float BaseColorIndex2 = 1.0;
uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 EmissionColorLayer2 = vec4(1.0);
uniform int UVIndexLayer3;
uniform float LayerMaskScale3 = 1.0;
uniform float MetallicLayer3;
uniform float RoughnessLayer3 = 0.2;
uniform float EmissionIntensityLayer3;
uniform float BaseColorIndex3 = 1.0;
uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 EmissionColorLayer3 = vec4(1.0);
uniform int UVIndexLayer4;
uniform float LayerMaskScale4 = 1.0;
uniform float MetallicLayer4;
uniform float RoughnessLayer4 = 0.2;
uniform float EmissionIntensityLayer4;
uniform float BaseColorIndex4 = 1.0;
uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 EmissionColorLayer4 = vec4(1.0);
uniform int UVIndexLayer5;
uniform float LayerMaskScale5 = 1.0;
uniform float MetallicLayer5;
uniform float RoughnessLayer5 = 0.2;
uniform float EmissionIntensityLayer5;
uniform float BaseColorIndex5 = 1.0;
uniform vec4 BaseColorLayer5 = vec4(1.0);
uniform vec4 EmissionColorLayer5 = vec4(1.0);

vec2 transform_uv(vec2 uv, vec4 st) {
    vec2 result = (uv - st.zw) * st.xy;
    return vec2(result.x, 1.0 - result.y);
}
vec2 select_uv(vec2 uv0, vec2 uv1, int index) {
    return index != 0 && NumRequiredUV > 1 ? uv1 : uv0;
}
vec2 layer_uv(vec2 uv0, vec2 uv1, int index, vec4 st) {
    return transform_uv(select_uv(uv0, uv1, index), st);
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
vec3 blend_color(vec3 base, vec3 layer, float weight) {
    if (LayerBlendType == 1) return base + layer * weight;
    if (LayerBlendType == 2) return mix(base, base * layer, weight);
    return mix(base, layer, weight);
}
vec3 layer_color(vec4 color, float index) {
#ifdef EYE_MATERIAL
    if (EnableColorTableMap) return texelFetch(ColorTableMap, ivec2(clamp(int(index - 1.0), 0, textureSize(ColorTableMap, 0).x - 1), 0), 0).rgb;
#endif
    if (EnableColorTableMap) return sample_trinity(ColorTableMap, vec2((index - 0.5) / max(ColorTableDivideNumber, 1.0), 0.0), 20).rgb;
    return color.rgb;
}
float diffuse_weight(float intensity) {
#ifdef EYE_MATERIAL
    return !EnableEmissionColorMap ? max(1.0 - intensity, 0.0) : 1.0;
#else
    return EnableLerpBaseColorEmission && !EnableEmissionColorMap ? max(1.0 - intensity, 0.0) : 1.0;
#endif
}
void vertex() {
    // Approximation retained until the original wind controller is available.
    if (WindReceiverType != 0) {
        float mask = EnableWindMaskMap ? sample_trinity(WindMaskMap, transform_uv(UV, UVScaleOffset), 15).r : 1.0;
        vec3 world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
        VERTEX.x += sin(TIME * 1.7 + dot(world_position.xz, vec2(0.7, 0.5))) * mask * 0.025;
    }
}
void fragment() {
    vec2 uv = transform_uv(UV, UVScaleOffset);
#ifdef EYE_MATERIAL
    uv = eye_uv(UV, UVScaleOffset, UVRotation);
    if (EnableParallaxMap) {
        vec3 ray = -(VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL)));
        if (EnableIrisRefraction > 0) ray = refract(ray, vec3(0.0, 0.0, 1.0), 1.0 / max(ParallaxIOR, 1.00001));
        ray = normalize(ray);
        float nv = abs(dot(NORMAL, VIEW));
        float steps = mix(12.0, 2.0, nv);
        vec2 delta = ray.xy * vec2(-1.0, 1.0) / min(ray.z, -0.00001) * ParallaxHeight / steps;
        vec2 derivative = abs(dFdx(uv) + dFdy(uv));
        delta *= derivative / max(length(derivative), 0.00001);
        if (!EnableEyeClearCoat) delta *= 1.0 - pow(1.0 - nv, 5.0);
        vec2 previous = vec2(1.0, 1.1), current = vec2(1.0), offset = vec2(0.0);
        for (int step_index = 0; step_index < 14; step_index++) {
            if (step_index >= int(floor(steps) + 2.0)) break;
            current.x = sample_trinity(ParallaxMap, uv + offset, 12).r;
            if (current.x >= current.y) {
                float difference = current.x - current.y;
                float denominator = difference - (previous.x - previous.y);
                offset -= delta * difference / max(denominator, 0.00001);
                break;
            }
            previous = current;
            current.y -= 1.0 / steps;
            offset += delta;
        }
        uv += offset;
    }
#else
    if (EnableParallaxMap) {
        vec3 view_ts = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
        float z = abs(view_ts.z) < 0.00001 ? 0.00001 : view_ts.z;
        uv += view_ts.xy / z * sample_trinity(ParallaxMap, uv, 12).r * ParallaxHeight;
    }
#endif
    // The texture replaces BaseColor in Standard; it does not multiply it.
    vec4 base = EnableBaseColorMap ? sample_trinity(BaseColorMap, uv, 0) : BaseColor;
#ifdef INSIDE_EMISSION_PARALLAX
    if (EnableBaseColorMap) base.a = BaseColor.a;
#endif
    if (EnableAlphaTest && base.a < DiscardValue) discard;
    vec3 base0 = base.rgb;
    vec2 normal_uv = EnableUVScaleOffsetNormal ? transform_uv(UV, UVScaleOffsetNormal) : uv;
#ifdef EYE_MATERIAL
    normal_uv = EnableEyeClearCoat ? uv : eye_uv(UV, UVScaleOffsetNormal, UVRotationNormal);
#endif
#ifdef BG_WIND
    normal_uv = UvsetType == 0 ? uv : vec2(UV2.x, 1.0 - UV2.y);
#endif
    vec3 normal_ts = EnableNormalMap ? unpack_normal(sample_trinity(NormalMap, normal_uv, 2), NormalHeight) : vec3(0.0, 0.0, 1.0);
    float metallic = EnableMetallicMap ? sample_trinity(MetallicMap, uv, 5).r : Metallic;
    float roughness = EnableRoughnessMap ? sample_trinity(RoughnessMap, uv, 6).r : Roughness;
#ifdef EYE_MATERIAL
    roughness *= base.a;
#endif
    vec3 emission = (EnableEmissionColorMap ? sample_trinity(EmissionColorMap, uv, 13).rgb : EmissionColor.rgb) * EmissionIntensity;
    vec2 ao_uv = UVIndexAO < 0 ? uv : vec2(select_uv(UV, UV2, UVIndexAO).x, 1.0 - select_uv(UV, UV2, UVIndexAO).y);
    float ao = EnableAOMap ? sample_trinity(AOMap, ao_uv, 8).r : 1.0;
    if (EnablePackedMap || EnableAlphaPackedMap || EnableEmissionPackedMap) {
        vec3 packed = sample_trinity(PackedMap, uv, 16).rgb;
        if (EnablePackedMap) {
            ao = packed.r; roughness = packed.g; metallic = packed.b;
        } else {
            if (EnableAlphaPackedMap && packed.r < DiscardValue) discard;
            if (!EnableAlphaPackedMap && EnableEmissionPackedMap) emission *= packed.r;
            roughness = packed.g * RoughnessIntensity;
            metallic = packed.b * MetallicIntensity;
        }
    }
    vec4 weights = vec4(0.0);
    float total = 1.0;
    float reflectance = diffuse_weight(EmissionIntensity);
    if (NumMaterialLayer > 1) {
        weights = vec4(1.0);
        if (LayerMaskSource == 1) {
            vec2 mask_uv = UVIndexLayerMask < 0 ? uv : vec2(select_uv(UV, UV2, UVIndexLayerMask).x, 1.0 - select_uv(UV, UV2, UVIndexLayerMask).y);
            if (EnableLayerMaskUVTransform) {
                vec2 raw = vec2(mask_uv.x, 1.0 - mask_uv.y) - UVCenterRotationLayerMask.xy;
                float angle = radians(UVCenterRotationLayerMask.z);
                raw = mat2(vec2(cos(angle), sin(angle)), vec2(-sin(angle), cos(angle))) * raw;
                mask_uv = transform_uv(raw + UVCenterRotationLayerMask.xy, UVScaleOffsetLayerMask);
            }
            weights = sample_trinity(LayerMaskMap, mask_uv, 11);
        } else if (LayerMaskSource == 2) {
            weights = COLOR;
        }
#ifdef EYE_MATERIAL
        weights = sample_trinity(LayerMaskMap, uv, 11);
#endif
        if (LayerMaskSwizzle == 1) weights = vec4(weights.r, 1.0, 1.0, 1.0);
        if (LayerMaskSwizzle == 2) weights = vec4(weights.a, 1.0, 1.0, 1.0);
        weights *= vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4);
        if (NumMaterialLayer < 3) weights.g = 0.0;
        if (NumMaterialLayer < 4) weights.b = 0.0;
        if (NumMaterialLayer < 5) weights.a = 0.0;
        total = LayerBaseMaskSource == 1 ? clamp(1.0 - dot(weights, vec4(1.0)), 0.0, 1.0) : 1.0;
#ifdef EYE_MATERIAL
        total = clamp(1.0 - dot(weights, vec4(1.0)), 0.0, 1.0);
#endif
        base.rgb *= total;
        if (!EnableMetallicMap) metallic *= total;
        if (!EnableRoughnessMap) roughness *= total;
        if (!EnableEmissionColorMap) emission *= total;
    }
    base.rgb *= reflectance;
    for (int i = 0; i < 4; i++) {
        float weight = weights[i];
        vec4 tint = i == 0 ? BaseColorLayer1 : (i == 1 ? BaseColorLayer2 : (i == 2 ? BaseColorLayer3 : BaseColorLayer4));
        float index = i == 0 ? BaseColorIndex1 : (i == 1 ? BaseColorIndex2 : (i == 2 ? BaseColorIndex3 : BaseColorIndex4));
        vec3 color = layer_color(tint, index);
        vec2 uv1 = layer_uv(UV, UV2, UVIndexLayer1, UVScaleOffset1);
        vec2 uv2 = layer_uv(UV, UV2, UVIndexLayer2, UVScaleOffset2);
        if (i == 0 && EnableBaseColorMap1) color *= sample_trinity(BaseColorMap1, uv1, 1).rgb;
        else color *= base0;
        float intensity = i == 0 ? EmissionIntensityLayer1 : (i == 1 ? EmissionIntensityLayer2 : (i == 2 ? EmissionIntensityLayer3 : EmissionIntensityLayer4));
        float r = i == 0 ? RoughnessLayer1 : (i == 1 ? RoughnessLayer2 : (i == 2 ? RoughnessLayer3 : RoughnessLayer4));
        float m = i == 0 ? MetallicLayer1 : (i == 1 ? MetallicLayer2 : (i == 2 ? MetallicLayer3 : MetallicLayer4));
        vec3 e = i == 0 ? EmissionColorLayer1.rgb : (i == 1 ? EmissionColorLayer2.rgb : (i == 2 ? EmissionColorLayer3.rgb : EmissionColorLayer4.rgb));
#ifdef BG_LAYER
        if (i == 0 && LayerMaskSource == 2) {
            float layer_alpha = EnableBaseColorMap1 ? sample_trinity(BaseColorMap1, uv1, 1).a : 1.0;
            float height_mask = mix(min(1.0 - base.a, weight), max(layer_alpha, weight), weight);
            weight = smoothstep(LayerMaskLevelMin, max(LayerMaskLevelMin + 0.00001, LayerMaskLevelMax), height_mask);
        }
#endif
        base.rgb = blend_color(base.rgb, color * diffuse_weight(intensity), weight);
        reflectance = mix(reflectance, diffuse_weight(intensity), weight);
        bool has_extra_metal = false;
        bool has_extra_emission = false;
#ifdef BG_LAYER
        has_extra_metal = i == 0 && EnableMetallicMap1;
        has_extra_emission = i == 0 && EnableEmissionColorMap1;
        if (has_extra_metal) metallic = mix(metallic, sample_trinity(MetallicMap1, uv1, 21).r * m, weight);
        if (has_extra_emission) emission = mix(emission, sample_trinity(EmissionColorMap1, uv1, 22).rgb * intensity, weight);
#endif
        if (!EnableMetallicMap && !has_extra_metal) metallic = mix(metallic, m, weight);
        if (i == 0 && EnableRoughnessMap1) roughness = mix(roughness, sample_trinity(RoughnessMap1, uv1, 7).r * r, weight);
        else if (!EnableRoughnessMap) roughness = mix(roughness, r, weight);
        if (!EnableEmissionColorMap && !has_extra_emission) emission = mix(emission, e * intensity, weight);
        if (i == 0 && EnableNormalMap1) {
            vec3 n = unpack_normal(sample_trinity(NormalMap1, uv1, 3), NormalHeight1);
            normal_ts = mix(normal_ts, EnableAOMap1 ? blend_reoriented(n, normal_ts) : n, weight);
        }
        if (i == 1 && EnableNormalMap2) {
            vec3 n = unpack_normal(sample_trinity(NormalMap2, uv2, 4), NormalHeight2);
            normal_ts = mix(normal_ts, EnableAOMap2 ? blend_reoriented(n, normal_ts) : n, weight);
        }
        if (i == 0 && EnableAOMap1) ao = mix(ao, sample_trinity(AOMap1, uv1, 9).r, weight);
        if (i == 1 && EnableAOMap2) ao = mix(ao, sample_trinity(AOMap2, uv2, 10).r, weight);
        if (NumMaterialLayer > 1) total = mix(total, 1.0, weight);
    }
    if (NumMaterialLayer > 1) {
        base.rgb /= max(total, 0.00001);
        if (!EnableMetallicMap) metallic /= max(total, 0.00001);
        if (!EnableRoughnessMap) roughness /= max(total, 0.00001);
        if (!EnableEmissionColorMap) emission /= max(total, 0.00001);
    }
    if (EnableVertexBaseColor) base *= mix(vec4(1.0), COLOR, VertexColorIntensity);
    if (NumMaterialLayer > 5) {
        float weight = sample_trinity(OpacityMap1, layer_uv(UV, UV2, UVIndexLayer5, UVScaleOffset5), 19).r * LayerMaskScale5;
        base.rgb = mix(base.rgb, layer_color(BaseColorLayer5, BaseColorIndex5) * diffuse_weight(EmissionIntensityLayer5), weight);
        metallic = mix(metallic, MetallicLayer5, weight);
        roughness = mix(roughness, RoughnessLayer5, weight);
        reflectance = mix(reflectance, diffuse_weight(EmissionIntensityLayer5), weight);
        emission = mix(emission, EmissionColorLayer5.rgb * EmissionIntensityLayer5, weight);
    }
#ifdef INSIDE_EMISSION_PARALLAX
    vec3 view_ts = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
    float view_z = abs(view_ts.z) < 0.00001 ? 0.00001 : view_ts.z;
    vec3 mapped_normal = normalize(mat3(TANGENT, BINORMAL, NORMAL) * normal_ts);
    float nv = clamp(dot(mapped_normal, VIEW), 0.00001, 1.0);
    float rim = clamp(EmissionRimF0 + (EmissionRimF90 - EmissionRimF0) * pow(1.0 - nv, 5.0), 0.0, 1.0);
    float mono = rim >= EmissionRimThreshold ? 1.0 : 0.0;
    float denominator = mono - EmissionRimThreshold;
    float transition = abs(denominator) > 0.00001 ? (mono - rim) / denominator : 0.0;
    transition = transition > 0.00001 ? pow(transition, max(EmissionRimCurve, 0.00001)) : transition;
    rim = mix(mono, rim, clamp(transition, 0.0, 1.0));
    emission = (1.0 - rim * EmissionRimRate) * emission + EmissionRimColor.rgb * EmissionIntensity * rim * EmissionRimRate;
    vec2 height_uv = layer_uv(UV, UV2, UVIndexInsideEmissionParallaxHeight, UVScaleOffsetInsideEmissionParallaxHeight);
    float height = sample_trinity(ParallaxMap, height_uv, 12).r * InsideEmissionParallaxHeight + InsideEmissionParallaxOffset;
    vec2 inside_uv = layer_uv(UV, UV2, UVIndexInsideEmissionParallaxIntensity, UVScaleOffsetInsideEmissionParallaxIntensity);
    inside_uv = inside_repeat_uv(inside_uv, vec2(view_ts.x, -view_ts.y) / view_z * height);
    float inside_fresnel = max(0.0, InsideEmissionParallaxF0 + (InsideEmissionParallaxF90 - InsideEmissionParallaxF0) * pow(1.0 - nv, 5.0));
    vec3 inside_emission = sample_trinity(InsideEmissionParallaxIntensityMap, inside_uv, 21).rgb * InsideEmissionParallaxIntensity * inside_fresnel;
    if (NumMaterialLayer > 1) inside_emission *= dot(weights, vec4(EnableIEPTexture1, EnableIEPTexture2, EnableIEPTexture3, EnableIEPTexture4));
    emission += inside_emission;
#endif
    if (NumDetailLayer > 0) {
        vec4 detail_mask = sample_trinity(DetailMaskMap, uv, 17);
        vec2 detail_uv = transform_uv(UV, UVScaleOffset4);
        vec3 detail = unpack_normal(sample_trinity(NormalMap1, detail_uv, 3), NormalHeight1);
        normal_ts = mix(normal_ts, blend_reoriented(normal_ts, detail), detail_mask.r);
        roughness = mix(roughness, roughness * sample_trinity(RoughnessMap1, detail_uv, 7).r, detail_mask.r);
        if (NumDetailLayer > 1) {
            detail_uv = transform_uv(UV, UVScaleOffset5);
            detail = unpack_normal(sample_trinity(NormalMap2, detail_uv, 4), NormalHeight2);
            normal_ts = mix(normal_ts, blend_reoriented(normal_ts, detail), detail_mask.g);
            roughness = mix(roughness, roughness * sample_trinity(RoughnessMap2, detail_uv, 18).r, detail_mask.g);
        }
    }
    if (EnableLightMap) emission += base.rgb * sample_trinity(LightMap, vec2(UV2.x, 1.0 - UV2.y), 14).rgb * ao;

#ifdef EYE_MATERIAL
    if (EnableHighlight && !EnableEyeClearCoat) {
        float highlight = sample_trinity(HighlightMaskMap, eye_uv(UV, UVScaleOffset1, 0.0), 23).r;
        emission = mix(emission, EmissionColorLayer5.rgb * EmissionIntensityLayer5, highlight);
        base.rgb *= max(1.0 - EmissionIntensityLayer5 * highlight, 0.0);
    }
    if (EnableOverrideColor) {
        base.rgb = mix(base.rgb, BaseColorLayer5.rgb, LayerMaskScale5);
        emission *= 1.0 - LayerMaskScale5;
    }
    if (RequireEyelidShadowMap || EyelidType == 1 || EyelidType == 3) {
        float shadow = sample_trinity(EyelidShadowMaskMap, eye_uv(UV, UVScaleOffset2, UVRotation2), 24).r;
        base.rgb *= mix(vec3(1.0), BaseColorLayer6.rgb, shadow);
    }
    for (int lid_index = 0; lid_index < 2; lid_index++) {
        if ((lid_index == 0 && (EyelidType == 1 || EyelidType == 3)) || (lid_index == 1 && (EyelidType == 2 || EyelidType == 3))) {
            vec4 lid = lid_index == 0 ? sample_trinity(UpperEyelidColorMap, eye_uv(UV, UVScaleOffset3, UVRotation3), 21) : sample_trinity(LowerEyelidColorMap, eye_uv(UV, UVScaleOffset4, UVRotation4), 22);
            vec3 color = lid_index == 0 ? BaseColorLayer7.rgb : BaseColorLayer8.rgb;
            base.rgb = mix(base.rgb, lid.rgb * color, lid.a);
            metallic = mix(metallic, lid_index == 0 ? MetallicLayer7 : MetallicLayer8, lid.a);
            roughness = mix(roughness, lid_index == 0 ? RoughnessLayer7 : RoughnessLayer8, lid.a);
            emission *= 1.0 - lid.a;
            if (EnableEyeClearCoat) normal_ts = mix(normal_ts, vec3(0.0, 0.0, 1.0), lid.a);
        }
    }
#endif
    ALBEDO = base.rgb;
    NORMAL_MAP = normalize(normal_ts) * 0.5 + 0.5;
    METALLIC = clamp(metallic, 0.0, 1.0);
    ROUGHNESS = clamp(roughness, 0.0, 1.0);
    SPECULAR = clamp(Reflectance * reflectance / 0.08, 0.0, 1.0);

    EMISSION = emission;
    AO = ao;
#ifdef EYE_MATERIAL
    if (EnableEyeClearCoat) SPECULAR = 0.0;
#endif
#ifdef TRANSPARENT_MATERIAL
    float opacity = base.a;
    if (RefractionMode == 1) {
        float nv = clamp(dot(normalize(mat3(TANGENT, BINORMAL, NORMAL) * normal_ts), VIEW) - FresnelAngleBias, 0.0, 1.0);
        opacity *= FresnelAlphaMin + (FresnelAlphaMax - FresnelAlphaMin) * pow(1.0 - nv, 5.0);
    }
    ALPHA = opacity;
#endif
}
