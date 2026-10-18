shader_type canvas_item;

// Godot fixes the blend equation per shader, so Nintendo's alpha, additive and
// premultiplied blend factors are emulated through premultiplied alpha. UIBuilder
// swaps the render mode to blend_mul for multiply materials.
render_mode unshaded, blend_premul_alpha;

// BNTX component swizzle, which Godot textures lack: 0 zero, 1 one, 2-5 RGBA.
uniform ivec4 channel_sources = ivec4(2, 3, 4, 5);

uniform vec4 black_color = vec4(0.0);
uniform vec4 white_color = vec4(1.0);

uniform vec4 picture_params = vec4(0.0);

uniform vec2 uv_scale = vec2(1.0);

uniform bool has_texture1 = false;
uniform sampler2D texture1 : filter_linear, repeat_disable;
uniform ivec4 texture1_channel_sources = ivec4(2, 3, 4, 5);
uniform vec3 texture1_transform = vec3(0.0);
uniform vec2 texture1_scale = vec2(1.0);
uniform ivec2 texture1_wrap = ivec2(0);
uniform bool texture1_nearest = false;
uniform bool texture1_projected = false;
uniform vec2 texture1_projection_center = vec2(0.0);
uniform vec2 texture1_projection_size = vec2(1.0);
uniform vec2 texture1_projection_offset = vec2(0.0);

uniform ivec2 combiner_modes = ivec2(0, 1);

uniform bool has_shape = false;
uniform vec2 shape_size = vec2(1.0);
uniform int shape_flags = 0;
uniform vec4 shape_radii = vec4(0.0);
uniform vec4 shape_exponents = vec4(2.0);
uniform float stroke_size = 0.0;
uniform vec4 stroke_color = vec4(0.0);
uniform vec4 inner_shadow_color = vec4(0.0);
uniform vec3 inner_shadow_params = vec3(0.0);
uniform vec4 overlay_color = vec4(0.0);
uniform vec4 gradient_stops = vec4(0.0, 1.0, 1.0, 1.0);
uniform vec4 gradient_color0 = vec4(0.0);
uniform vec4 gradient_color1 = vec4(1.0);
uniform vec4 gradient_color2 = vec4(1.0);
uniform vec4 gradient_color3 = vec4(1.0);
uniform float gradient_angle = 0.0;

varying vec4 pane_color;
varying vec2 local_position;

float wrap_coordinate(float value, int mode) {
    if (mode == 1) return fract(value);
    if (mode == 2) return 1.0 - abs(mod(value, 2.0) - 1.0);
    return clamp(value, 0.0, 1.0);
}

void vertex()
{
    pane_color = COLOR;
    local_position = VERTEX;
}

float select_corner(vec4 values, vec2 centered)
{
    return centered.y < 0.0 ? (centered.x < 0.0 ? values.x : values.y)
            : (centered.x < 0.0 ? values.z : values.w);
}

float shape_distance(vec2 position)
{
    vec2 half_size = shape_size * 0.5;
    vec2 centered = position - half_size;
    float radius = clamp(select_corner(shape_radii, centered), 0.0, min(half_size.x, half_size.y));
    float exponent = max(select_corner(shape_exponents, centered), 0.1);
    vec2 q = abs(centered) - half_size + vec2(radius);
    vec2 outside = max(q, vec2(0.0));
    float corner = radius > 0.0
            ? pow(pow(outside.x, exponent) + pow(outside.y, exponent), 1.0 / exponent)
            : length(outside);
    return corner + min(max(q.x, q.y), 0.0) - radius;
}

vec4 gradient_at(vec2 position)
{
    // Nintendo angles are y-up; Godot's canvas is y-down.
    vec2 direction = vec2(cos(gradient_angle), -sin(gradient_angle));
    float extent = abs(direction.x) * shape_size.x + abs(direction.y) * shape_size.y;
    float t = clamp(dot(position - shape_size * 0.5, direction) / max(extent, 1e-4) + 0.5, 0.0, 1.0);
    vec4 color = gradient_color0;
    color = mix(color, gradient_color1, clamp((t - gradient_stops.x) / max(gradient_stops.y - gradient_stops.x, 1e-4), 0.0, 1.0));
    color = mix(color, gradient_color2, clamp((t - gradient_stops.y) / max(gradient_stops.z - gradient_stops.y, 1e-4), 0.0, 1.0));
    color = mix(color, gradient_color3, clamp((t - gradient_stops.z) / max(gradient_stops.w - gradient_stops.z, 1e-4), 0.0, 1.0));
    return color;
}

vec4 blend_over(vec4 source, vec4 destination)
{
    float alpha = source.a + destination.a * (1.0 - source.a);
    vec3 color = source.rgb * source.a + destination.rgb * destination.a * (1.0 - source.a);
    return vec4(color / max(alpha, 1e-5), alpha);
}

