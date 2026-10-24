shader_type spatial;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap : hint_normal, repeat_enable;
uniform sampler2D PackedMap : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap : hint_default_white, repeat_enable;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap1 : hint_normal, repeat_enable;
uniform sampler2D PackedMap1 : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap1 : hint_default_white, repeat_enable;
uniform sampler2D BaseColorMap2 : source_color, hint_default_white, repeat_enable;
uniform sampler2D NormalMap2 : hint_normal, repeat_enable;
uniform sampler2D PackedMap2 : hint_default_white, repeat_enable;
uniform sampler2D RoughnessMap2 : hint_default_white, repeat_enable;

uniform int sampler_repeat_u[12];
uniform int sampler_repeat_v[12];
uniform vec4 sampler_border_color[12];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"

uniform int NumMaterialLayer = 1;

uniform bool EnablePackedMap = true;
uniform bool EnableRoughnessMap;
uniform bool EnableMetallicValue;
uniform bool EnableVertexBaseColor;
uniform bool EnableVertexSelectedChannel;
uniform bool EnableWorldXzUv;

uniform float VertexColorIntensity = 1.0;
uniform float UVRotation;
uniform float LayerMaskScale1 = 1.0;

uniform int VertexColorChannel;
uniform int VertexColorChannel1 = 1;

uniform float Contrast = 1.0;
uniform float Contrast1 = 1.0;
uniform float Influence;
uniform float Influence1;

varying vec3 ground_world_position;

uniform float Tiling = 1.0;
uniform float NormalIntensity = 1.0;
uniform float RoughnessIntensity = 1.0;
uniform float MetallicIntensity = 0.0;
uniform float Tiling1 = 1.0;
uniform float NormalIntensity1 = 1.0;
uniform float RoughnessIntensity1 = 1.0;
uniform float MetallicIntensity1 = 0.0;
uniform float Tiling2 = 1.0;
uniform float NormalIntensity2 = 1.0;
uniform float RoughnessIntensity2 = 1.0;
uniform float MetallicIntensity2 = 0.0;


void vertex() { ground_world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz; }
void fragment() {
    vec2 source_uv = UV;
    if (EnableWorldXzUv) {
        // UVRotation is in turns.
        float c = cos(UVRotation * TAU), s = sin(UVRotation * TAU);
        source_uv = vec2(c * ground_world_position.x - s * ground_world_position.z, s * ground_world_position.x + c * ground_world_position.z);
    }
    vec3 colors[3]; vec3 normals[3]; vec3 packed[3]; float metals[3];
    vec2 uv0 = material_uv(source_uv, vec4(Tiling, Tiling, 0.0, 0.0));
    colors[0] = sample_trinity(BaseColorMap, uv0, 0).rgb;
    normals[0] = material_normal_strength(material_normal(sample_trinity(NormalMap, uv0, 1), 1.0), NormalIntensity);
    packed[0] = EnablePackedMap ? sample_trinity(PackedMap, uv0, 2).rgb : vec3(1.0, 1.0, EnableRoughnessMap ? sample_trinity(RoughnessMap, uv0, 3).r : 0.0);
    packed[0].g *= RoughnessIntensity;
    metals[0] = EnableMetallicValue ? MetallicIntensity : 0.0;
    vec2 uv1 = material_uv(source_uv, vec4(Tiling1, Tiling1, 0.0, 0.0));
    colors[1] = sample_trinity(BaseColorMap1, uv1, 4).rgb;
    normals[1] = material_normal_strength(material_normal(sample_trinity(NormalMap1, uv1, 5), 1.0), NormalIntensity1);
    packed[1] = EnablePackedMap ? sample_trinity(PackedMap1, uv1, 6).rgb : vec3(1.0, 1.0, EnableRoughnessMap ? sample_trinity(RoughnessMap1, uv0, 7).r : 0.0);
    packed[1].g *= RoughnessIntensity1;
    metals[1] = EnableMetallicValue ? MetallicIntensity1 : 0.0;
    vec2 uv2 = material_uv(source_uv, vec4(Tiling2, Tiling2, 0.0, 0.0));
    colors[2] = sample_trinity(BaseColorMap2, uv2, 8).rgb;
    normals[2] = material_normal_strength(material_normal(sample_trinity(NormalMap2, uv2, 9), 1.0), NormalIntensity2);
    packed[2] = EnablePackedMap ? sample_trinity(PackedMap2, uv2, 10).rgb : vec3(1.0, 1.0, EnableRoughnessMap ? sample_trinity(RoughnessMap2, uv0, 11).r : 0.0);
    packed[2].g *= RoughnessIntensity2;
    metals[2] = EnableMetallicValue ? MetallicIntensity2 : 0.0;
    vec3 color = colors[0], normal_ts = normals[0], orh = packed[0];
    float metal = metals[0];
    if (NumMaterialLayer > 2) {
        float phase = pow(clamp(COLOR[clamp(VertexColorChannel, 0, 3)], 0.0, 1.0), max(Contrast, 0.00001));
        float weight = material_contrast(clamp(phase * 2.0 + packed[0].b - 1.0, 0.0, 1.0), Influence);
        color = mix(colors[2], color, weight); normal_ts = mix(normals[2], normal_ts, weight); orh = mix(packed[2], orh, weight);
        metal = mix(metals[2], metal, phase);
    }
    if (NumMaterialLayer > 1) {
        int channel = EnableVertexSelectedChannel ? VertexColorChannel : VertexColorChannel1;
        float phase = clamp(pow(clamp(COLOR[clamp(channel, 0, 3)], 0.0, 1.0), max(Contrast1, 0.00001)), 0.0, LayerMaskScale1);
        float weight = material_contrast(clamp(phase * 2.0 + packed[1].b - 1.0, 0.0, 1.0), Influence1);
        color = mix(color, colors[1], weight); normal_ts = mix(normal_ts, normals[1], weight); orh = mix(orh, packed[1], weight);
        metal = mix(metal, metals[1], phase);
    }
    ALBEDO = color;
    if (EnableVertexBaseColor) ALBEDO *= mix(vec3(1.0), COLOR.rgb, VertexColorIntensity);
    NORMAL_MAP = normalize(normal_ts) * 0.5 + 0.5;
    AO = orh.r; ROUGHNESS = orh.g; METALLIC = metal;

}
