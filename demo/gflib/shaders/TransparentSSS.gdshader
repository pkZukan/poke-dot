shader_type spatial;

// Float uniforms
uniform float EmissionIntensity;
uniform float EmissionIntensityLayer1;
uniform float EmissionIntensityLayer2;
uniform float EmissionIntensityLayer3;
uniform float EmissionIntensityLayer4;
uniform float LayerMaskScale1;
uniform float LayerMaskScale2;
uniform float LayerMaskScale3;
uniform float LayerMaskScale4;
uniform float NormalHeight;
uniform float Reflectance;
uniform float Roughness;
uniform float RoughnessLayer1;
uniform float RoughnessLayer2;
uniform float RoughnessLayer3;
uniform float RoughnessLayer4;
uniform float SSSMaskOffset;
uniform float SSSMaskScale;

// Float4 uniforms
uniform vec4 BaseColor;
uniform vec4 BaseColorLayer1;
uniform vec4 BaseColorLayer2;
uniform vec4 BaseColorLayer3;
uniform vec4 BaseColorLayer4;
uniform vec4 EmissionColor;
uniform vec4 EmissionColorLayer1;
uniform vec4 EmissionColorLayer2;
uniform vec4 EmissionColorLayer3;
uniform vec4 EmissionColorLayer4;
uniform vec4 SubsurfaceColor;
uniform vec4 SubsurfaceColorLayer1;
uniform vec4 SubsurfaceColorLayer2;
uniform vec4 SubsurfaceColorLayer3;
uniform vec4 SubsurfaceColorLayer4;
uniform vec4 UVScaleOffset;
uniform vec4 UVScaleOffsetNormal;

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableAdditionalLight;
uniform int EnableGlobalWeather;
uniform int EnableJewel;
uniform int ReceiveShadow;

// String shader parameters (metadata; not Godot uniforms)
// EnableAOMap
// EnableAlphaTest
// EnableBaseColorMap
// EnableEmissionColorMap
// EnableMetallicMap
// EnableNormalMap
// EnableParallaxMap
// EnableRoughnessMap
// NumMaterialLayer

void vertex()
{
	//
}

void fragment()
{
	ALBEDO = vec3(0);
}