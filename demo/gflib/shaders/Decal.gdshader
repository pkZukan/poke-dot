shader_type spatial;
render_mode unshaded, blend_mul, fog_disabled, depth_draw_never, depth_test_disabled, cull_disabled;
uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;

uniform int sampler_repeat_u[1];
uniform int sampler_repeat_v[1];
uniform vec4 sampler_border_color[1];
#include "trinity_sampler.gdshaderinc"
#include "trinity_refraction.gdshaderinc"

uniform bool UseSceneProjection = true;
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);

void fragment() {
    vec2 uv = (UV - UVScaleOffset.zw) * UVScaleOffset.xy;
    uv.y = 1.0 - uv.y;
    float height = 1.0;
    if (UseSceneProjection) {
        float depth = textureLod(trinity_scene_depth, SCREEN_UV, 0.0).r;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
        depth = depth * 2.0 - 1.0;
#endif
        vec4 position = INV_PROJECTION_MATRIX * vec4(SCREEN_UV * 2.0 - 1.0, depth, 1.0);
        if (abs(position.w) < 0.00001) discard;
        vec4 world = INV_VIEW_MATRIX * vec4(position.xyz / position.w, 1.0);
        vec3 local = (inverse(MODEL_MATRIX) * world).xyz;
        if (any(greaterThanEqual(abs(local), vec3(0.5)))) discard;
        uv = local.xz + vec2(0.5);
        height = clamp(1.0 - 2.0 * abs(local.y), 0.0, 1.0);
    }
    vec4 color = sample_trinity(BaseColorMap, uv, 0);
    // Decal textures are white where they leave the surface unchanged, so they
    // multiply what's beneath them, faded by alpha and the box height.
    ALBEDO = mix(vec3(1.0), color.rgb, color.a * height);
}
