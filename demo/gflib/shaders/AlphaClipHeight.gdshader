shader_type spatial;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, repeat_enable;

uniform int sampler_repeat_u[3];
uniform int sampler_repeat_v[3];
uniform vec4 sampler_border_color[3];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_light.gdshaderinc"

uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform float DiscardValue = 0.5;
uniform float HeightContrast;
uniform float VertexColorContrast = 1.0;
uniform float RoughnessIntensity = 1.0;
uniform float NormalIntensity = 1.0;
uniform float Metallic;


void fragment() {
    vec2 uv = material_uv(UV, UVScaleOffset);
    vec4 roughness = sample_trinity(RoughnessMap, uv, 2);
    float vertex_mask = 1.0 - pow(clamp(COLOR.r, 0.0, 1.0), max(VertexColorContrast, 0.00001));
    float overlay = vertex_mask < 0.5 ? 2.0 * vertex_mask * roughness.g : 1.0 - 2.0 * (1.0 - vertex_mask) * (1.0 - roughness.g);
    float height_mask = mix(-HeightContrast, 1.0 + HeightContrast, overlay);
    if (height_mask < DiscardValue) discard;
    ALBEDO = sample_trinity(BaseColorMap, uv, 0).rgb;
    NORMAL_MAP = material_normal(sample_trinity(NormalMap, uv, 1), NormalIntensity) * 0.5 + 0.5;
    ROUGHNESS = roughness.r * RoughnessIntensity;
    METALLIC = Metallic;
    TRINITY_LIGHT_FRAGMENT;
}
