shader_type spatial;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D PackedMap : hint_default_white, repeat_enable;
uniform sampler2D WindowGradientMap : source_color, hint_default_white, repeat_enable;

uniform int sampler_repeat_u[4];
uniform int sampler_repeat_v[4];
uniform vec4 sampler_border_color[4];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"

uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffsetWindow = vec4(1.0, 1.0, 0.0, 0.0);

uniform float NormalHeight = 1.0;
uniform float WindowSeed;
uniform float WindowTurnOn;
uniform float WindowSaturation;
uniform float WindowIntensity;

uniform bool EnableVertexBaseColor;

uniform float VertexColorIntensity = 1.0;

void fragment() {
    vec2 uv = material_uv(UV, UVScaleOffset);
    vec3 packed = sample_trinity(PackedMap, uv, 2).rgb;
    vec2 cell = floor(-material_uv(UV2, UVScaleOffsetWindow));
    float wave = sin(dot(cell, vec2(7.3322, 31.22566)));
    float x = fract(wave * WindowSeed);
    float y = fract(wave * WindowSeed * 0.1376);
    float z = fract(sin(dot(cell, vec2(12.62, 44.7))) * WindowSeed * 0.5);
    float coordinate = fract(WindowSeed * wave * wave) * x;
    vec3 window = sample_trinity(WindowGradientMap, vec2(coordinate, 0.5), 3).rgb;
    float turn_on = 0.5 - WindowTurnOn;
    float modifier = round(clamp(x * y - turn_on, 0.0, 1.0));
    float screen_factor = round(x - turn_on);
    modifier = clamp(screen_factor, modifier, 1.0);
    window *= modifier * clamp(screen_factor, 0.15, 1.0);
    window = material_saturation(window, WindowSaturation) * x * z * WindowIntensity * packed.r;
    ALBEDO = sample_trinity(BaseColorMap, uv, 0).rgb;
    if (EnableVertexBaseColor) ALBEDO *= mix(vec3(1.0), COLOR.rgb, VertexColorIntensity);
    
    NORMAL_MAP = material_normal(sample_trinity(NormalMap, uv, 1), NormalHeight) * 0.5 + 0.5;
    METALLIC = packed.b;
    ROUGHNESS = packed.g;
    AO = 1.0;
    EMISSION = window;
}
