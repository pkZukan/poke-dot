shader_type spatial;
render_mode unshaded;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;

uniform int sampler_repeat_u[1];
uniform int sampler_repeat_v[1];
uniform vec4 sampler_border_color[1];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"

uniform bool EnableAlphaTest;

uniform float DiscardValue = 0.5;

void vertex() {
    // Shadow-map proxy: it only exists in the shadow pass.
    if (IN_SHADOW_PASS)
        POSITION = PROJECTION_MATRIX * MODELVIEW_MATRIX * vec4(VERTEX, 1.0);
    else
        POSITION = vec4(0.0, 0.0, 0.0, 1.0);
}

void fragment() {
    if (EnableAlphaTest && sample_trinity(BaseColorMap, vec2(UV.x, 1.0 - UV.y), 0).a < DiscardValue) discard;
    ALBEDO = vec3(0.0);
}
