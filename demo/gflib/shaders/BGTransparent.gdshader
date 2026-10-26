shader_type spatial;
render_mode unshaded, blend_premul_alpha, cull_disabled;

uniform sampler2D BaseColorMap : source_color, hint_default_white, filter_linear_mipmap_anisotropic, repeat_enable;

uniform int sampler_repeat_u[1];
uniform int sampler_repeat_v[1];
uniform vec4 sampler_border_color[1];
#include "trinity_sampler.gdshaderinc"
#include "trinity_material.gdshaderinc"
#include "trinity_refraction.gdshaderinc"
#include "trinity_blend.gdshaderinc"
#include "trinity_billboard.gdshaderinc"

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

void vertex() {
    if (BillboardType == 1) {
        MODELVIEW_MATRIX = trinity_billboard(MODELVIEW_MATRIX, MODEL_MATRIX, VIEW_MATRIX);
        MODELVIEW_NORMAL_MATRIX = mat3(MODELVIEW_MATRIX);
    }
}

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
    vec2 blend_state = trinity_blend(color.a, 1.0);
    PREMUL_ALPHA_FACTOR = blend_state.x;
    ALPHA = blend_state.y;
}
