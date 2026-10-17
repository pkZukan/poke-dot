shader_type spatial;
render_mode unshaded, cull_disabled;

uniform sampler2D MoonColorMap : hint_default_black, repeat_enable;
uniform sampler2D MoonNormalMap : hint_normal, repeat_enable;
uniform sampler2D TopCloudsShapeMap : hint_default_black, repeat_enable;
uniform sampler2D TopCloudsBreakupMap : hint_default_black, repeat_enable;
uniform sampler2D NightStarsMap : hint_default_black, repeat_enable;
uniform sampler2D NightStarsMaskMap : hint_default_black, repeat_enable;

uniform int sampler_repeat_u[6];
uniform int sampler_repeat_v[6];
uniform vec4 sampler_border_color[6];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"

uniform vec3 SkySunDirection = vec3(0.0, -1.0, 0.0);
uniform vec3 SkyMoonDirection = vec3(0.0, 1.0, 0.0);
varying vec3 sky_object_position;

uniform vec4 ZenithColor = vec4(0.1, 0.3, 0.6, 1.0);
uniform vec4 HorizonColor = vec4(0.5, 0.6, 0.8, 1.0);
uniform vec4 CloudColorRim = vec4(1.0);
uniform vec4 CloudColor = vec4(1.0);
uniform vec4 SunTintColor = vec4(1.0);
uniform vec4 FogColor = vec4(1.0);

uniform float MoonScale = 1.0;
uniform float MoonPhase = 0.0;
uniform float MoonShadingRotation = 0.0;
uniform float TopCloudSpeed = 0.0;
uniform float TopPlaneHeight = 1.0;
uniform float TopPlaneHorizon = 0.01;
uniform float TopCloudCoverage = 0.5;
uniform float TopBreakupSpeed = 0.0;
uniform float TopPlaneTiling = 1.0;
uniform float CloudsFadeOut = 0.0;
uniform float MoonIntensity = 0.0;
uniform float StarsTiling = 1.0;
uniform float StarsHorizonLift = 0.0;
uniform float StarsMaskTiling = 1.0;
uniform float StarsMaskSpeed = 0.0;
uniform float HorizonFalloff = 0.0;
uniform float StarsIntensity = 0.0;
uniform float CloudRim = 0.0;
uniform float SunIntensity = 0.0;
uniform float SunRadius = 0.0;
uniform float SunShine = 1.0;
uniform float FogOffset = 0.0;
uniform float FogGradation = 0.5;
uniform float FogIntensity = 0.0;

vec2 Unity_TilingAndOffset(vec2 source_uv, vec2 Tiling, vec2 Offset)
{
  return (source_uv * Tiling + Offset) * vec2(1.0, -1.0) + vec2(0.0, 1.0);
}

vec3 Unity_NormalStrength(vec3 In, float Strength)
{
  return vec3(In.rg * Strength, mix(1.0, In.b, clamp(Strength, 0.0, 1.0)));
}

float Unity_Remap(float In, vec2 InMinMax, vec2 OutMinMax)
{
  return OutMinMax.x + (In - InMinMax.x) * (OutMinMax.y - OutMinMax.x) / (InMinMax.y - InMinMax.x);
}

float Unity_Blend_Overlay(float Base, float Blend, float Opacity)
{
  float result1 = 1.0 - 2.0 * (1.0 - Base) * (1.0 - Blend);
  float result2 = 2.0 * Base * Blend;
  float zeroOrOne = step(Base, 0.5);
  float Out = result2 * zeroOrOne + (1.0 - zeroOrOne) * result1;
  return mix(Base, Out, Opacity);
}

float Unity_Blend_Screen(float Base, float Blend, float Opacity)
{
  float Out = 1.0 - (1.0 - Blend) * (1.0 - Base);
  return mix(Base, Out, Opacity);
}

vec4 Unity_Blend_LinearDodge(vec4 Base, vec4 Blend, float Opacity)
{
    vec4 Out = Base + Blend;
    return mix(Base, Out, Opacity);
}

