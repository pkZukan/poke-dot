shader_type spatial;
render_mode cull_back, depth_draw_opaque, diffuse_lambert_wrap;

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

void vertex() {
    //
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	
	vec4 layerMask = texture(LayerMaskMap, uvs);
	layerMask = mix(vec4(0.0), layerMask, vec4(1.0));

	//Albedo
	vec3 base_col = vec3(1, 1, 1);
	if (EnableBaseColorMap)
	{
		vec3 baseColMap = texture(BaseColorMap, uvs).rgb;
		base_col.rgb = mix(base_col.rgb, BaseColorLayer1.rgb * baseColMap, layerMask.r);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer2.rgb * baseColMap, layerMask.g);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer3.rgb * baseColMap, layerMask.b);
		base_col.rgb = mix(base_col.rgb, BaseColorLayer4.rgb * baseColMap, layerMask.a);
	}
	
	//AO
    float ao = 1.0;
    if (EnableAOMap)
        ao = pow(texture(OcclusionMap, uvs).r, max(OcclusionStrength, 0.001));

    //Normal
    vec3 norm = vec3(0.5, 0.5, 1.0); // flat normal default
    if (EnableNormalMap)
        norm = texture(NormalMap, uvs).rgb;

    //Specular/metallic/roughness
    float specular = 0.5;
    float metallic = 0.0;
    float roughness = 0.8;
    if (EnableHairSpecular) {
        vec4 spec_mask = texture(SpecularMaskMap, uvs);
        specular  = spec_mask.r ;
        roughness = 1.0 - spec_mask.g;
        metallic  = spec_mask.b;
    } else {
        float spec_mask = texture(SpecularMaskMap, uvs).r;
        specular  = spec_mask;
        roughness = 1.0 - spec_mask;
    }
		
	ALBEDO = base_col;
	NORMAL_MAP = norm;
    ROUGHNESS = roughness;
    SPECULAR = specular;
    METALLIC = metallic;
	AO = ao;
}