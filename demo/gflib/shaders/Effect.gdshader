shader_type spatial;

// Float uniforms
uniform float DepthFadeDistance;
uniform float DiscardValue;
uniform float DissolveEdgeIntensity;
uniform float DissolveEdgeWidth;
uniform float EmissionIntensity;
uniform float FresnelAlphaMax;
uniform float FresnelAlphaMin;
uniform float Metallic;
uniform float NormalHeight;
uniform float RimIntensity;
uniform float RimPower;
uniform float Roughness;
uniform float UVRotation;

// Float4 uniforms
uniform vec4 BaseColor;
uniform vec4 BaseColorLayer1;
uniform vec4 DissolveEdgeColor;
uniform vec4 EmissionColor;
uniform vec4 RimColor;
uniform vec4 UVScaleOffset;

// Int uniforms
uniform int AlphaProcess;
uniform int CastShadow;
uniform int CategoryLabel;
uniform int ColorProcess;
uniform int EnableGlobalWeather;
uniform int ReceiveShadow;
uniform int Texture0AlphaChannel;
uniform int Texture0ColorChannel;
uniform int Texture1AlphaChannel;
uniform int Texture1AlphaOp;
uniform int Texture1ColorChannel;
uniform int Texture1ColorOp;
uniform int Texture2AlphaChannel;
uniform int Texture2AlphaOp;
uniform int Texture2ColorChannel;
uniform int Texture2ColorOp;
uniform int VertexAlphaChannel;
uniform int VertexAlphaOp;
uniform int VertexColorChannel;
uniform int VertexColorOp;

// String shader parameters (metadata; not Godot uniforms)
// EnableAlphaTest
// EnableBaseColorMap
// EnableBaseColorMap1
// EnableBaseColorMap2
// EnableDepthFade
// EnableEmissionColorMap
// EnableFlowMap
// EnableLighting
// EnableMetallicMap
// EnableNormalMap
// EnableRoughnessMap
// EnableVertexColor
// RefractionMode

void vertex()
{
	//
}

void fragment()
{
	ALBEDO = vec3(0);
}