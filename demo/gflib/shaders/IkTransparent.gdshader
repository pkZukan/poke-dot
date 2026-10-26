shader_type spatial;
render_mode blend_premul_alpha, cull_disabled;

uniform sampler2D BaseColorMap : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D NormalMap : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D MetallicMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;

uniform int sampler_repeat_u[4];
uniform int sampler_repeat_v[4];
uniform vec4 sampler_border_color[4];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_blend.gdshaderinc"
#include "trinity_light.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableNormalMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableAlphaTest;
uniform bool EnableBasecolorParameter;
uniform bool EnableBasecolorEmissive;

uniform vec4 BaseColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);

uniform float Transparency = 1.0;
uniform float DiscardValue = 0.5;
uniform float NormalHeight = 1.0;
uniform float Metallic;
uniform float Roughness = 0.5;
uniform float EmissionIntensity;

void fragment() {
    vec2 uv = material_uv(UV, UVScaleOffset);
    vec4 color = EnableBaseColorMap ? sample_trinity(BaseColorMap, uv, 0) : vec4(1.0);
    if (EnableBasecolorParameter) color *= BaseColor;
    float alpha = clamp(color.a, 1.0 - Transparency, 1.0);
    if (EnableAlphaTest && alpha < DiscardValue) discard;
    if (EnableNormalMap) NORMAL_MAP = material_normal(sample_trinity(NormalMap, uv, 1), NormalHeight) * 0.5 + 0.5;
    
    ALBEDO = color.rgb;
    METALLIC = EnableMetallicMap ? sample_trinity(MetallicMap, uv, 2).r : Metallic;
    ROUGHNESS = EnableRoughnessMap ? sample_trinity(RoughnessMap, uv, 3).r : Roughness;
    EMISSION = EnableBasecolorEmissive ? color.rgb * color.a * EmissionIntensity : vec3(0.0);
    vec2 blend_state = trinity_blend(alpha, 1.0);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
    TRINITY_LIGHT_FRAGMENT;
}
