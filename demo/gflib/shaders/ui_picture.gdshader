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

varying vec4 pane_color;

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

    vec4 surface = (black_color + mapped * (white_color - black_color)) * pane_color;

    surface.rgb *= surface.a;

    if (additive_blend)
        surface.a = 0.0;

    COLOR = surface;
}