shader_type spatial;

uniform sampler2D BaseColorMap : source_color;

uniform bool EnableBaseColorMap;
uniform bool EnableAlphaTest;
uniform int NumMaterialLayer;
uniform int BillboardType;
uniform bool EnableOpacityFalloff;
uniform int LayerMaskSource;
uniform int NumDetailLayer;
uniform int NumRequiredUV;
uniform int LayerBlendType;
uniform bool EnableDepthFade;
uniform bool EnableVertexColor;

//Float
uniform float EmissionIntensity;

//Float4
uniform vec4 UVScaleOffset;
uniform vec4 EmissionColor;

void vertex() {
	//
}

vec2 xform_uvs(vec2 uvs, vec4 scale_off)
{
	vec2 v = scale_off.xy * (uvs - scale_off.zw);
	return vec2(v.x, 1.0 - v.y);
}

vec2 get_uv(vec2 uvs) 
{
    return xform_uvs(uvs, UVScaleOffset);
}

void fragment() {
	vec2 uvs = get_uv(UV);
	
	//Albedo
	vec4 base_col = vec4(1, 1, 1, 1);
	if (EnableBaseColorMap)
	{
		base_col = texture(BaseColorMap, uvs);
	}
	
	ALBEDO = base_col.rgb * COLOR.rgb;
	ALPHA = 1.0;
}
