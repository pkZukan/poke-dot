shader_type spatial;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D LayerMaskMap;
uniform sampler2D DisplacementMap;

uniform bool EnableBaseColorMap;
uniform bool EnableAlphaTest;
uniform int NumMaterialLayer;
uniform bool EnableDisplacementMap;
uniform int NumRequiredUV;

void vertex() 
{
	//
}

void fragment() 
{
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	if(EnableBaseColorMap)
		ALBEDO = texture(BaseColorMap, uvs).rgb;
}
