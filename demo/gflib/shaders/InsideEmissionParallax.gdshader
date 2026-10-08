shader_type spatial;

// Float uniforms
uniform float DiscardValue;
uniform float EmissionIntensity;
uniform float EmissionIntensityLayer1;
uniform float EmissionIntensityLayer2;
uniform float EmissionIntensityLayer3;
uniform float EmissionIntensityLayer4;
uniform float EmissionRimCurve;
uniform float EmissionRimF0;
uniform float EmissionRimF90;
uniform float EmissionRimRate;
uniform float EmissionRimThreshold;
uniform float EnableIEPTexture1;
uniform float EnableIEPTexture2;
uniform float EnableIEPTexture3;
uniform float EnableIEPTexture4;
uniform float InsideEmissionParallaxF0;
uniform float InsideEmissionParallaxF90;
uniform float InsideEmissionParallaxHeight;
uniform float InsideEmissionParallaxIntensity;
uniform float InsideEmissionParallaxIntensityMapLatticeU;
uniform float InsideEmissionParallaxIntensityMapLatticeV;
uniform float InsideEmissionParallaxIntensityMapRangeAvoidEdge;
uniform float InsideEmissionParallaxOffset;
uniform float LayerMaskScale1;
uniform float LayerMaskScale2;
uniform float LayerMaskScale3;
uniform float LayerMaskScale4;
uniform float MetallicLayer1;
uniform float MetallicLayer2;
uniform float MetallicLayer3;
uniform float MetallicLayer4;
uniform float NormalHeight;
uniform float RoughnessLayer1;
uniform float RoughnessLayer2;
uniform float RoughnessLayer3;
uniform float RoughnessLayer4;

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
uniform vec4 EmissionRimColor;
uniform vec4 UVScaleOffset;
uniform vec4 UVScaleOffsetInsideEmissionParallaxHeight;
uniform vec4 UVScaleOffsetInsideEmissionParallaxIntensity;

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int UVIndexInsideEmissionParallaxHeight;
uniform int UVIndexInsideEmissionParallaxIntensity;
uniform int UVIndexLayerMask;

// String shader parameters (metadata; not Godot uniforms)
// EnableAOMap
// EnableAlphaTest
// EnableBaseColorMap
// EnableDeferredRendering
// EnableEmissionColorMap
// EnableLerpBaseColorEmission
// EnableMetallicMap
// EnableNormalMap
// EnableRoughnessMap
// EnableVertexBaseColor
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