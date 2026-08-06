shader_type spatial;

uniform sampler2D BaseColorMap1 : source_color;
uniform sampler2D NormalMap : hint_normal;
uniform sampler2D NormalMap1 : hint_normal;
uniform sampler2D PackedMap;
uniform sampler2D GrayAlbedoMap;
uniform sampler2D EmissionMaskMap;

uniform bool EnableVertexBaseColor;
uniform bool EnableVertexColor;
uniform bool NumMaterialLayer;
uniform bool EnableTilingBlend;
uniform bool EnableLightMap;
uniform bool EnableGrayAlbedoMap;
uniform bool EnableEmissionMaskMap;
uniform bool EnableAlphaTest;

//Float
uniform float EmissionIntensity;
uniform float VertexColorIntensity;
uniform float MetallicIntensity;

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
	vec4 baseColMap1 = texture(BaseColorMap1, uvs);
	
	//Normal
	vec3 norm = vec3(1,1,1);
		norm *= texture(NormalMap, uvs).rgb;
		norm *= texture(NormalMap1, uvs).rgb;
	
	//Packed map
	vec3 packed = texture(PackedMap, uvs).rgb;
		
	//Emission
	vec3 emission = texture(EmissionMaskMap, uvs).rgb;
	emission *= EmissionIntensity;
	
	float specular = 0.5;
	
	ALBEDO = baseColMap1.rgb * packed;
	if(EnableVertexBaseColor)
		ALBEDO *= (COLOR.rgb * VertexColorIntensity);
	NORMAL_MAP = norm;
	SPECULAR = specular;
	EMISSION = emission;
}
