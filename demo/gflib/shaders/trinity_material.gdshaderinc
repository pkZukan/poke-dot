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
vec3 material_saturation(vec3 color, float saturation) { return mix(vec3(dot(color, vec3(0.2126729, 0.7151522, 0.0721750))), color, saturation); }
// Layer mask UV transform: rotate by center_rotation.z degrees about
// center_rotation.xy, then scale/offset around it. Takes an unflipped UV.
vec2 material_layer_mask_uv(vec2 uv, vec4 scale_offset, vec4 center_rotation) {
    float angle = radians(center_rotation.z), c = cos(angle), s = sin(angle);
    vec2 d = uv - center_rotation.xy;
    vec2 st = (vec2(c * d.x + s * d.y, c * d.y - s * d.x) - scale_offset.zw) * scale_offset.xy + center_rotation.xy;
    return vec2(st.x, 1.0 - st.y);
}
// Godot derives dielectric F0 as 0.16 * SPECULAR^2, retail shaders use F0 directly.
float material_specular(float f0) { return sqrt(clamp(f0 / 0.16, 0.0, 1.0)); }
