shader_type spatial;

uniform sampler2D BaseColorMap  : source_color, hint_default_white;
uniform sampler2D NormalMap     : hint_normal;
uniform sampler2D OcclusionMap   : hint_default_black;
uniform sampler2D SpecularMaskMap  : hint_default_white;
uniform sampler2D ShadowingColorMap : hint_default_white;
uniform sampler2D ShadowingColorMaskMap         : hint_default_white;
uniform sampler2D RimLightMaskMap        : hint_default_white;
uniform sampler2D LocalReflectionMap        : hint_default_white;
uniform sampler2D LayerMaskMap  : hint_default_black;
uniform sampler2D ParallaxMap   : hint_default_black;
uniform sampler2D HighlightMaskMap : source_color, hint_default_black;
uniform sampler2D EyelidShadowMaskMap      : hint_default_white;

//Shader strings (as bool)
uniform bool EnableBaseColorMap;
uniform bool EnableNormalMap;
uniform bool EnableAOMap;
uniform bool EnableAlphaTest;
uniform bool BaseColorMultiply;
uniform bool EnableHairSpecular;
uniform bool EnableEmissionLayer;
uniform bool EnableLerpBaseColorEmission;
uniform bool EnableEyeOptions;
uniform bool EnableHighlight;
uniform bool EnableParallaxMap;
uniform bool RequireEyelidShadowMap;
uniform bool EnableUVScaleOffsetNormal;

//Shader strings (as int)
uniform int NumMaterialLayer;

uniform bool EnableSSSMaskMap;

//Float params
uniform float OcclusionStrength;

//Float4 params
uniform vec4 BaseColor;
uniform vec4 EmissionColor;
uniform vec4 UVCenter0;
uniform vec4 UVScaleOffset;
uniform vec4 UVScaleOffsetNormal;
uniform vec4 ShadowingColor;
uniform vec4 BaseColorLayer1;
uniform vec4 BaseColorLayer2;
uniform vec4 BaseColorLayer3;
uniform vec4 BaseColorLayer4;
uniform vec4 ShadowingColorLayer1;
uniform vec4 ShadowingColorLayer2;
uniform vec4 ShadowingColorLayer3;
uniform vec4 ShadowingColorLayer4;

//int params
uniform int CastShadow;
uniform int ReceiveShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableDistanceDither;
uniform int UVIndexAO;
uniform int EnableAdditionalLight;
uniform int UVIndexLayerMask;

void vertex() {
    //
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	
	vec4 layerMask = texture(LayerMaskMap, uvs);
	layerMask = mix(vec4(0.0), layerMask, vec4(1.0));

	vec3 base_col = vec3(1, 1, 1);
	if (EnableBaseColorMap)
	{
		vec3 baseColMap = texture(BaseColorMap, uvs).rgb;
		base_col.rgb = mix(base_col.rgb, BaseColorLayer1.rgb * baseColMap, layerMask.r);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer2.rgb * baseColMap, layerMask.g);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer3.rgb * baseColMap, layerMask.b);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer4.rgb * baseColMap, layerMask.a);
	}
	
	float ao = 1.0f;
	if(EnableAOMap)
		ao = pow(texture(OcclusionMap, uvs).r, OcclusionStrength);

	vec3 norm = vec3(1, 1, 1);
	if (EnableNormalMap)
		norm *= texture(NormalMap, uvs).rgb;
		
	ALBEDO = base_col;
	NORMAL_MAP = norm;
	AO = ao;
}