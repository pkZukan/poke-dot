shader_type spatial;
render_mode unshaded, cull_disabled, fog_disabled, shadows_disabled;

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

// The dome is shaded from the Sky light of the active TRLGT stack (time-of-day
// curves), not from the floats stored in the sky_mat trmtr. These are shared by
// every sky material, so they're global uniforms (registered by the extension)
// named after the TRLGT parameters; TrinityEnvironment updates them.
global uniform vec3 SkySunDirection;    // toward the sun
global uniform vec3 SkyMoonDirection;
global uniform vec4 SkyZenithColor;     // colors are used as stored (no sRGB conversion)
global uniform vec4 SkyHorizonColor;
global uniform vec4 SkyCloudColor;
global uniform vec4 SkyCloudColorRim;
global uniform vec4 SkySunTintColor;
global uniform vec4 SkyFogColor;
global uniform vec4 ScatteringFogColor; // premultiplied by its own w
global uniform float ScatteringFogFactor;
global uniform float SkyHorizonFalloff;
global uniform float SkyCloudRim;
global uniform float SkyCloudsFadeOut;
global uniform float SkyTopCloudSpeed;
global uniform float SkyTopBreakupSpeed;
global uniform float SkyTopPlaneHeight;
global uniform float SkyTopPlaneHorizon;
global uniform float SkyTopPlaneTiling;
global uniform float SkyTopCloudCoverage;
global uniform float SkySunRadius;
global uniform float SkySunShine;
global uniform float SkySunBrightness;
global uniform float SkyMoonScale;
global uniform float SkyMoonPhase;
global uniform float SkyMoonShadingRotation;
global uniform float SkyMoonBrightness;
global uniform float SkyStarsTiling;
global uniform float SkyStarsHorizonLift;
global uniform float SkyStarsIntensity;
global uniform float SkyStarsMaskTiling;
global uniform float SkyStarsMaskSpeed;
global uniform float SkyFogIntensity;
global uniform float SkyFogOffset;
global uniform float SkyFogGradation;
global uniform sampler2D AtmosphereLUT : filter_linear, repeat_disable;

