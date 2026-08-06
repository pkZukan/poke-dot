shader_type spatial;

uniform sampler2D FlowMap;
uniform sampler2D FoamMaskMap;
uniform sampler2D NormalMap : hint_normal;
uniform sampler2D NormalMap1 : hint_normal;

uniform bool EnableBaseColorMap;
uniform bool EnableBaseColorMap1;
uniform bool EnableNormalMap;
uniform bool EnableNormalMap1;
uniform bool EnableFlowMap;
uniform bool EnableParallaxMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableEmissionColorMap;
uniform bool EnableVertexFoamMask;
uniform bool EnableAlphaTest;
uniform bool EnableVertexAnimationMask;
uniform int NumMaterialLayer;
uniform int BillboardType;
uniform int WindReceiverType;
uniform int LayerMaskSource;
uniform int LayerMaskSwizzle;
uniform int LayerBaseMaskSource;
uniform bool EnableVertexBaseColor;
uniform int NumDetailLayer;
uniform bool EnableDepthFade;
uniform bool ReceiveShadow;
uniform bool EnableDeferredRendering;
uniform int InstancingType;
uniform bool EnableGrassCollisionMap;
uniform int NumRequiredUV;
uniform bool EnableLerpBaseColorEmission;
uniform int LayerBlendType;
uniform bool EnableTriplanar;
uniform bool EnablePackedMap;
uniform bool EnableDisplacementMap;

//Float
uniform float EmissionIntensity;

//Float4
uniform vec4 BaseColor;
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
	vec4 albedo = BaseColor;

	//FlowMap
	vec3 flowMap = vec3(1);
	vec3 flowMask = vec3(1);
	if(EnableFlowMap)
	{
		flowMap = texture(FlowMap, uvs).rgb;
		flowMask = texture(FoamMaskMap, uvs).rgb;
	}

	//Normal
	vec3 norm = vec3(1,1,1);
	if(EnableNormalMap)
		norm *= texture(NormalMap, uvs).rgb;
	if(EnableNormalMap1)
		norm *= texture(NormalMap1, uvs).rgb;
	
	ALBEDO = albedo.rgb;
	ALPHA = albedo.a;
	NORMAL_MAP = norm;
}
