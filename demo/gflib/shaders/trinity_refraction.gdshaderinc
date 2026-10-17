// Screen-space approximation using Godot's opaque scene copy. Transparent
// surfaces behind the current mesh are absent from that copy.
uniform sampler2D trinity_scene_color : hint_screen_texture, filter_linear_mipmap, repeat_disable;
uniform sampler2D trinity_scene_depth : hint_depth_texture, filter_nearest, repeat_disable;
vec2 trinity_refract_uv(vec3 position, vec3 normal, vec3 view_dir, float ior, float thickness, mat4 projection) {
    vec3 ray = refract(-view_dir, normal, 1.0 / max(ior, 1.00001));
    vec4 clip = projection * vec4(position + ray * thickness, 1.0);
    float divisor = abs(clip.w) < 0.00001 ? 0.00001 : clip.w;
    return clamp(clip.xy / divisor * 0.5 + 0.5, vec2(0.0), vec2(1.0));
}
float trinity_depth_difference(vec2 uv, float pixel_depth, mat4 inverse_projection) {
    float depth = textureLod(trinity_scene_depth, uv, 0.0).r;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
    depth = depth * 2.0 - 1.0;
#endif
    vec4 position = inverse_projection * vec4(uv * 2.0 - 1.0, depth, 1.0);
    if (abs(position.w) < 0.00001) return 1000000.0;
    float scene_depth = -position.z / position.w;
    return abs(scene_depth - pixel_depth);
}

float trinity_depth_fade(vec2 uv, float pixel_depth, float distance_scale, mat4 inverse_projection) {
    return clamp(trinity_depth_difference(uv, pixel_depth, inverse_projection) / max(distance_scale, 0.00001), 0.0, 1.0);
}
