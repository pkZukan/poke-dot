shader_type canvas_item;
render_mode unshaded;

// BNTX channel sources: zero, one, red, green, blue, alpha.
// Remap after sampling so BC4/BC5 can stay compressed on the GPU.
uniform ivec4 channel_sources = ivec4(2, 3, 4, 5);
uniform vec4 black_color = vec4(0.0);
uniform vec4 white_color = vec4(1.0);
uniform vec2 uv_translation = vec2(0.0);
uniform vec2 uv_scale = vec2(1.0);
uniform float uv_rotation = 0.0;
varying vec4 pane_color;

void vertex() {
	pane_color = COLOR;
}

float channel_value(vec4 texel, int source) {
	if (source == 0) return 0.0;
	if (source == 1) return 1.0;
	if (source == 2) return texel.r;
	if (source == 3) return texel.g;
	if (source == 4) return texel.b;
	return texel.a;
}

void fragment() {
	vec2 uv = (UV - vec2(0.5)) * uv_scale;
	float c = cos(uv_rotation), s = sin(uv_rotation);
	uv = mat2(vec2(c, s), vec2(-s, c)) * uv + vec2(0.5) + uv_translation;
	vec4 texel = texture(TEXTURE, uv);
	vec4 mapped = vec4(
		channel_value(texel, channel_sources.x),
		channel_value(texel, channel_sources.y),
		channel_value(texel, channel_sources.z),
		channel_value(texel, channel_sources.w));
	COLOR = (black_color + mapped * (white_color - black_color)) * pane_color;
}
