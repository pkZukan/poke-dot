shader_type spatial;
render_mode unshaded, blend_mix, cull_disabled;

uniform sampler2D BaseColorMap : source_color, hint_default_white, repeat_enable;

uniform int sampler_repeat_u[1];
uniform int sampler_repeat_v[1];
uniform vec4 sampler_border_color[1];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_refraction.gdshaderinc"

uniform bool EnableBaseColorMap = true;
uniform bool EnableVertexColor;
uniform bool EnableDepthFade;
uniform bool EnableOpacityFalloff;
uniform vec4 BaseColor = vec4(1.0);
uniform vec4 UVScaleOffset = vec4(1.0, 1.0, 0.0, 0.0);
uniform vec4 UVScaleOffset1 = vec4(1.0, 1.0, 0.0, 0.0);
uniform float DepthFadeDistance = 1.0;
uniform float NearLength;
uniform float FarLength = 100.0;
uniform float OpacityNear = 1.0;
uniform float OpacityFar = 1.0;


void fragment() {
    vec4 color = BaseColor;
    if (EnableBaseColorMap) {
        color.rgb = sample_trinity(BaseColorMap, material_uv(UV, UVScaleOffset), 0).rgb;
        color.a = sample_trinity(BaseColorMap, material_uv(UV, UVScaleOffset1), 0).a;
    }

    if (EnableVertexColor) 
        color *= COLOR;
    if (EnableDepthFade) 
        color.a *= trinity_depth_fade(SCREEN_UV, -VERTEX.z, DepthFadeDistance, INV_PROJECTION_MATRIX);
    if (EnableOpacityFalloff) 
        color.a *= mix(OpacityNear, OpacityFar, clamp((-VERTEX.z - NearLength) / max(FarLength - NearLength, 0.00001), 0.0, 1.0));

    ALBEDO = color.rgb;
    ALPHA = color.a;
}
