shader_type spatial;

// Float uniforms
uniform float BackRimLightIntensity;
uniform float BaseColorDarkness;
uniform float DarkAreaContrast;
uniform float DarkAreaHueOffset;
uniform float DarkAreaShift;
uniform float DiffusionLevels;
uniform float DiscardValue;
uniform float EmissionIntensity;
uniform float EmissionIntensityLayer1;
uniform float EmissionIntensityLayer2;
uniform float EmissionIntensityLayer3;
uniform float EmissionIntensityLayer4;
uniform float FresnelAlphaMax;
uniform float FresnelAlphaMin;
uniform float FresnelAngleBias;
uniform float HalfLambertBias;
uniform float HueShiftAreaValue;
uniform float HueShiftBias;
uniform float LayerMaskScale1;
uniform float LayerMaskScale2;
uniform float LayerMaskScale3;
uniform float LayerMaskScale4;
uniform float Metallic;
uniform float MetallicLayer1;
uniform float MetallicLayer2;
uniform float MetallicLayer3;
uniform float MetallicLayer4;
uniform float MidAreaContrast;
uniform float MidAreaHueOffset;
uniform float MidAreaShift;
uniform float NormalHeight;
uniform float OcclusionStrength;
uniform float ReflectionsBlur;
uniform float RimLightContrast;
uniform float RimLightIntensity;
uniform float RimLightOffset;
uniform float SaturationPower;
uniform float ShadingBias;
uniform float ShadowStrength;
uniform float ShadowingBias;
uniform float ShadowingColorMaskMapValue;
uniform float ShadowingContrast;
uniform float ShadowingGIGain;
uniform float ShadowingShift;
uniform float SpecularContrast;
uniform float SpecularIntensity;
uniform float SpecularLayer1Contrast;
uniform float SpecularLayer1Intensity;
uniform float SpecularLayer1Offset;
uniform float SpecularLayer2Contrast;
uniform float SpecularLayer2Intensity;
uniform float SpecularLayer2Offset;
uniform float SpecularLayer3Contrast;
uniform float SpecularLayer3Intensity;
uniform float SpecularLayer3Offset;
uniform float SpecularLayer4Contrast;
uniform float SpecularLayer4Intensity;
uniform float SpecularLayer4Offset;
uniform float SpecularOffset;

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
uniform vec4 ShadowingColor;
uniform vec4 ShadowingColorLayer1;
uniform vec4 ShadowingColorLayer2;
uniform vec4 ShadowingColorLayer3;
uniform vec4 ShadowingColorLayer4;
uniform vec4 UVScaleOffset;

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableAdditionalLight;
uniform int EnableDistanceDither;
uniform int EnableGlobalWeather;
uniform int ReceiveShadow;
uniform int UVIndexAO;
uniform int UVIndexLayerMask;

// String shader parameters (metadata; not Godot uniforms)
// BaseColorMultiply
// EnableAOMap
// EnableAlphaTest
// EnableBaseColorMap
// EnableEmissionLayer
// EnableHairSpecular
// EnableLerpBaseColorEmission
// EnableNormalMap
// NumMaterialLayer
// NumRequiredUV

void vertex()
{
	//
}

void fragment()
{
	ALBEDO = vec3(0);
}