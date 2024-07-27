shader_type spatial;

render_mode cull_front;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D BaseColorMap1 : source_color;
uniform sampler2D NormalMap : hint_normal;
uniform sampler2D NormalMap1 : hint_normal;
uniform sampler2D NormalMap2 : hint_normal;
uniform sampler2D MetallicMap;
uniform sampler2D RoughnessMap;
uniform sampler2D RoughnessMap1;
uniform sampler2D AOMap;
uniform sampler2D AOMap1;
uniform sampler2D AOMap2;
uniform sampler2D LayerMaskMap;
uniform sampler2D ParallaxMap;
uniform sampler2D EmissionColorMap;
uniform sampler2D LightMap;
uniform sampler2D WindMaskMap;

uniform bool EnableBaseColorMap;
uniform bool EnableNormalMap;
uniform bool EnableParallaxMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableEmissionColorMap;
uniform bool EnableAOMap;
uniform bool EnableAlphaTest;
uniform bool EnableLightMap;
uniform int NumMaterialLayer;
uniform int BillboardType;
uniform int WindReceiverType;
uniform bool EnableWindMaskMap;
uniform bool EnableBaseColorMap1;
uniform bool EnableNormalMap1;
uniform bool EnableNormalMap2;
uniform bool EnableRoughnessMap1;
uniform bool EnableAOMap1;
uniform bool EnableAOMap2;
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

void vertex() {
	//
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	float ao_factor = 1.0;
	if(EnableAOMap) ao_factor = texture(AOMap, uvs).r;
	
	vec3 col = vec3(1,1,1);
	if(EnableBaseColorMap)
		col *= texture(BaseColorMap, uvs).rgb;
	if(EnableBaseColorMap1)
		col *= texture(BaseColorMap1, uvs).rgb;
	ALBEDO = col * ao_factor;
	
	vec3 norm = vec3(1,1,1);
	if(EnableNormalMap)
		norm *= texture(NormalMap, uvs).rgb;
	if(EnableNormalMap1)
		norm *= texture(NormalMap1, uvs).rgb;
	if(EnableNormalMap2)
		norm *= texture(NormalMap2, uvs).rgb;
	NORMAL_MAP = norm;
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
