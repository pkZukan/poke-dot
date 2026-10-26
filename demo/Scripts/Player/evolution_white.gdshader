shader_type spatial;
render_mode unshaded, blend_mix, depth_draw_never, cull_disabled, fog_disabled;

uniform float whiteness : hint_range(0.0, 1.0) = 0.0;

void fragment() {
	ALBEDO = vec3(1.0);
	ALPHA = whiteness;
}