vec4 apply_shape(vec4 surface)
{
    float distance = shape_distance(local_position);
    float edge = max(fwidth(distance), 1e-4);
    float coverage = clamp(0.5 - distance / edge, 0.0, 1.0);
    float layer_alpha = surface.a;
    vec4 layer = vec4(surface.rgb, (shape_flags & 0x80) != 0 ? 0.0 : surface.a);

    if ((shape_flags & 0x08) != 0) {
        vec4 gradient = gradient_at(local_position);
        layer = blend_over(vec4(gradient.rgb, gradient.a * layer_alpha), layer);
    }
    if ((shape_flags & 0x04) != 0)
        layer = blend_over(vec4(overlay_color.rgb, overlay_color.a * layer_alpha), layer);
    if ((shape_flags & 0x02) != 0) {
        // Offset away from the y-up light angle, flipped for Godot's y-down canvas.
        float angle = inner_shadow_params.x;
        vec2 offset = inner_shadow_params.y * vec2(-cos(angle), sin(angle));
        float shifted = shape_distance(local_position - offset);
        float size = inner_shadow_params.z;
        float shadow = size > 0.0 ? smoothstep(-size, 0.0, shifted) : step(0.0, shifted);
        layer = blend_over(vec4(inner_shadow_color.rgb, inner_shadow_color.a * shadow * layer_alpha), layer);
    }
    if ((shape_flags & 0x01) != 0) {
        float stroke = clamp((distance + stroke_size) / edge + 0.5, 0.0, 1.0);
        layer = blend_over(vec4(stroke_color.rgb, stroke_color.a * stroke * layer_alpha), layer);
    }

    layer.a *= coverage;
    return layer;
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

vec4 map_channels(vec4 texel, ivec4 sources)
{
    return vec4(
        channel_value(texel, sources.x),
        channel_value(texel, sources.y),
        channel_value(texel, sources.z),
        channel_value(texel, sources.w)
    );
}

vec2 transform_uv(vec2 uv, vec2 scale, vec3 transform)
{
    uv = (uv - vec2(0.5)) * scale;
    float c = cos(transform.z);
    float s = sin(transform.z);
    uv = mat2(vec2(c, s), vec2(-s, c)) * uv;
    return uv + vec2(0.5) + transform.xy;
}

vec4 combine(vec4 previous, vec4 texel, int mode)
{
    switch (mode) {
        case 1:
            return previous * texel;
        case 2:
            return previous + texel;
        case 3:
            return previous + texel - vec4(0.5);
        case 5:
            return previous - texel;
        case 8:
            return mix(2.0 * previous * texel,
                    vec4(1.0) - 2.0 * (vec4(1.0) - previous) * (vec4(1.0) - texel),
                    step(vec4(0.5), previous));
        default:
            return previous;
    }
}

void fragment()
{
    int blend_mode = int(picture_params.w + 0.5);

    vec2 uv = transform_uv(UV, uv_scale, vec3(picture_params.xyz));
    vec4 mapped = map_channels(texture(TEXTURE, uv), channel_sources);

    if (has_texture1) {
        // A Godot sampler's wrap and filter are fixed by its uniform hint, so the
        // material's wrap and nearest filtering for this texture are emulated.
        // Nintendo's pane projection runs opposite to Godot's local x and y axes
        // (inferred from the research gauge's authored keyframes).
        vec2 base1 = texture1_projected
                ? vec2(0.5) - (local_position - texture1_projection_center) / texture1_projection_size - texture1_projection_offset
                : UV;
        vec2 uv1 = transform_uv(base1, texture1_scale, texture1_transform);
        uv1 = vec2(wrap_coordinate(uv1.x, texture1_wrap.x), wrap_coordinate(uv1.y, texture1_wrap.y));
        if (texture1_nearest) {
            vec2 texture1_size = vec2(textureSize(texture1, 0));
            uv1 = (floor(uv1 * texture1_size) + vec2(0.5)) / texture1_size;
        }
        vec4 texel1 = map_channels(texture(texture1, uv1), texture1_channel_sources);
        mapped.rgb = combine(mapped, texel1, combiner_modes.x).rgb;
        mapped.a = combine(mapped, texel1, combiner_modes.y).a;
        mapped = clamp(mapped, 0.0, 1.0);
    }

    vec4 surface = (black_color + mapped * (white_color - black_color)) * pane_color;

    if (has_shape)
        surface = apply_shape(surface);

    if (blend_mode == 4) {
        // Godot's blend_mul also multiplies destination alpha; Nintendo's multiply only touches color.
        surface.a = 1.0;
    } else {
        if (blend_mode != 2 && blend_mode != 3)
            surface.rgb *= surface.a;

        // Zero alpha turns premultiplied blending into additive.
        if (blend_mode == 1 || blend_mode == 2)
            surface.a = 0.0;
    }

    COLOR = surface;
}