vec3 Unity_TransformWorldToTangent(vec3 dirWS, mat3 tangentToWorld)
{
  vec3 row0 = tangentToWorld[0];
  vec3 row1 = tangentToWorld[1];
  vec3 row2 = tangentToWorld[2];

  vec3 col0 = cross(row1, row2);
  vec3 col1 = cross(row2, row0);
  vec3 col2 = cross(row0, row1);

  float determinant = dot(row0, col0);
  float sgn = determinant<0.0 ? (-1.0) : 1.0;

  mat3 matTBN_I_T = mat3(col0, col1, col2);

  return normalize(sgn * (dirWS * matTBN_I_T));
}

vec3 Unity_RotateAboutAxis_Radians(vec3 In, vec3 Axis, float Rotation)
{
  float s = sin(Rotation);
  float c = cos(Rotation);
  float one_minus_c = 1.0 - c;

  Axis = normalize(Axis);
  vec3 col0 = vec3(one_minus_c * Axis.x * Axis.x + c, one_minus_c * Axis.x * Axis.y - Axis.z * s, one_minus_c * Axis.z * Axis.x + Axis.y * s);
  vec3 col1 = vec3(one_minus_c * Axis.x * Axis.y + Axis.z * s, one_minus_c * Axis.y * Axis.y + c, one_minus_c * Axis.y * Axis.z - Axis.x * s);
  vec3 col2 = vec3(one_minus_c * Axis.z * Axis.x - Axis.y * s, one_minus_c * Axis.y * Axis.z + Axis.x * s, one_minus_c * Axis.z * Axis.z + c);
  mat3 rot_mat = mat3(col0, col1, col2);

  return rot_mat * In;
}


