vec2 material_uv(vec2 uv, vec4 transform) {
    vec2 result = (uv - transform.zw) * transform.xy;
    return vec2(result.x, 1.0 - result.y);
}
vec3 material_normal(vec4 value, float strength) {
    vec2 xy = (value.rg * 2.0 - 1.0) * strength;
    return vec3(xy, sqrt(max(0.0, 1.0 - dot(xy, xy))));
}
vec3 material_normal_strength(vec3 n, float strength) {
    return vec3(n.xy * strength, mix(1.0, n.z, clamp(strength, 0.0, 1.0)));
}
float material_contrast(float value, float contrast) { return clamp(mix(-contrast, 1.0 + contrast, value), 0.0, 1.0); }
float material_overlay(float base, float blend) { return base < 0.5 ? 1.0 - 2.0 * (1.0 - base) * (1.0 - blend) : 2.0 * base * blend; }
vec3 material_saturation(vec3 color, float saturation) { return mix(vec3(dot(color, vec3(0.299, 0.587, 0.114))), color, saturation); }
