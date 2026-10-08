shader_type spatial;

// Float uniforms
uniform float DiscardValue;
uniform float EmissionIntensity;
uniform float EmissionIntensityLayer1;
uniform float EmissionIntensityLayer2;
uniform float EmissionIntensityLayer3;
uniform float EmissionIntensityLayer4;
uniform float IridescenceAOBlend;
uniform float IridescenceBlend;
uniform float IridescenceBlend1;
uniform float IridescenceBlend2;
uniform float IridescenceBlend3;
uniform float IridescenceBlend4;
uniform float IridescenceCenter;
uniform float IridescenceEnd;
uniform float IridescenceFresnel;
uniform float IridescencePower;
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
uniform vec4 BaseColorLayer1;
uniform vec4 BaseColorLayer2;
uniform vec4 BaseColorLayer3;
uniform vec4 BaseColorLayer4;
uniform vec4 EmissionColor;
uniform vec4 EmissionColorLayer1;
uniform vec4 EmissionColorLayer2;
uniform vec4 EmissionColorLayer3;
uniform vec4 EmissionColorLayer4;
uniform vec4 IridescenceColor1;
uniform vec4 IridescenceColor2;
uniform vec4 IridescenceColor3;
uniform vec4 UVScaleOffset;
uniform vec4 UVScaleOffsetNormal;

// Int uniforms
uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableJewel;
uniform int ReceiveShadow;
uniform int UVIndexAO;
uniform int UVIndexLayerMask;

// String shader parameters (metadata; not Godot uniforms)
// EnableAOMap
// EnableAlphaTest
// EnableBaseColorMap
// EnableEmissionColorMap
// EnableLerpBaseColorEmission
// EnableMetallicMap
// EnableNormalMap
// EnableParallaxMap
// EnableRoughnessMap
// EnableUVScaleOffsetNormal
// EnableVertexBaseColor
// IridescenceBlendType
// IridescenceInterpolateType
// IridescenceType
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