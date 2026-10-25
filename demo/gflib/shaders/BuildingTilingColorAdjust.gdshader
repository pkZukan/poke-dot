shader_type spatial;

uniform sampler2D PackedMap : hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap1 : hint_normal, repeat_enable;
uniform sampler2D GrayAlbedoMap : hint_default_white, repeat_enable;
uniform sampler2D EmissionMaskMap : hint_default_white, repeat_enable;

uniform int sampler_repeat_u[6];
uniform int sampler_repeat_v[6];
uniform vec4 sampler_border_color[6];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_light.gdshaderinc"

uniform bool EnableAlphaTest;
uniform bool EnableTilingBlend;
uniform bool EnableGrayAlbedoMap;
uniform bool EnableVertexColor;
uniform bool EnableVertexBaseColor;
uniform bool EnableEmissionMaskMap;
uniform float Tiling = 1.0;
uniform float DiscardValue = 0.5;
uniform float NormalIntensity = 1.0;
uniform float RoughnessIntensity = 1.0;
uniform float MetallicIntensity = 1.0;
uniform float ColorChannelR = 1.0;
uniform float ColorChannelG;
uniform float ColorChannelB;
uniform float AOIntensity = 1.0;
uniform float VertexColorIntensity = 1.0;
uniform float EmissionIntensity;
uniform vec4 EmissionColor = vec4(1.0);
uniform vec4 LowColor = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 HiColor = vec4(1.0);


void fragment() {
    vec2 tiled = material_uv(EnableGrayAlbedoMap || !EnableTilingBlend ? UV : UV2, vec4(Tiling, Tiling, 0.0, 0.0));
    vec4 packed = sample_trinity(PackedMap, tiled, 0);
    if (EnableAlphaTest && packed.a < DiscardValue) discard;
    vec3 n = material_normal_strength(material_normal(sample_trinity(NormalMap, tiled, 1), 1.0), NormalIntensity);
    float factor = packed.r;
    vec3 channels = vec3(ColorChannelR, ColorChannelG, ColorChannelB);
    if (EnableTilingBlend && !EnableVertexColor) {
        vec2 object_uv = vec2(UV.x, 1.0 - UV.y);
        vec3 object_color = mix(vec3(0.5), sample_trinity(BaseColorMap1, object_uv, 2).rgb, channels);
        vec3 object_normal = material_normal(sample_trinity(NormalMap1, object_uv, 3), 1.0);
        n = normalize(vec3(n.xy + object_normal.xy, n.z * object_normal.z));
        factor = material_overlay(material_overlay(material_overlay(factor, object_color.r), object_color.b), object_color.g);
    }
    vec3 color = mix(LowColor.rgb, HiColor.rgb, factor);
    if (EnableGrayAlbedoMap) {
        vec3 tiling_color = sample_trinity(BaseColorMap1, tiled, 2).rgb;
        float occlusion = dot(sample_trinity(GrayAlbedoMap, vec2(UV2.x, 1.0 - UV2.y), 4).rgb, channels);
        vec3 base = mix(tiling_color, tiling_color * occlusion, AOIntensity);
        color = mix(2.0 * base * occlusion, vec3(1.0) - 2.0 * (vec3(1.0) - base) * (1.0 - occlusion), greaterThan(base, vec3(0.5)));
    }
    if (EnableVertexColor || EnableVertexBaseColor) 
        color *= mix(vec3(1.0), COLOR.rgb, VertexColorIntensity);

    ALBEDO = color;
    NORMAL_MAP = normalize(n) * 0.5 + 0.5;
    ROUGHNESS = packed.g * RoughnessIntensity;
    METALLIC = packed.b * MetallicIntensity;
    EMISSION = EnableEmissionMaskMap ? sample_trinity(EmissionMaskMap, vec2(UV.x, 1.0 - UV.y), 5).rgb * EmissionColor.rgb * EmissionIntensity : vec3(0.0);
    TRINITY_LIGHT_FRAGMENT;
}
