shader_type spatial;
render_mode cull_back;

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

//Hacky way to set sampler data because we cant access slots directly
uniform int sampler_repeat_u[12];
uniform int sampler_repeat_v[12];
const int UV_WRAP = 0;
const int UV_CLAMP = 1;
const int UV_MIRROR = 6;
const int UV_MIRROR_ONCE = 7;

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
uniform float SaturationPower;
uniform float BaseColorDarkness;
uniform float NormalHeight;
uniform float Metallic;
uniform float OcclusionStrength;
uniform float SpecularOffset;
uniform float SpecularIntensity;
uniform float SpecularContrast;
uniform float ShadowStrength;
uniform float EmissionIntensity;
uniform float DiscardValue;
uniform float ShadingBias;
uniform float ShadowingColorMaskMapValue;
uniform float HueShiftBias;
uniform float ShadowingBias;
uniform float HalfLambertBias;
uniform float ShadowingGIGain;
uniform float RimLightOffset;
uniform float RimLightContrast;
uniform float RimLightIntensity;
uniform float BackRimLightIntensity;
uniform float ReflectionsBlur;
uniform float DiffusionLevels;
uniform float LayerMaskScale1;
uniform float SpecularLayer1Intensity;
uniform float SpecularLayer1Offset;
uniform float SpecularLayer1Contrast;
uniform float MetallicLayer1;
uniform float LayerMaskScale2;
uniform float SpecularLayer2Intensity;
uniform float SpecularLayer2Offset;
uniform float SpecularLayer2Contrast;
uniform float MetallicLayer2;
uniform float LayerMaskScale3;
uniform float LayerMaskScale4;
uniform float SpecularLayer4Intensity;
uniform float SpecularLayer4Offset;
uniform float SpecularLayer4Contrast;
uniform float MetallicLayer4;
uniform float MidAreaShift;
uniform float MidAreaContrast;
uniform float MidAreaHueOffset;
uniform float DarkAreaShift;
uniform float DarkAreaContrast;
uniform float DarkAreaHueOffset;
uniform float HueShiftAreaValue;
uniform float ShadowingShift;
uniform float ShadowingContrast;
uniform float ParallaxHeight;
uniform float ParallaxIOR;
uniform float EmissionIntensityLayer5;
uniform float UVRotation2;

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

void vertex() 
{
    //
}

vec2 xform_uvs(vec2 uvs, vec4 scale_off, vec2 center)
{
	vec2 v = scale_off.xy * (uvs - scale_off.zw - center) + center;
	return vec2(v.x, 1.0 - v.y);
}

vec2 get_uv(vec2 uvs, int slot) 
{
    vec2 result = uvs;
    
    //Scale and flip
	result = xform_uvs(result, ((slot == 1 && EnableUVScaleOffsetNormal) ? UVScaleOffsetNormal : UVScaleOffset), UVCenter0.xy);

    // Process U wrap mode
    int wrap_u = sampler_repeat_u[slot];
    if (wrap_u == UV_WRAP) {
        result.x = fract(result.x);
    } else if (wrap_u == UV_CLAMP) {
        result.x = clamp(result.x, 0.0, 1.0);
    } else if (wrap_u == UV_MIRROR) {
        result.x = 1.0 - abs(mod(result.x, 2.0) - 1.0);
    }

    // Process V wrap mode
    int wrap_v = sampler_repeat_v[slot];
    if (wrap_v == UV_WRAP) {
        result.y = fract(result.y);
    } else if (wrap_v == UV_CLAMP) {
        result.y = clamp(result.y, 0.0, 1.0);
    } else if (wrap_v == UV_MIRROR) {
        result.y = 1.0 - abs(mod(result.y, 2.0) - 1.0);
    }
    
    return result;
}

void fragment() 
{	
	vec4 layerMask = texture(LayerMaskMap, get_uv(UV, 8));
	layerMask = mix(vec4(0.0), layerMask, vec4(1.0));

	//Albedo
	vec4 base_col = vec4(1, 1, 1, 1);
	if (EnableBaseColorMap)
	{
		vec4 baseColMap = texture(BaseColorMap, get_uv(UV, 0));
		base_col.rgb = mix(base_col.rgb, BaseColorLayer1.rgb * baseColMap.rgb, layerMask.r);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer2.rgb * baseColMap.rgb, layerMask.g);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer3.rgb * baseColMap.rgb, layerMask.b);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer4.rgb * baseColMap.rgb, layerMask.a);
	}
	
	//AO
    float ao = 1.0;
    if (EnableAOMap)
        ao = pow(texture(OcclusionMap, get_uv(UV, 2)).r, max(OcclusionStrength, 0.001));

    //Normal
    vec3 norm = vec3(0.5, 0.5, 1.0); // flat normal default
    if (EnableNormalMap)
        norm = texture(NormalMap, get_uv(UV, 1)).rgb;

    //Specular/metallic/roughness
    float specular = 0.5;
    float metallic = 0.0;
    float roughness = 0.8;
    if (EnableHairSpecular) {
        vec4 spec_mask = texture(SpecularMaskMap, get_uv(UV, 3));
        specular  = spec_mask.r ;
        roughness = 1.0 - spec_mask.g;
        metallic  = spec_mask.b;
    } else {
        float spec_mask = texture(SpecularMaskMap, get_uv(UV, 3)).r;
        specular  = spec_mask;
        roughness = 1.0 - spec_mask;
    }
		
	ALBEDO = base_col.rgb * COLOR.rgb;
	NORMAL_MAP = norm;
    ROUGHNESS = roughness;
    SPECULAR = specular;
    METALLIC = metallic;
	AO = ao;
}