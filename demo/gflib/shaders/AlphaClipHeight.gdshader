shader_type spatial;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D NormalMap;
uniform sampler2D RoughnessMap;

uniform float NormalIntensity;
uniform float RoughnessIntensity;
uniform float Metallic;
uniform float VertexColorContrast;
uniform float HeightContrast;
uniform float DiscardValue;

uniform vec4 UVScaleOffset;

uniform int CastShadow;
uniform int ReceiveShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableDistanceDither;

void vertex() 
{
	//
}

void fragment() 
{
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	vec4 albedo = texture(BaseColorMap, uvs);
	ALBEDO = albedo.rgb;
	ALPHA = albedo.a;
	NORMAL_MAP = texture(NormalMap, uvs).rgb;
}