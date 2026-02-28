shader_type spatial;
render_mode cull_front;

uniform sampler2D BaseColorMap  : source_color, hint_default_white;
uniform sampler2D BaseColorMap1 : source_color, hint_default_white;
uniform sampler2D NormalMap     : hint_normal;
uniform sampler2D NormalMap1    : hint_normal;
uniform sampler2D NormalMap2    : hint_normal;
uniform sampler2D MetallicMap   : hint_default_black;
uniform sampler2D RoughnessMap  : hint_default_white;
uniform sampler2D RoughnessMap1 : hint_default_white;
uniform sampler2D AOMap         : hint_default_white;
uniform sampler2D AOMap1        : hint_default_white;
uniform sampler2D AOMap2        : hint_default_white;
uniform sampler2D LayerMaskMap  : hint_default_black;
uniform sampler2D ParallaxMap   : hint_default_black;
uniform sampler2D EmissionColorMap : source_color, hint_default_black;
uniform sampler2D LightMap      : hint_default_white;
uniform sampler2D WindMaskMap   : hint_default_black;

uniform bool EnableBaseColorMap  = true;
uniform bool EnableNormalMap     = false;
uniform bool EnableParallaxMap   = false;
uniform bool EnableMetallicMap   = false;
uniform bool EnableRoughnessMap  = false;
uniform bool EnableEmissionColorMap = false;
uniform bool EnableAOMap         = false;
uniform bool EnableAlphaTest     = false;
uniform bool EnableLightMap      = false;
uniform bool EnableWindMaskMap   = false;
uniform bool EnableBaseColorMap1 = false;
uniform bool EnableNormalMap1    = false;
uniform bool EnableNormalMap2    = false;
uniform bool EnableRoughnessMap1 = false;
uniform bool EnableAOMap1        = false;
uniform bool EnableAOMap2        = false;
uniform bool EnableVertexBaseColor = false;
uniform bool EnableDepthFade     = false;
uniform bool ReceiveShadow       = true;
uniform bool EnableLerpBaseColorEmission = false;
uniform bool EnableTriplanar     = false;
uniform bool EnablePackedMap     = false;
uniform bool EnableDisplacementMap = false;

uniform int NumMaterialLayer    = 1;
uniform int BillboardType       = 0;
uniform int WindReceiverType    = 0;
uniform int LayerMaskSource     = 0;
uniform int LayerMaskSwizzle    = 0;
uniform int LayerBaseMaskSource = 0;
uniform int NumDetailLayer      = 0;
uniform int InstancingType      = 0;
uniform int NumRequiredUV       = 1;
uniform int LayerBlendType      = 0;

void vertex() {
	//
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip

	float ao_factor = 1.0;
	if (EnableAOMap) ao_factor = texture(AOMap, uvs).r;

	vec3 col = vec3(1, 1, 1);
	if (EnableBaseColorMap)
		col *= texture(BaseColorMap, uvs).rgb;
	if (EnableBaseColorMap1)
		col *= texture(BaseColorMap1, uvs).rgb;
	if (EnableVertexBaseColor)
		col *= COLOR.rgb;
	ALBEDO = col * ao_factor;

	vec3 norm = vec3(1, 1, 1);
	if (EnableNormalMap)
		norm *= texture(NormalMap, uvs).rgb;
	if (EnableNormalMap1)
		norm *= texture(NormalMap1, uvs).rgb;
	if (EnableNormalMap2)
		norm *= texture(NormalMap2, uvs).rgb;
	NORMAL_MAP = norm;
}