vec3 SkySimple(vec3 WorldSpaceNormal, vec3 WorldSpaceTangent, vec3 WorldSpaceBiTangent, vec3 ObjectSpacePosition, vec2 uv0, vec3 WorldSpaceCameraPosition, vec3 WorldSpaceViewDirection, float TimeParameters)
{
  const float pi = 3.14159;

  vec3 MainLight_Direction_Sun = SkySunDirection;
  vec3 MainLight_Direction_Moon = SkyMoonDirection;

  mat3 tangentTransform_AbsoluteWorld = mat3(WorldSpaceTangent, WorldSpaceBiTangent, WorldSpaceNormal);
  vec3 TangentSpace_LightIncoming = Unity_TransformWorldToTangent(MainLight_Direction_Moon.xyz * -1.0, tangentTransform_AbsoluteWorld);

  vec2 Moon_uv = TangentSpace_LightIncoming.xy * 2.0 / vec2(MoonScale, MoonScale * -1.0) + vec2(0.5, 0.5);
  vec4 Moon_color = sample_trinity(MoonColorMap, vec2(Moon_uv.x, 1.0 - Moon_uv.y), 0);
  vec3 Moon_normal = sample_trinity(MoonNormalMap, vec2(Moon_uv.x, 1.0 - Moon_uv.y), 1).rbg;
  Moon_normal = Moon_normal * 2.0 - 1.0;
  Moon_normal = normalize(Moon_normal);
  float Moon_phase = MoonPhase * 2.0 * pi;
  vec3 Moon_phase_vector = vec3(sin(Moon_phase), cos(Moon_phase), 0.0);
  Moon_phase_vector = Unity_RotateAboutAxis_Radians(Moon_phase_vector, vec3(0.0, 1.0, 0.0), MoonShadingRotation * pi);

  float Moon_facter = (dot(Moon_normal, Moon_phase_vector) + 0.1) * 3.0;
  Moon_facter = mix(0.0, Moon_color.r, clamp(Moon_facter, 0.0, 1.0)) * 0.35;

  vec2 Top_Cloud_Speed = vec2(TimeParameters * TopCloudSpeed, 0.0);
  vec2 Top_Clouds_Base_uv = (WorldSpaceViewDirection * (TopPlaneHeight / min(WorldSpaceViewDirection.y - TopPlaneHorizon, -0.001))).xz;

  vec2 Top_Clouds_Shape_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, vec2 (1.0, 1.0), Top_Cloud_Speed * vec2(0.0005, 0.0));
  vec4 Tex_top_clouds_shape_color = sample_trinity(TopCloudsShapeMap, vec2(Top_Clouds_Shape_uv.x, 1.0 - Top_Clouds_Shape_uv.y), 2);
  float Top_Cloud_Coverage = sqrt(1.0 - pow(TopCloudCoverage - 1.0, 2.0));
  Top_Cloud_Coverage = Unity_Remap(1.0 - Top_Cloud_Coverage, vec2(0.0, 1.0), vec2(0.4, mix(6.0, 3.0, Top_Cloud_Coverage)));
  float Top_Cloud_Shape = Unity_Remap(Tex_top_clouds_shape_color.r, vec2(0.4, Top_Cloud_Coverage), vec2 (0.0, 1.0));
  Top_Cloud_Shape = clamp(Top_Cloud_Shape, 0.0, 1.0);

  vec2 Top_Cloud_Breakup_Speed = Top_Cloud_Speed * (vec2(TopBreakupSpeed));

  vec2 Top_Clouds_Breakup_R_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, vec2(TopPlaneTiling), Top_Cloud_Breakup_Speed * vec2(0.00025, 0.0025));
  vec4 Tex_top_clouds_breakup_color = sample_trinity(TopCloudsBreakupMap, vec2(Top_Clouds_Breakup_R_uv.x, 1.0 - Top_Clouds_Breakup_R_uv.y), 3);
  float Top_Cloud_Breakup_R = Unity_Remap(Tex_top_clouds_breakup_color.r, vec2 (0.0, 0.8), vec2 (0.0, 1.0));

  vec2 Top_Clouds_Breakup_G_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, (vec2(TopPlaneTiling)), Top_Cloud_Breakup_Speed * vec2(0.0005, -0.005));
  vec4 Tex_top_clouds_breakup_g_color = sample_trinity(TopCloudsBreakupMap, vec2(Top_Clouds_Breakup_G_uv.x, 1.0 - Top_Clouds_Breakup_G_uv.y), 3);
  float Top_Cloud_Breakup_G = Unity_Remap(Tex_top_clouds_breakup_g_color.g, vec2 (0.0, 0.8), vec2 (0.0, 1.0));

  float Top_Cloud_Breakup_RG = Unity_Blend_Overlay(Top_Cloud_Breakup_R, Top_Cloud_Breakup_G, 1.0);

  float Top_Cloud_Density = Unity_Blend_Overlay(Top_Cloud_Shape, Top_Cloud_Breakup_RG, 1.0);
  Top_Cloud_Density = clamp(Top_Cloud_Density, 0.0, 1.0);

  vec2 Top_Clouds_Breakup_B_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, (vec2(TopPlaneTiling)), Top_Cloud_Breakup_Speed * vec2(0.00075, 0.00075));
  vec4 Tex_top_clouds_breakup_b_color = sample_trinity(TopCloudsBreakupMap, vec2(Top_Clouds_Breakup_B_uv.x, 1.0 - Top_Clouds_Breakup_B_uv.y), 3);
  float Top_Cloud_Breakup_B = Unity_Remap(Tex_top_clouds_breakup_b_color.b, vec2 (0.15, 0.85), vec2 (0.5, 0.95));

  float Top_Cloud_Mask = mix(0.0, Top_Cloud_Density, Top_Cloud_Breakup_B);
  Top_Cloud_Mask = Unity_Blend_Screen(Top_Cloud_Mask, Top_Cloud_Mask, 1.0);
  Top_Cloud_Mask = clamp(Top_Cloud_Mask, 0.0, 1.0);

  float Top_Cloud_UV0_Y = Unity_Remap(uv0.y, vec2 (0.0, 1.0), vec2 (CloudsFadeOut, 6.0));
  Top_Cloud_UV0_Y = smoothstep(0.0, 1.0, clamp(Top_Cloud_UV0_Y, 0.0, 1.0));
  float Top_Clouds = mix(0.0, (Top_Cloud_Mask * Top_Cloud_UV0_Y), clamp(ObjectSpacePosition.y, 0.0, 1.0));

  float Sun_Moon_Blend_Base = Unity_Remap(Top_Clouds, vec2 (0.5, 1.0), vec2 (0.0, 1.0));

  float Cloud_Moon_Blend = Moon_facter;
  float Moon_Blend_Rate = Unity_Remap(Top_Clouds, vec2 (0.9, 1.0), vec2 (0.0, 1.0)) + Sun_Moon_Blend_Base;
  Cloud_Moon_Blend = mix(smoothstep(0.0, 1.0, Cloud_Moon_Blend), 0.0, min(Moon_Blend_Rate, 1.0));
  Cloud_Moon_Blend *= MoonIntensity;

  vec2 Night_Stars_uv = WorldSpaceViewDirection.xz * (StarsTiling / min(WorldSpaceViewDirection.y - StarsHorizonLift, -0.001));
  vec4 Tex_night_stars_color = sample_trinity(NightStarsMap, Night_Stars_uv, 4);
  vec2 Night_StarsMask_uv = normalize(ObjectSpacePosition).xz;
  float Night_StarsMask_uvRadius = length(Night_StarsMask_uv);
  if (Night_StarsMask_uvRadius > 0.0001) {
    Night_StarsMask_uvRadius = asin(clamp(Night_StarsMask_uvRadius, 0.0, 1.0)) * 2.0 / PI / Night_StarsMask_uvRadius;
    Night_StarsMask_uv = Night_StarsMask_uv * Night_StarsMask_uvRadius * 0.5 + vec2(0.5);
  }
  Night_StarsMask_uv = Night_StarsMask_uv * StarsMaskTiling + vec2(TimeParameters * StarsMaskSpeed, 0.0);
  float Tex_night_stars_mask = sample_trinity(NightStarsMaskMap, Night_StarsMask_uv, 5).r;
  float Night_Stars_Color_B = clamp((Tex_night_stars_color.b - Moon_color.g), 0.0, 1.0);
  float Night_Stars_Falloff = Unity_Remap(ObjectSpacePosition.y, vec2(HorizonFalloff, 1.0), vec2 (0.06, 0.1));
  Night_Stars_Falloff = clamp(Night_Stars_Falloff, 0.0, 1.0);
  float Night_Stars_Factor = Night_Stars_Color_B * Night_Stars_Falloff * 3.0;
  Night_Stars_Factor *= StarsIntensity * Tex_night_stars_mask;

  float Sky_Color_Horizon_Falloff = Unity_Remap(ObjectSpacePosition.y, vec2(HorizonFalloff, 1.0), vec2 (0.0, 0.1));
  Sky_Color_Horizon_Falloff = clamp(Sky_Color_Horizon_Falloff, 0.0, 1.0);
  Sky_Color_Horizon_Falloff = smoothstep(0.0, 1.0, Sky_Color_Horizon_Falloff);

  float Sky_Elevation_Falloff = clamp(1.0 - MainLight_Direction_Sun.y - 0.3, 0.0, 1.0);
  vec4 Sky_Color = mix(HorizonColor, ZenithColor, Sky_Color_Horizon_Falloff);

  vec4 Cloud_Color_Rim = CloudColorRim * 2.5;

  float Top_Clouds_Pow = clamp(Top_Clouds * Top_Clouds, 0.0, 1.0);
  float Top_Clouds_Pow_C = clamp(mix(-0.5, 1.5, Top_Clouds_Pow), 0.0, 1.0);

  Sky_Color += (1.0 - Top_Clouds_Pow_C) * vec4(Night_Stars_Factor);

  vec4 Sky_Top_Color = Unity_Blend_LinearDodge(Sky_Color, (Cloud_Color_Rim * Top_Clouds_Pow * Top_Clouds), Top_Clouds_Pow_C);

  vec4 Cloud_Color = CloudColor;

  float Cloud_Rim = Unity_Remap(Top_Clouds_Pow, vec2(CloudRim, 1.0), vec2 (0.0, 1.0));
  Cloud_Rim = clamp(Cloud_Rim, 0.0, 1.0);
  Cloud_Rim = smoothstep(0.0, 1.0, Cloud_Rim);

  Sky_Top_Color = mix(Sky_Top_Color, Cloud_Color, Cloud_Rim);
  Sky_Color = Unity_Blend_LinearDodge(Sky_Color, Cloud_Color * Top_Clouds_Pow, Top_Clouds_Pow_C);
  float Sky_Top_Blend_Sun = 1.0 - clamp(dot(MainLight_Direction_Sun, WorldSpaceNormal) + 0.6, 0.0, 1.0);
  float Sky_Top_Blend_Moon = 1.0 - clamp(dot(MainLight_Direction_Moon, WorldSpaceNormal) + 0.6, 0.0, 1.0);
  float Sky_Top_Blend = max(Sky_Top_Blend_Moon * min(MoonIntensity, 1.0), Sky_Top_Blend_Sun * min(SunIntensity, 1.0));
  vec4 Sky_Color_Base = mix(Sky_Color, Sky_Top_Color, Sky_Top_Blend);

  float Sun_Radius = Unity_Remap(SunRadius, vec2(0.0, 1.0), vec2(0.01, 1.0));
  Sun_Radius = mix(1.0, 0.0, clamp(acos(clamp(dot(MainLight_Direction_Sun, -WorldSpaceNormal), -1.0, 1.0)) / Sun_Radius, 0.0, 1.0));
  Sun_Radius = pow(Sun_Radius, max(Unity_Remap(SunShine, vec2(1.0, 2.0), vec2(0.002, 5.0)), 0.00001));
  vec3 Sun_Color = SunTintColor.rgb * Sun_Radius;

  vec3 Sun_With_Cloud = Sun_Color;
  Sun_With_Cloud = mix(Sun_With_Cloud, vec3(0.0, 0.0, 0.0), Sun_Moon_Blend_Base);
  Sun_With_Cloud *= SunIntensity;

  vec3 Final_Sky_Color = vec3(Cloud_Moon_Blend) + Sun_With_Cloud + Sky_Color_Base.rgb;

  float Sky_Fog_Gradation = sqrt(1.0 - pow(uv0.y, 2.0)) + Unity_Remap(pow(FogOffset, 3.0), vec2(0.0, 1.0), vec2(-0.5, 0.5));
  Sky_Fog_Gradation = clamp(smoothstep(1.0 - FogGradation, FogGradation, Sky_Fog_Gradation), 0.0, 1.0);

  vec3 Sky_Fog_Color = mix(Final_Sky_Color, FogColor.rgb, Sky_Fog_Gradation);

  Final_Sky_Color = mix(Final_Sky_Color, Sky_Fog_Color, FogIntensity);
  return Final_Sky_Color;
}

void vertex() { 
  sky_object_position = VERTEX; 
}

void fragment() {
    vec3 world_normal = normalize((INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz);
    vec3 world_tangent = normalize((INV_VIEW_MATRIX * vec4(TANGENT, 0.0)).xyz);
    vec3 world_binormal = normalize((INV_VIEW_MATRIX * vec4(BINORMAL, 0.0)).xyz);
    vec3 view_dir = normalize((INV_VIEW_MATRIX * vec4(VIEW, 0.0)).xyz);
    ALBEDO = SkySimple(world_normal, world_tangent, world_binormal, sky_object_position, UV, CAMERA_POSITION_WORLD, view_dir, TIME);
}
