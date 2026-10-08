shader_type spatial;

// Float uniforms
uniform float EmissionColorFresnel;
uniform float EmissionIntensity;
uniform float EmissionMaskFresnel;
uniform float EmissionNormalHeight;
uniform float FresnelAlphaMax;
uniform float FresnelAlphaMin;
uniform float FresnelAngleBias;
uniform float IndexOfRefraction;
uniform float JewelBaseColorBrightness;
uniform float NormalHeight;
uniform float RefractionDepthBias;
uniform float Thickness;

// Float4 uniforms
uniform vec4 BaseColor;
uniform vec4 EmissionColor;
uniform vec4 EmissionFresnelColor;
uniform vec4 FresnelUVSpeed;
uniform vec4 UVScaleOffset;
uniform vec4 UVScaleOffsetEmissionNormal;

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int ReceiveShadow;

// String shader parameters (metadata; not Godot uniforms)
// EnableBaseColorMap
// EnableEmissionNormalMap
// EnablePackedMap
// RefractionMode

void vertex()
{
	//
}

void fragment()
{
	ALBEDO = vec3(0);
}