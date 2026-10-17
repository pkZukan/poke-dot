shader_type canvas_item;

// Premultiplied blending also lets Nintendo additive materials use this
// shared Godot UI shader.
render_mode unshaded, blend_premul_alpha;

// BNTX channel sources:
// zero, one, red, green, blue, alpha.
uniform ivec4 channel_sources = ivec4(2, 3, 4, 5);

uniform vec4 black_color = vec4(0.0);
uniform vec4 white_color = vec4(1.0);

// x/y = translation
// z   = rotation in radians
// w   = additive blend flag
uniform vec4 picture_params = vec4(0.0);

uniform vec2 uv_scale = vec2(1.0);

// Optional second-texture coverage stage. Keep its UV transform and channel
// mapping separate: single-channel and RG BNTX textures have different alpha.
uniform bool has_alpha_mask = false;
uniform sampler2D alpha_mask : filter_linear, repeat_disable;
uniform ivec4 mask_channel_sources = ivec4(2, 3, 4, 5);
uniform vec3 mask_transform = vec3(0.0);
uniform vec2 mask_scale = vec2(1.0);
uniform ivec2 mask_wrap = ivec2(0);
uniform bool mask_nearest = false;

varying vec4 pane_color;

float wrap_coordinate(float value, int mode) {
    if (mode == 1) return fract(value);
    if (mode == 2) return 1.0 - abs(mod(value, 2.0) - 1.0);
    return clamp(value, 0.0, 1.0);
}

void vertex() 
{
    pane_color = COLOR;
}

float channel_value(vec4 texel, int source) 
{
    switch (source) {
        case 0:
            return 0.0;
        case 1:
            return 1.0;
        case 2:
            return texel.r;
        case 3:
            return texel.g;
        case 4:
            return texel.b;
        default:
            return texel.a;
    }
}

void fragment() 
{
    vec2 uv_translation = picture_params.xy;
    float uv_rotation = picture_params.z;
    bool additive_blend = picture_params.w > 0.5;

    vec2 uv = (UV - vec2(0.5)) * uv_scale;

    float c = cos(uv_rotation);
    float s = sin(uv_rotation);

    uv = mat2(vec2(c, s), vec2(-s, c)) * uv;
    uv += vec2(0.5) + uv_translation;

    vec4 texel = texture(TEXTURE, uv);
    vec4 mapped = vec4(
        channel_value(texel, channel_sources.x),
        channel_value(texel, channel_sources.y),
        channel_value(texel, channel_sources.z),
        channel_value(texel, channel_sources.w)
    );

    if (has_alpha_mask) {
        vec2 mask_uv = (UV - vec2(0.5)) * mask_scale;
        float mc = cos(mask_transform.z);
        float ms = sin(mask_transform.z);
        mask_uv = mat2(vec2(mc, ms), vec2(-ms, mc)) * mask_uv;
        mask_uv += vec2(0.5) + mask_transform.xy;
        mask_uv = vec2(wrap_coordinate(mask_uv.x, mask_wrap.x), wrap_coordinate(mask_uv.y, mask_wrap.y));
        if (mask_nearest) {
            vec2 mask_size = vec2(textureSize(alpha_mask, 0));
            mask_uv = (floor(mask_uv * mask_size) + vec2(0.5)) / mask_size;
        }
        mapped.a *= channel_value(texture(alpha_mask, mask_uv), mask_channel_sources.w);
    }

    vec4 surface = (black_color + mapped * (white_color - black_color)) * pane_color;

    surface.rgb *= surface.a;

    if (additive_blend)
        surface.a = 0.0;

    COLOR = surface;
}