vec2 Unity_TilingAndOffset(vec2 source_uv, vec2 Tiling, vec2 Offset)
{
  return (source_uv * Tiling + Offset) * vec2(1.0, -1.0) + vec2(0.0, 1.0);
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
  float sgn = determinant < 0.0 ? -1.0 : 1.0;

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

vec3 SkySimple(vec3 WorldSpaceNormal, vec3 WorldSpaceTangent, vec3 WorldSpaceBiTangent, vec3 ObjectSpacePosition, vec2 uv0, vec3 WorldSpaceViewDirection, float TimeParameters)
{
  const float pi = 3.14159;

  vec3 MainLight_Direction_Sun = SkySunDirection;
  vec3 MainLight_Direction_Moon = SkyMoonDirection;

  // --- Moon ---
  mat3 tangentTransform_AbsoluteWorld = mat3(WorldSpaceTangent, WorldSpaceBiTangent, WorldSpaceNormal);
  vec3 TangentSpace_LightIncoming = Unity_TransformWorldToTangent(MainLight_Direction_Moon * -1.0, tangentTransform_AbsoluteWorld);

  vec2 Moon_uv = TangentSpace_LightIncoming.xy * 2.0 / vec2(SkyMoonScale, SkyMoonScale * -1.0) + vec2(0.5, 0.5);
  vec4 Moon_color = sample_trinity(MoonColorMap, vec2(Moon_uv.x, 1.0 - Moon_uv.y), 0);
  vec3 Moon_normal = sample_trinity(MoonNormalMap, vec2(Moon_uv.x, 1.0 - Moon_uv.y), 1).rbg;
  Moon_normal = normalize(Moon_normal * 2.0 - 1.0);
  float Moon_phase = SkyMoonPhase * 2.0 * pi;
  vec3 Moon_phase_vector = vec3(sin(Moon_phase), cos(Moon_phase), 0.0);
  Moon_phase_vector = Unity_RotateAboutAxis_Radians(Moon_phase_vector, vec3(0.0, 1.0, 0.0), SkyMoonShadingRotation * pi);

  float Moon_facter = (dot(Moon_normal, Moon_phase_vector) + 0.1) * 3.0;
  Moon_facter = mix(0.0, Moon_color.r, clamp(Moon_facter, 0.0, 1.0)) * 0.35;

  // --- top clouds ---
  vec2 Top_Cloud_Speed = vec2(TimeParameters * SkyTopCloudSpeed, 0.0);
  // Unclamped divide (only the stars divide is clamped).
  vec2 Top_Clouds_Base_uv = (WorldSpaceViewDirection * (SkyTopPlaneHeight / (WorldSpaceViewDirection.y - SkyTopPlaneHorizon))).xz;

  vec2 Top_Clouds_Shape_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, vec2(1.0, 1.0), Top_Cloud_Speed * vec2(0.0005, 0.0));
  vec4 Tex_top_clouds_shape_color = sample_trinity(TopCloudsShapeMap, vec2(Top_Clouds_Shape_uv.x, 1.0 - Top_Clouds_Shape_uv.y), 2);
  float Top_Cloud_Coverage = sqrt(1.0 - pow(SkyTopCloudCoverage - 1.0, 2.0));
  Top_Cloud_Coverage = Unity_Remap(1.0 - Top_Cloud_Coverage, vec2(0.0, 1.0), vec2(0.4, mix(6.0, 3.0, Top_Cloud_Coverage)));
  float Top_Cloud_Shape = Unity_Remap(Tex_top_clouds_shape_color.r, vec2(0.4, Top_Cloud_Coverage), vec2(0.0, 1.0));
  Top_Cloud_Shape = clamp(Top_Cloud_Shape, 0.0, 1.0);

  vec2 Top_Cloud_Breakup_Speed = Top_Cloud_Speed * vec2(SkyTopBreakupSpeed);

  vec2 Top_Clouds_Breakup_R_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, vec2(SkyTopPlaneTiling), Top_Cloud_Breakup_Speed * vec2(0.00025, 0.0025));
  vec4 Tex_top_clouds_breakup_color = sample_trinity(TopCloudsBreakupMap, vec2(Top_Clouds_Breakup_R_uv.x, 1.0 - Top_Clouds_Breakup_R_uv.y), 3);
  float Top_Cloud_Breakup_R = Unity_Remap(Tex_top_clouds_breakup_color.r, vec2(0.0, 0.8), vec2(0.0, 1.0));

  vec2 Top_Clouds_Breakup_G_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, vec2(SkyTopPlaneTiling), Top_Cloud_Breakup_Speed * vec2(0.0005, -0.005));
  vec4 Tex_top_clouds_breakup_g_color = sample_trinity(TopCloudsBreakupMap, vec2(Top_Clouds_Breakup_G_uv.x, 1.0 - Top_Clouds_Breakup_G_uv.y), 3);
  float Top_Cloud_Breakup_G = Unity_Remap(Tex_top_clouds_breakup_g_color.g, vec2(0.0, 0.8), vec2(0.0, 1.0));

  float Top_Cloud_Breakup_RG = Unity_Blend_Overlay(Top_Cloud_Breakup_R, Top_Cloud_Breakup_G, 1.0);

  float Top_Cloud_Density = Unity_Blend_Overlay(Top_Cloud_Shape, Top_Cloud_Breakup_RG, 1.0);
  Top_Cloud_Density = clamp(Top_Cloud_Density, 0.0, 1.0);

  vec2 Top_Clouds_Breakup_B_uv = Unity_TilingAndOffset(Top_Clouds_Base_uv, vec2(SkyTopPlaneTiling), Top_Cloud_Breakup_Speed * vec2(0.00075, 0.00075));
  vec4 Tex_top_clouds_breakup_b_color = sample_trinity(TopCloudsBreakupMap, vec2(Top_Clouds_Breakup_B_uv.x, 1.0 - Top_Clouds_Breakup_B_uv.y), 3);
  float Top_Cloud_Breakup_B = Unity_Remap(Tex_top_clouds_breakup_b_color.b, vec2(0.15, 0.85), vec2(0.5, 0.95));

  float Top_Cloud_Mask = mix(0.0, Top_Cloud_Density, Top_Cloud_Breakup_B);
  Top_Cloud_Mask = Unity_Blend_Screen(Top_Cloud_Mask, Top_Cloud_Mask, 1.0);
  Top_Cloud_Mask = clamp(Top_Cloud_Mask, 0.0, 1.0);

  float Top_Cloud_UV0_Y = Unity_Remap(uv0.y, vec2(0.0, 1.0), vec2(SkyCloudsFadeOut, 6.0));
  Top_Cloud_UV0_Y = smoothstep(0.0, 1.0, clamp(Top_Cloud_UV0_Y, 0.0, 1.0));
  float Top_Clouds = mix(0.0, Top_Cloud_Mask * Top_Cloud_UV0_Y, clamp(ObjectSpacePosition.y, 0.0, 1.0));

  float Sun_Moon_Blend_Base = Unity_Remap(Top_Clouds, vec2(0.5, 1.0), vec2(0.0, 1.0));

  // --- blend moon with cloud ---
  float Cloud_Moon_Blend = Moon_facter;
  float Moon_Blend_Rate = Unity_Remap(Top_Clouds, vec2(0.9, 1.0), vec2(0.0, 1.0)) + Sun_Moon_Blend_Base;
  Cloud_Moon_Blend = mix(smoothstep(0.0, 1.0, Cloud_Moon_Blend), 0.0, min(Moon_Blend_Rate, 1.0));
  Cloud_Moon_Blend *= SkyMoonBrightness;

  // --- stars ---
  vec2 Night_Stars_uv = WorldSpaceViewDirection.xz * (SkyStarsTiling / min(WorldSpaceViewDirection.y - SkyStarsHorizonLift, -0.001));
  vec4 Tex_night_stars_color = sample_trinity(NightStarsMap, Night_Stars_uv, 4);
  vec2 Night_StarsMask_uv = normalize(ObjectSpacePosition).xz;
  float Night_StarsMask_uvRadius = length(Night_StarsMask_uv);
  if (Night_StarsMask_uvRadius > 0.0001) {
    Night_StarsMask_uvRadius = asin(Night_StarsMask_uvRadius) * 2.0 / PI / Night_StarsMask_uvRadius;
    Night_StarsMask_uv = Night_StarsMask_uv * Night_StarsMask_uvRadius * 0.5 + vec2(0.5);
  }
  Night_StarsMask_uv = Night_StarsMask_uv * SkyStarsMaskTiling + vec2(TimeParameters * SkyStarsMaskSpeed, 0.0);
  float Tex_night_stars_mask = sample_trinity(NightStarsMaskMap, Night_StarsMask_uv, 5).r;
  float Night_Stars_Color_B = clamp(Tex_night_stars_color.b - Moon_color.g, 0.0, 1.0);
  float Night_Stars_Falloff = Unity_Remap(ObjectSpacePosition.y, vec2(SkyHorizonFalloff, 1.0), vec2(0.06, 0.1));
  Night_Stars_Falloff = clamp(Night_Stars_Falloff, 0.0, 1.0);
  float Night_Stars_Factor = Night_Stars_Color_B * Night_Stars_Falloff * 3.0;
  Night_Stars_Factor *= SkyStarsIntensity * Tex_night_stars_mask;

  // --- sky coloring ---
  float Sky_Color_Horizon_Falloff = Unity_Remap(ObjectSpacePosition.y, vec2(SkyHorizonFalloff, 1.0), vec2(0.0, 0.1));
  Sky_Color_Horizon_Falloff = clamp(Sky_Color_Horizon_Falloff, 0.0, 1.0);
  Sky_Color_Horizon_Falloff = smoothstep(0.0, 1.0, Sky_Color_Horizon_Falloff);

  vec4 Sky_Color = mix(SkyHorizonColor, SkyZenithColor, Sky_Color_Horizon_Falloff);

  // --- cloud coloring ---
  vec4 Cloud_Color_Rim = SkyCloudColorRim * 2.5;

  float Top_Clouds_Pow = clamp(Top_Clouds * Top_Clouds, 0.0, 1.0);
  float Top_Clouds_Pow_C = clamp(mix(-0.5, 1.5, Top_Clouds_Pow), 0.0, 1.0);

  Sky_Color += (1.0 - Top_Clouds_Pow_C) * vec4(Night_Stars_Factor);

  vec4 Sky_Top_Color = Unity_Blend_LinearDodge(Sky_Color, Cloud_Color_Rim * Top_Clouds_Pow * Top_Clouds, Top_Clouds_Pow_C);

  vec4 Cloud_Color = SkyCloudColor;

  float Cloud_Rim = Unity_Remap(Top_Clouds_Pow, vec2(SkyCloudRim, 1.0), vec2(0.0, 1.0));
  Cloud_Rim = clamp(Cloud_Rim, 0.0, 1.0);
  Cloud_Rim = smoothstep(0.0, 1.0, Cloud_Rim);

  Sky_Top_Color = mix(Sky_Top_Color, Cloud_Color, Cloud_Rim);
  Sky_Color = Unity_Blend_LinearDodge(Sky_Color, Cloud_Color * Top_Clouds_Pow, Top_Clouds_Pow_C);
  float Sky_Top_Blend_Sun = 1.0 - clamp(dot(MainLight_Direction_Sun, WorldSpaceNormal) + 0.6, 0.0, 1.0);
  float Sky_Top_Blend_Moon = 1.0 - clamp(dot(MainLight_Direction_Moon, WorldSpaceNormal) + 0.6, 0.0, 1.0);
  float Sky_Top_Blend = max(Sky_Top_Blend_Moon * min(SkyMoonBrightness, 1.0), Sky_Top_Blend_Sun * min(SkySunBrightness, 1.0));
  vec4 Sky_Color_Base = mix(Sky_Color, Sky_Top_Color, Sky_Top_Blend);

  // --- sun ---
  float Sun_Radius = Unity_Remap(SkySunRadius, vec2(0.0, 1.0), vec2(0.01, 1.0));
  Sun_Radius = mix(1.0, 0.0, clamp(acos(clamp(dot(MainLight_Direction_Sun, -WorldSpaceNormal), -1.0, 1.0)) / Sun_Radius, 0.0, 1.0));
  Sun_Radius = pow(Sun_Radius, Unity_Remap(SkySunShine, vec2(1.0, 2.0), vec2(0.002, 5.0)));
  vec3 Sun_Color = SkySunTintColor.rgb * Sun_Radius;

  // --- blend sun with cloud ---
  vec3 Sun_With_Cloud = mix(Sun_Color, vec3(0.0), Sun_Moon_Blend_Base);
  Sun_With_Cloud *= SkySunBrightness;

  vec3 Final_Sky_Color = vec3(Cloud_Moon_Blend) + Sun_With_Cloud + Sky_Color_Base.rgb;

  // --- sky fog ---
  float Sky_Fog_Gradation = sqrt(1.0 - pow(uv0.y, 2.0)) + Unity_Remap(pow(SkyFogOffset, 3.0), vec2(0.0, 1.0), vec2(-0.5, 0.5));
  Sky_Fog_Gradation = clamp(smoothstep(1.0 - SkyFogGradation, SkyFogGradation, Sky_Fog_Gradation), 0.0, 1.0);

  vec3 Sky_Fog_Color = mix(Final_Sky_Color, SkyFogColor.rgb, Sky_Fog_Gradation);
  return mix(Final_Sky_Color, Sky_Fog_Color, SkyFogIntensity);
}

