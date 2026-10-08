shader_type spatial;

// Float uniforms
uniform float AOIntensity;
uniform float AOIntensityFresnel;
uniform float EmissionIntensity;
uniform float EmissionIntensityLayer1;
uniform float EmissionIntensityLayer2;
uniform float EmissionIntensityLayer3;
uniform float EmissionIntensityLayer4;
uniform float EmissionIntensityLayer5;
uniform float FresnelAlphaMax;
uniform float FresnelAlphaMin;
uniform float FresnelAngleBias;
uniform float FresnelPower;
uniform float LayerMaskScale1;
uniform float LayerMaskScale2;
uniform float LayerMaskScale3;
uniform float LayerMaskScale4;
uniform float MetallicLayer1;
uniform float MetallicLayer2;
uniform float MetallicLayer3;
uniform float MetallicLayer4;
uniform float MetallicLayer5;
uniform float NormalHeight;
uniform float Reflectance;
uniform float RoughnessLayer1;
uniform float RoughnessLayer2;
uniform float RoughnessLayer3;
uniform float RoughnessLayer4;
uniform float RoughnessLayer5;
uniform float SSSMaskOffset;

// Float4 uniforms
uniform vec4 BaseColor;
uniform vec4 BaseColorLayer1;
uniform vec4 BaseColorLayer2;
uniform vec4 BaseColorLayer3;
uniform vec4 BaseColorLayer4;
uniform vec4 BaseColorLayer5;
uniform vec4 BaseColorLayer6;
uniform vec4 BaseColorLayer7;
uniform vec4 BaseColorLayer8;
uniform vec4 BaseColorLayer9;
uniform vec4 EmissionColor;
uniform vec4 EmissionColorLayer1;
uniform vec4 EmissionColorLayer2;
uniform vec4 EmissionColorLayer3;
uniform vec4 EmissionColorLayer4;
uniform vec4 EmissionColorLayer5;
uniform vec4 UVScaleOffset;
uniform vec4 UVScaleOffsetNormal;

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableJewel;
uniform int ReceiveShadow;

// String shader parameters (metadata; not Godot uniforms)
// EnableAOMap
// EnableBaseColorMap
// EnableEmissionColorMap
// EnableMetallicMap
// EnableNormalMap
// EnableRoughnessMap
// EnableVertexColor
// NumMaterialLayer

void vertex()
{
	//
}

void fragment()
{
	ALBEDO = vec3(0);
}