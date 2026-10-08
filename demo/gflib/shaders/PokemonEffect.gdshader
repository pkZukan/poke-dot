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
uniform float Metallic;
uniform float MetallicClearCoat;
uniform float NormalHeight;
uniform float NormalHeight1;
uniform float Roughness;
uniform float RoughnessClearCoat;
uniform float Thickness;

// Float4 uniforms
uniform vec4 BaseColorClearCoat;
uniform vec4 EmissionColor;
uniform vec4 EmissionFresnelColor;
uniform vec4 FresnelUVSpeed;

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;

// String shader parameters (metadata; not Godot uniforms)
// RefractionMode

void vertex()
{
	//
}

void fragment()
{
	ALBEDO = vec3(0);
}