// --- Scattering fog (Bruneton 2017 parameterisation; the LUT is baked at a
// fixed camera height, so only mu and mu_s vary) ---
#define SCATTER_TOP_RADIUS 6420000.0
#define SCATTER_BOTTOM_RADIUS 6360000.0
#define SCATTER_CAMERA_HEIGHT 100.0
#define SCATTER_LUT_SIZE_X 32
#define SCATTER_LUT_SIZE_Y 128

float scatter_texture_coord(float x, int texture_size)
{
  return 0.5 / float(texture_size) + x * (1.0 - 1.0 / float(texture_size));
}

float scatter_safe_sqrt(float a)
{
  return sqrt(max(a, 0.0));
}

float scatter_distance_to_top(float r, float mu)
{
  float discriminant = r * r * (mu * mu - 1.0) + SCATTER_TOP_RADIUS * SCATTER_TOP_RADIUS;
  return max(-r * mu + scatter_safe_sqrt(discriminant), 0.0);
}

bool scatter_ray_intersects_ground(float r, float mu)
{
  return mu < 0.0 && r * r * (mu * mu - 1.0) + SCATTER_BOTTOM_RADIUS * SCATTER_BOTTOM_RADIUS >= 0.0;
}

float scatter_mie_phase(float g, float nu)
{
  float k = 3.0 / (8.0 * PI) * (1.0 - g * g) / (2.0 + g * g);
  return k * (1.0 + nu * nu) / pow(abs(1.0 + g * g - 2.0 * g * nu), 1.5);
}

