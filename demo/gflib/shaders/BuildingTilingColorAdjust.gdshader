shader_type spatial;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D BaseColorMap1 : source_color;
uniform sampler2D NormalMap : hint_normal;
uniform sampler2D NormalMap1 : hint_normal;
uniform sampler2D MetallicMap;
uniform sampler2D RoughnessMap;
uniform sampler2D RoughnessMap1;
uniform sampler2D PackedMap;
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

//Float
uniform float EmissionIntensity;
uniform float VertexColorIntensity;
uniform float MetallicIntensity;

//Float4
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
	
	vec4 layerMask = texture(LayerMaskMap, uvs);
	layerMask = mix(vec4(0.0), layerMask, vec4(1.0));
	
	//Albedo
	vec4 base_col = vec4(1, 1, 1, 1);
	if (EnableBaseColorMap)
	{
		vec4 baseColMap = texture(BaseColorMap, uvs);
		base_col.rgb = mix(base_col.rgb, baseColMap.rgb, layerMask.r);
		base_col.rgb = mix(base_col.rgb, baseColMap.rgb, layerMask.g);
		base_col.rgb = mix(base_col.rgb, baseColMap.rgb, layerMask.b);
		base_col.rgb = mix(base_col.rgb, baseColMap.rgb, layerMask.a);
	}
		
	vec4 baseColMap1 = texture(BaseColorMap1, uvs);
	
	//Normal
	vec3 norm = vec3(1,1,1);
	if(EnableNormalMap)
		norm *= texture(NormalMap, uvs).rgb;
	if(EnableNormalMap1)
		norm *= texture(NormalMap1, uvs).rgb;
	
	//Packed map
	vec3 packed = vec3(1,1,1);
	packed *= texture(PackedMap, uvs).rgb;
		
	//Emission
	vec3 emission = texture(EmissionColorMap, uvs).rgb;
	emission *= EmissionIntensity;

	//Metallic
	float metallic = 0.0;
    if (EnableMetallicMap) {
        metallic = texture(MetallicMap, uvs).r;
		metallic *= MetallicIntensity;
    }
	
	//Roughness
	float roughness = 0.0;
	if(EnableRoughnessMap)
		roughness = mix(roughness, texture(RoughnessMap, uvs).r, layerMask.r);
	if(EnableRoughnessMap1)
		roughness = mix(roughness, texture(RoughnessMap1, uvs).r, layerMask.r);
		
	float specular = 0.5;
	
	ALBEDO = baseColMap1.rgb * packed;
	if(EnableVertexBaseColor)
		ALBEDO *= (COLOR.rgb * VertexColorIntensity);
	NORMAL_MAP = norm;
	METALLIC = metallic;
	ROUGHNESS = roughness;
	SPECULAR = specular;
	EMISSION = emission;
}
