shader_type spatial;
render_mode unshaded;

uniform sampler2D ScanMap : hint_default_white, repeat_enable;
uniform sampler2D NoiseMap1 : hint_default_white, repeat_enable;
uniform sampler2D AmbientSphereMap : hint_default_white, repeat_enable;
uniform sampler2D DotPatternMap : hint_default_white, repeat_enable;
uniform sampler2D NoiseMap2 : hint_default_white, repeat_enable;
uniform sampler2D GradientMap2 : hint_default_white, repeat_enable;

uniform int sampler_repeat_u[6];
uniform int sampler_repeat_v[6];
uniform vec4 sampler_border_color[6];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"

uniform float BaseColorScale = 1.0;
uniform float DotColorScale = 1.0;
uniform float DotAlphaStrength = 1.0;
uniform float LineColorScale = 1.0;
uniform float LineAlphaStrength = 1.0;
uniform float EdgeColorScale = 1.0;

uniform vec4 BaseLightColor1 = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 BaseLightColor2 = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 BaseDarkColor1 = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 BaseDarkColor2 = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 DotLightColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 DotDarkColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 LineLightColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 LineDarkColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 EdgeLightColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 EdgeDarkColor = vec4(1.0, 1.0, 1.0, 1.0);

vec2 ScrollUV(vec2 uv, vec2 speed) { return uv + TIME * speed; }
float CalcFresnelRate(vec3 eye, vec3 normal, float low, float high) { return 1.0 - smoothstep(high, low, abs(dot(normal, eye))); }

void fragment() {
  vec2 main_uv = vec2(UV.x, 1.0 - UV.y);

  float base_factor = sample_trinity(ScanMap, ScrollUV(vec2(0.0, main_uv.y * 9.0), vec2(0, 0.11)), 0).r;
  float base_mask1 = sample_trinity(NoiseMap1, ScrollUV(vec2(0.0, main_uv.y * 1.5), vec2(0, 0.3)), 1).r;
  float base_mask2 = sample_trinity(AmbientSphereMap, UV2, 2).r;
  float dot_factor = sample_trinity(DotPatternMap, main_uv * 65.0, 3).r;
  float dot_mask1 = sample_trinity(NoiseMap2, ScrollUV(main_uv * 2.0, vec2(-0.12, 0.12)), 4).r;
  float dot_mask2 = sample_trinity(GradientMap2, vec2(main_uv.x, 1.0 - main_uv.y) * vec2(1.0, 1.5), 5).r;
  float line_factor = sample_trinity(DotPatternMap, main_uv * vec2(65.0, 0.0) + vec2(0, 0.5), 3).r;
  float line_mask1 = sample_trinity(GradientMap2, main_uv + vec2(0, -0.22), 5).r;
  float line_mask2 = sample_trinity(NoiseMap2, ScrollUV(main_uv * vec2(1.5, 0), vec2(0.0, 0.09)), 4).r;
  float edge_factor = sample_trinity(GradientMap2, vec2(main_uv.x, 1.0 - main_uv.y) + vec2(0, 0.48), 5).r;

  base_factor *= mix(0.9, 0.5, smoothstep(0.0, 2000.0, dot(VERTEX, VERTEX)));
  vec3 base_dark_color;
  vec3 base_light_color;
  float fract_t = fract(TIME * 2.0);
  if (fract_t < 0.25) {
    float lerp_value = fract_t / 0.25;
    base_dark_color = mix(BaseDarkColor1.rgb, BaseDarkColor2.rgb, lerp_value);
    base_light_color = mix(BaseLightColor1.rgb, BaseLightColor2.rgb, lerp_value);
  } else {
    float lerp_value = (fract_t - 0.25) / 0.75;
    base_dark_color = mix(BaseDarkColor2.rgb, BaseDarkColor1.rgb, lerp_value);
    base_light_color = mix(BaseLightColor2.rgb, BaseLightColor1.rgb, lerp_value);
  }
  base_mask1 *= base_mask1;
  base_mask2 *= base_mask2;
  base_factor = base_factor * base_mask1 * base_mask2;
  vec3 base_color = mix(base_dark_color, base_light_color, base_factor) * BaseColorScale;

  float dot_alpha = dot_factor;
  dot_alpha *= dot_mask1;
  dot_mask2 = 1.0 - dot_mask2;
  dot_factor += dot_mask2;
  dot_alpha *= dot_mask2;
  dot_alpha *= 1.2 * DotAlphaStrength;
  dot_alpha *= (1.0 + sin(TIME * 12.5) * 0.2);
  vec3 dot_color = mix(DotDarkColor.rgb, DotLightColor.rgb, dot_factor) * DotColorScale;

  float line_alpha = 1.0;
  line_mask1 *= line_mask1;
  line_alpha *= line_mask1 * LineAlphaStrength;
  line_alpha *= line_mask2;
  line_factor = line_factor * line_mask1 * line_mask2;
  vec3 line_color = mix(LineDarkColor.rgb, LineLightColor.rgb, line_factor) * LineColorScale;

  edge_factor *= edge_factor;
  float edge_alpha = edge_factor;
  edge_alpha *= CalcFresnelRate(normalize((INV_VIEW_MATRIX * vec4(VIEW, 0.0)).xyz), normalize((INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz), 0.35, 0.0);
  vec3 edge_color = mix(EdgeDarkColor.rgb, EdgeLightColor.rgb, edge_factor) * EdgeColorScale;

  vec3 emission = mix(base_color, dot_color, clamp(dot_alpha, 0.0, 1.0));
  emission = mix(emission, line_color, clamp(line_alpha, 0.0, 1.0));
  emission += edge_color * edge_alpha;

  ALBEDO = emission;
}