vec2 scatter_scattering_uv(float r, float mu, float mu_s, bool ray_r_mu_intersects_ground)
{
  float H = sqrt(SCATTER_TOP_RADIUS * SCATTER_TOP_RADIUS - SCATTER_BOTTOM_RADIUS * SCATTER_BOTTOM_RADIUS);
  float rho = scatter_safe_sqrt(r * r - SCATTER_BOTTOM_RADIUS * SCATTER_BOTTOM_RADIUS);

  float r_mu = r * mu;
  float discriminant = r_mu * r_mu - r * r + SCATTER_BOTTOM_RADIUS * SCATTER_BOTTOM_RADIUS;
  float u_mu;

  if (ray_r_mu_intersects_ground) {
    float d = -r_mu - scatter_safe_sqrt(discriminant);
    float d_min = r - SCATTER_BOTTOM_RADIUS;
    float d_max = rho;
    u_mu = 0.5 - 0.5 * scatter_texture_coord(d_max == d_min ? 0.0 : (d - d_min) / (d_max - d_min), SCATTER_LUT_SIZE_Y / 2);
  } else {
    float d = -r_mu + scatter_safe_sqrt(discriminant + H * H);
    float d_min = SCATTER_TOP_RADIUS - r;
    float d_max = rho + H;
    u_mu = 0.5 + 0.5 * scatter_texture_coord((d - d_min) / (d_max - d_min), SCATTER_LUT_SIZE_Y / 2);
  }

  float d = scatter_distance_to_top(SCATTER_BOTTOM_RADIUS, mu_s);
  float d_min = SCATTER_TOP_RADIUS - SCATTER_BOTTOM_RADIUS;
  float d_max = H;
  float a = (d - d_min) / (d_max - d_min);
  float A = 0.4 * SCATTER_BOTTOM_RADIUS / (d_max - d_min);
  float u_mu_s = scatter_texture_coord(max(1.0 - a / A, 0.0) / (1.0 + a), SCATTER_LUT_SIZE_X);

  return vec2(u_mu_s, u_mu);
}

