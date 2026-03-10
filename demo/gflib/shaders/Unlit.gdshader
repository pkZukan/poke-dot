shader_type spatial;
render_mode unshaded, cull_back, depth_draw_opaque;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D LayerMaskMap;
uniform sampler2D DisplacementMap;

uniform bool EnableBaseColorMap;
uniform bool EnableAlphaTest;
uniform int NumMaterialLayer;
uniform bool EnableDisplacementMap;
uniform int NumRequiredUV;

//Hacky way to set sampler data because we cant access slots directly
uniform int sampler_repeat_u[3];
uniform int sampler_repeat_v[3];
const int UV_WRAP = 0;
const int UV_CLAMP = 1;
const int UV_MIRROR = 6;
const int UV_MIRROR_ONCE = 7;

//Float params
uniform float DisplacementHeight;
uniform float DiscardValue;
uniform float LayerMaskScale1;
uniform float LayerMaskScale2;
uniform float LayerMaskScale3;
uniform float LayerMaskScale4;
uniform float EmissionIntensity;

//Float4 params
uniform vec4 BaseColor;
uniform vec4 UVScaleOffset;
uniform vec4 BaseColorLayer1;
uniform vec4 BaseColorLayer2;
uniform vec4 BaseColorLayer3;
uniform vec4 BaseColorLayer4;
uniform vec4 UVScaleOffset3;

uniform int CastShadow;
uniform int CategoryLabel;
uniform int EnableJewel;
uniform int EnableGlobalWeather;
uniform int EnableDirt;
uniform int UVIndexLayer3;

void vertex() 
{
	//
}

vec2 get_uv(vec2 uvs, int slot) 
{
    vec2 result = uvs;
    
	//flip
	result.y = 1.0 - result.y;
	
    return result;
}

void fragment() 
{
	float displace_val = texture(DisplacementMap, get_uv(UV, 2)).r;
	float disp_signed  = (displace_val * 2.0 - 1.0) * DisplacementHeight;
	vec2 displaced_uv  = NORMAL.xy * disp_signed;
	
	vec2 baseCol_uv = get_uv(UV, 0) + displaced_uv;
	vec2 layerMsk_uv = get_uv(UV, 1) + displaced_uv;
	
	vec4 layerMask = texture(LayerMaskMap, layerMsk_uv);
	layerMask = mix(vec4(0.0), layerMask, vec4(1.0));
	
	//Albedo
	vec4 base_col = vec4(1, 1, 1, 1);
	if (EnableBaseColorMap)
	{
		vec4 baseColMap = texture(BaseColorMap, baseCol_uv);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer1.rgb * baseColMap.rgb, layerMask.r);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer2.rgb * baseColMap.rgb, layerMask.g);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer3.rgb * baseColMap.rgb, layerMask.b);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer4.rgb * baseColMap.rgb, layerMask.a);
	}
		
	ALBEDO = base_col.rgb;
	ALPHA = base_col.a;
	EMISSION = base_col.rgb * EmissionIntensity;
}
