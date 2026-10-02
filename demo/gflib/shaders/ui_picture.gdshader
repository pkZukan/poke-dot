shader_type canvas_item;
render_mode unshaded;

// BNTX channel sources: zero, one, red, green, blue, alpha.
// Remap after sampling so BC4/BC5 can stay compressed on the GPU.
uniform ivec4 channel_sources = ivec4(2, 3, 4, 5);
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
	vec4 texel = texture(TEXTURE, UV);
	vec4 mapped = vec4(
		channel_value(texel, channel_sources.x),
		channel_value(texel, channel_sources.y),
		channel_value(texel, channel_sources.z),
		channel_value(texel, channel_sources.w));
	COLOR = mapped * pane_color;
}
