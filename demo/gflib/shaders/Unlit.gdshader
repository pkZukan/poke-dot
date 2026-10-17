shader_type spatial;
render_mode unshaded, cull_back, depth_draw_opaque;

// Retail NVN formulas and variant choices: docs/shaders/retail.md.
uniform sampler2D BaseColorMap : source_color, repeat_enable;
uniform sampler2D LayerMaskMap : hint_default_white, repeat_enable;
uniform sampler2D DisplacementMap : repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, repeat_enable;
uniform sampler2D OpacityMap : hint_default_white, repeat_enable;
uniform sampler2D OpacityMap1 : hint_default_black, repeat_enable;
uniform int sampler_repeat_u[6];
uniform int sampler_repeat_v[6];
uniform vec4 sampler_border_color[6];
#include "trinity_sampler.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableBaseColorMap1;
uniform bool EnableOpacityMap;
uniform bool EnableOpacityMap1;
uniform bool EnableAlphaTest;
uniform bool EnableDisplacementMap;
uniform bool EnableFlipBookUV;
uniform bool EnableBaseColorMapSaturation;
uniform int NumMaterialLayer = 1;
uniform int NumRequiredUV = 1;
uniform int LayerMaskSource = 1; // Retail: Const=0, Texture=1.
uniform int LayerBaseMaskSource = 1; // One=0, OneMinusLayerMaskSum=1.
uniform float DisplacementHeight;
uniform float DiscardValue;
uniform float LayerMaskScale1 = 1.0;
uniform float LayerMaskScale2 = 1.0;
uniform float LayerMaskScale3 = 1.0;
uniform float LayerMaskScale4 = 1.0;
uniform float LayerMaskScale5 = 1.0;
uniform float EmissionIntensity;
uniform float BaseColorMapSaturation = 1.0;
uniform float FlipBookWidth = 1.0;
uniform float FlipBookHeight = 1.0;
uniform float FlipBookFrame;
uniform float FlipBookInvertU;
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 BaseColorLayer1 = vec4(1.0);
uniform vec4 BaseColorLayer2 = vec4(1.0);
uniform vec4 BaseColorLayer3 = vec4(1.0);
uniform vec4 BaseColorLayer4 = vec4(1.0);
uniform vec4 BaseColorLayer5 = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset3 = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset5 = vec4(1.0, 1.0, 0.0, 0.0);
uniform int UVIndexLayer1;
uniform int UVIndexLayer3;
uniform int UVIndexLayer5;
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableJewel;
uniform int EnableGlobalWeather;
uniform int EnableDirt;

vec2 transform_uv(vec2 uv, vec4 st) {
    vec2 result = (uv - st.zw) * st.xy;
    return vec2(result.x, 1.0 - result.y);
}
vec2 select_uv(vec2 uv0, vec2 uv1, int index) {
    return index != 0 && NumRequiredUV > 1 ? uv1 : uv0;
}
vec4 saturate_color(vec4 color) {
    if (EnableBaseColorMapSaturation) {
        float luma = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        color.rgb = mix(vec3(luma), color.rgb, BaseColorMapSaturation);
    }
    return color;
}
void vertex() {
    if (EnableDisplacementMap) {
        vec2 uv = transform_uv(select_uv(UV, UV2, UVIndexLayer3), UVScaleOffset3);
        float phase = sample_trinity(DisplacementMap, uv, 2).r;
        VERTEX += NORMAL * (sin(phase) * COLOR.r * DisplacementHeight);
    }
}
void fragment() {
    vec2 uv = transform_uv(UV, UVScaleOffset);
    if (EnableFlipBookUV) {
        vec2 count = max(vec2(FlipBookWidth, FlipBookHeight), vec2(1.0));
        float frame = mod(FlipBookFrame, count.x * count.y);
        float row = floor(frame / count.x);
        vec2 tile = vec2(frame - row * count.x, count.y - 1.0 - row);
        vec2 tile_int = floor(tile);
        vec2 raw = UV * count;
        raw.y -= count.y - 1.0;
        raw = (raw - UVScaleOffset.zw + tile) / count;
        raw.x = mix(raw.x, (2.0 * tile_int.x + 1.0) / count.x - raw.x, FlipBookInvertU);
        raw = clamp(raw, tile_int / count, (tile_int + vec2(1.0)) / count);
        uv = vec2(raw.x, 1.0 - raw.y);
    }
    vec4 color = BaseColor;
    if (EnableBaseColorMap) color *= saturate_color(sample_trinity(BaseColorMap, uv, 0));
    if (EnableOpacityMap) color.a *= sample_trinity(OpacityMap, uv, 4).r * clamp(EmissionIntensity, 0.0, 1.0);
    if (NumMaterialLayer > 1) {
        vec4 weights = LayerMaskSource == 1 ? sample_trinity(LayerMaskMap, uv, 1) : vec4(1.0);
        weights *= vec4(LayerMaskScale1, LayerMaskScale2, LayerMaskScale3, LayerMaskScale4);
        if (NumMaterialLayer < 3) weights.g = 0.0;
        if (NumMaterialLayer < 4) weights.b = 0.0;
        if (NumMaterialLayer < 5) weights.a = 0.0;
        vec3 base = color.rgb;
        float total = 1.0;
        if (EnableOpacityMap1) {
            float opacity = 1.0 - sample_trinity(OpacityMap1, transform_uv(UV, UVScaleOffset1), 5).r;
            color.a *= mix(1.0, opacity, weights.r);
        } else {
            total = LayerBaseMaskSource == 1 ? clamp(1.0 - dot(weights, vec4(1.0)), 0.0, 1.0) : 1.0;
            color.rgb *= total;
            total = mix(total, 1.0, weights.r);
            if (EnableBaseColorMap1) {
                vec4 layer = BaseColorLayer1 * saturate_color(sample_trinity(BaseColorMap1, transform_uv(select_uv(UV, UV2, UVIndexLayer1), UVScaleOffset1), 3));
                color = mix(color, layer, weights.r);
            } else {
                color.rgb = mix(color.rgb, base * BaseColorLayer1.rgb, weights.r);
            }
        }
        for (int i = 1; i < 4; i++) {
            vec3 tint = i == 1 ? BaseColorLayer2.rgb : (i == 2 ? BaseColorLayer3.rgb : BaseColorLayer4.rgb);
            color.rgb = mix(color.rgb, base * tint, weights[i]);
            total = mix(total, 1.0, weights[i]);
        }
        if (NumMaterialLayer > 5) {
            float weight = sample_trinity(OpacityMap, transform_uv(select_uv(UV, UV2, UVIndexLayer5), UVScaleOffset5), 4).r * LayerMaskScale5;
            color.rgb = mix(color.rgb, base * BaseColorLayer5.rgb, weight);
        }
        color.rgb /= max(total, 0.00001);
    }
    if (EnableAlphaTest && color.a < DiscardValue) discard;
    ALBEDO = color.rgb * EmissionIntensity;
}
