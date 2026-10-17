shader_type spatial;
render_mode unshaded;

uniform sampler2D AlbedoColorMap : source_color, hint_default_white, repeat_enable;
uniform sampler2D DitherPatternMap : hint_default_black, repeat_enable;
uniform sampler2D OutlineMaskMap : hint_default_black, repeat_enable;

uniform int sampler_repeat_u[3];
uniform int sampler_repeat_v[3];
uniform vec4 sampler_border_color[3];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"

uniform float ScanlineSpeed = 0.0;
uniform float DarkShadingContrast = 0.0;
uniform float LightShadingContrast = 0.0;
uniform float AlbedoIntensity = 0.0;
uniform float LightPartIntensity = 0.0;
uniform float DarkPartIntensity = 0.0;
uniform float OutlineIntensity = 0.0;
uniform float DitherPatternTiling = 0.0;
uniform float ScanlineIntensity = 0.0;
uniform float TransitionLight = 0.0;

uniform vec4 OutlineColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 MaterialLightDirection = vec3(0.0, 1.0, 0.0);
varying vec3 procedural_world;

vec3 SG_sgglitch(vec2 _uv, vec2 _glitch_speed, float _time, float _glitch_tex, float _glitch_alpha)
{
  vec2 glitch = ceil(sin(sin(_glitch_speed * vec2(_time) + _uv.yy) * vec2(20.0, 25.0)));
  float glitch_amount = (glitch.x + glitch.y) * _glitch_tex;
  vec2 glitched_uv = mix(_uv, vec2(_uv.x + glitch_amount, _uv.y), vec2(_glitch_alpha));
  return vec3(glitched_uv, glitch_amount);
}

float SG_SGCheapContrast(float _In, float _Contrast)
{
  return clamp(mix(0.0 - _Contrast, _Contrast + 1.0, _In), 0.0, 1.0);
}

vec2 TilingAndOffsetUV(vec2 _uv, vec2 _tiling, vec2 _offset)
{
  vec2 uv = _uv * _tiling + _offset;
  return vec2(uv.x, 1.0 - uv.y);
}

vec3 ShiftHueDegrees(vec3 In, float Offset)
{

  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 P = mix(vec4(In.bg, K.wz), vec4(In.gb, K.xy), step(In.b, In.g));
  vec4 Q = mix(vec4(P.xyw, In.r), vec4(In.r, P.yzx), step(P.x, In.r));
  float D = Q.x - min(Q.w, Q.y);
  float epsilon = 0.0000000001;
  float V = (D == 0.0) ? Q.x : (Q.x + epsilon);
  vec3 hsv = vec3(abs(Q.z + (Q.w - Q.y)/(6.0 * D + epsilon)), D / (Q.x + epsilon), V);

  float hue = hsv.x + Offset / 360.0;
  hsv.x = (hue < 0.0)
          ? hue + 1.0
          : (hue > 1.0)
              ? hue - 1.0
              : hue;


  vec4 K2 = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 P2 = abs(fract(hsv.xxx + K2.xyz) * 6.0 - K2.www);
  return hsv.z * mix(K2.xxx, clamp(P2 - K2.xxx, 0.0, 1.0), hsv.y);
}

float FresnelEffect(vec3 Normal, vec3 ViewDir, float Power) {
  return pow((1.0 - clamp(dot(normalize(Normal), normalize(ViewDir)), 0.0, 1.0)), Power);
}

void vertex() { 
  procedural_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz; 
}

void fragment() {
  vec2 base_uv = vec2(UV.x, 1.0 - UV.y);
  vec4 alb_sample = sample_trinity(AlbedoColorMap, base_uv, 0);
  float scanline_time = TIME * ScanlineSpeed;
  vec2 tiled_pos_ws = TilingAndOffsetUV(procedural_world.xy, vec2(1.0, -1.0), vec2(scanline_time));
  float glitch_msk1 = SG_sgglitch(tiled_pos_ws, vec2(0.5, 0.75), scanline_time, 0.5, 0.01).z;
  glitch_msk1 = clamp(glitch_msk1 * ScanlineIntensity, 0.0, OutlineIntensity);
  glitch_msk1 = mix(glitch_msk1, OutlineIntensity, TransitionLight);
  vec3 glitched_base_color = mix(alb_sample, OutlineColor, glitch_msk1).rgb;

  vec2 dither_pattern_uv = TilingAndOffsetUV(VERTEX.xy, vec2(DitherPatternTiling), vec2(0.0, TIME * -4.0));
  vec4 dither_pattern_sample = sample_trinity(DitherPatternMap, dither_pattern_uv, 1);

  vec3 dithered_color = mix(glitched_base_color, ShiftHueDegrees(glitched_base_color, 20.0), dither_pattern_sample.ggg);
  dithered_color = mix(dithered_color, ShiftHueDegrees(glitched_base_color, -20.0), dither_pattern_sample.bbb);

  float dark_factor = FresnelEffect(NORMAL, normalize(MaterialLightDirection), 2.0);
  dark_factor = SG_SGCheapContrast(dark_factor, DarkShadingContrast) * dither_pattern_sample.r;
  vec3 dark_color = mix(dithered_color, dithered_color * DarkPartIntensity, vec3(dark_factor));

  float light_factor = FresnelEffect(NORMAL, normalize(MaterialLightDirection), 0.25);
  light_factor = SG_SGCheapContrast(light_factor, LightShadingContrast);
  light_factor = (1.0 - light_factor) * (1.0 - dither_pattern_sample.r);
  vec3 hologram_color = mix(dark_color, dithered_color * LightPartIntensity, vec3(light_factor) * AlbedoIntensity);

  vec4 emm_sample = sample_trinity(OutlineMaskMap, base_uv, 2);
  vec3 emission= mix(hologram_color, OutlineColor.rgb * OutlineIntensity, emm_sample.rrr);

  ALBEDO = emission;
}