float sky_scattering_fog(float mie_scatter_phase, vec3 view_direction)
{
  vec3 camera = vec3(0.0, SCATTER_CAMERA_HEIGHT + SCATTER_BOTTOM_RADIUS, 0.0);
  float r = camera.y;
  float mu = dot(camera, view_direction) / r;
  float mu_s = dot(camera, SkySunDirection) / r;
  float nu = dot(view_direction, SkySunDirection);
  bool ground = scatter_ray_intersects_ground(r, mu);

  vec2 uv = scatter_scattering_uv(r, mu, mu_s, ground);
  return textureLod(AtmosphereLUT, uv, 0.0).r * scatter_mie_phase(mie_scatter_phase, nu);
}

vec3 trinity_sky_color(vec3 normal_vec, vec3 tangent_vec, vec3 binormal_vec, vec3 object_pos, vec2 uv0, vec3 view_vec, float time)
{
  vec3 sky_color = SkySimple(normal_vec, tangent_vec, binormal_vec, object_pos, uv0, view_vec, time);

  if (ScatteringFogColor.a > 0.0)
    sky_color += sky_scattering_fog(ScatteringFogFactor, -normal_vec) * ScatteringFogColor.rgb;

  return sky_color;
}

varying vec3 sky_object_position;
varying vec3 sky_world_position;
varying vec3 sky_world_normal;
varying vec4 sky_world_tangent;

void vertex() {
    // Use the mesh normal as authored: Godot flips NORMAL for back faces.
    sky_object_position = VERTEX;
    sky_world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
    sky_world_normal = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
    sky_world_tangent = vec4(normalize((MODEL_MATRIX * vec4(TANGENT, 0.0)).xyz), sign(dot(cross(NORMAL, TANGENT), BINORMAL)));
}

void fragment() {
    vec3 normal_vec = normalize(sky_world_normal);
    vec3 tangent_vec = normalize(sky_world_tangent.xyz);
    vec3 binormal_vec = sky_world_tangent.w * cross(normal_vec, tangent_vec);
    vec3 view_vec = normalize(CAMERA_POSITION_WORLD - sky_world_position);

    ALBEDO = trinity_sky_color(normal_vec, tangent_vec, binormal_vec, sky_object_position, UV, view_vec, TIME);
}
