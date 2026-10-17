shader_type spatial;
render_mode unshaded, shadows_disabled, fog_disabled, blend_premul_alpha, depth_draw_always;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;

uniform int sampler_repeat_u[1];
uniform int sampler_repeat_v[1];
uniform vec4 sampler_border_color[1];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"

uniform bool EnableAlphaTest;
uniform float DiscardValue = 0.5;

void fragment() {
    if (EnableAlphaTest && sample_trinity(BaseColorMap, vec2(UV.x, 1.0 - UV.y), 0).a < DiscardValue) discard;
    ALBEDO = vec3(0.0);
    ALPHA = 0.0;
}
