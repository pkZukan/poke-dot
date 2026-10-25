#ifndef TRINITY_LIGHT_INCLUDED
#define TRINITY_LIGHT_INCLUDED

// Retail ShadowFactor: the directional shadow is min(baked world shadow, cascade shadow),
// and ReceiveShadow=0 materials get neither. TrinityEnvironment drives these globals.
global uniform sampler2D TrinityWorldShadowMap : filter_linear, repeat_disable;
global uniform vec4 TrinityWorldShadowWeights;   // RGBA channel weights for the time of day
global uniform vec4 TrinityWorldShadowTransform; // uv = world.xz * xy + zw
global uniform vec4 TrinityWorldShadowParams;    // x fade distance, y fade power, z intensity (0 = off)

// The baked map takes over from the cascades beyond the fade distance.
float trinity_world_shadow(vec3 world, vec3 camera) {
    if (TrinityWorldShadowParams.z <= 0.0) return 1.0;
    vec2 uv = world.xz * TrinityWorldShadowTransform.xy + TrinityWorldShadowTransform.zw;
    float baked = dot(textureLod(TrinityWorldShadowMap, uv, 0.0), TrinityWorldShadowWeights);
    float fade = max(0.0, 1.0 - pow(distance(world, camera) / TrinityWorldShadowParams.x, TrinityWorldShadowParams.y));
    return 1.0 - TrinityWorldShadowParams.z * (1.0 - min(baked + fade, 1.0));
}

// light() has no VERTEX, so the world position is rebuilt from the fragment depth.
float trinity_light_attenuation(float attenuation, bool directional, int receive_shadow, vec2 screen_uv, float depth, mat4 inv_projection, mat4 inv_view) {
    if (!directional) return attenuation;
    if (receive_shadow <= 0) return 1.0;
    vec4 view = inv_projection * vec4(screen_uv * 2.0 - 1.0, depth, 1.0);
    vec3 world = (inv_view * vec4(view.xyz / view.w, 1.0)).xyz;
    return min(attenuation, trinity_world_shadow(world, inv_view[3].xyz));
}
#define TRINITY_ATTENUATION(receive) trinity_light_attenuation(ATTENUATION, LIGHT_IS_DIRECTIONAL, receive, SCREEN_UV, FRAGCOORD.z, INV_PROJECTION_MATRIX, INV_VIEW_MATRIX)

// Retail casts the sun's shadows from ShadowTransform but shades with DirectionalMain.
// The Godot sun points along the shadow caster; shade it with DirectionalMain instead.
global uniform vec4 TrinitySunShadowDirection;  // world, toward the light; w = 1 when the directions differ
global uniform vec3 TrinitySunShadingDirection; // world, toward the light
vec3 trinity_light_direction(vec3 light, bool directional, mat4 view) {
    if (!directional || TrinitySunShadowDirection.w <= 0.0) return light;
    if (dot(light, normalize(mat3(view) * TrinitySunShadowDirection.xyz)) < 0.9999) return light;
    return normalize(mat3(view) * TrinitySunShadingDirection);
}
#define TRINITY_LIGHT trinity_light_direction(LIGHT, LIGHT_IS_DIRECTIONAL, VIEW_MATRIX)

#ifdef TRINITY_CUSTOM_LIGHT
#define TRINITY_LIGHT_FRAGMENT
#else
uniform int ReceiveShadow = 1;

// light() can't read SPECULAR, so fragment() hands the dielectric F0 over.
varying float trinity_dielectric_f0;
#define TRINITY_LIGHT_FRAGMENT trinity_dielectric_f0 = 0.16 * SPECULAR * SPECULAR

float trinity_schlick(float u) {
    float m = 1.0 - u, m2 = m * m;
    return m2 * m2 * m;
}

// Godot's default Burley diffuse and Schlick-GGX specular, plus the world shadow.
// Only Godot's multiscatter energy compensation is missing, which light() can't reach.
void light() {
    float attenuation = TRINITY_ATTENUATION(ReceiveShadow);
    vec3 L = TRINITY_LIGHT;
    float cNdotL = max(min(dot(NORMAL, L), 1.0), 0.0);
    float cNdotV = max(dot(NORMAL, VIEW), 1e-4);
    vec3 H = normalize(VIEW + L);
    float cLdotH = clamp(dot(L, H), 0.0, 1.0);
    if (METALLIC < 1.0) {
        float fd90_minus_1 = 2.0 * cLdotH * cLdotH * ROUGHNESS - 0.5;
        float diffuse = (1.0 + fd90_minus_1 * trinity_schlick(cNdotV)) * (1.0 + fd90_minus_1 * trinity_schlick(cNdotL)) * cNdotL / PI;
        DIFFUSE_LIGHT += LIGHT_COLOR * diffuse * attenuation;
        DIFFUSE_LIGHT += LIGHT_COLOR * (1.0 / PI - diffuse) * BACKLIGHT * attenuation;
    }
    if (ROUGHNESS > 0.0) {
        float cNdotH = clamp(dot(NORMAL, H), 0.0, 1.0);
        float alpha = ROUGHNESS * ROUGHNESS;
        float a = cNdotH * alpha;
        float k = alpha / (1.0 - cNdotH * cNdotH + a * a);
        float d = k * k / PI;
        float v = clamp(0.5 / mix(2.0 * cNdotL * cNdotV, cNdotL + cNdotV, alpha), 0.0, 1.0);
        vec3 f0 = mix(vec3(trinity_dielectric_f0), ALBEDO, METALLIC);
        float f90 = clamp(dot(f0, vec3(50.0 * 0.33)), METALLIC, 1.0);
        vec3 fresnel = f0 + (f90 - f0) * trinity_schlick(cLdotH);
        SPECULAR_LIGHT += cNdotL * d * fresnel * v * LIGHT_COLOR * attenuation * SPECULAR_AMOUNT;
    }
}
#endif

#endif
