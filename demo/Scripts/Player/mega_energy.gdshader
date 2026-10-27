shader_type spatial;
render_mode unshaded, blend_add, depth_draw_never, cull_disabled, shadows_disabled, fog_disabled;

uniform float strength : hint_range(0.0, 1.0) = 0.0;
uniform vec4 energy_color : source_color = vec4(0.5, 0.25, 1.0, 1.0);

void fragment() {
    float rim = pow(1.0 - abs(dot(normalize(NORMAL), normalize(VIEW))), 1.5);
    float bands = pow(0.5 + 0.5 * sin(UV.y * 60.0 - TIME * 16.0 + sin(UV.x * 24.0) * 2.0), 6.0);
    ALBEDO = energy_color.rgb;
    EMISSION = mix(energy_color.rgb, vec3(1.0), bands * 0.6) * (2.0 + strength * 3.0);
    ALPHA = strength * (rim * 0.8 + bands * 0.4);
}
