shader_type spatial;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D NormalMap : hint_normal;
uniform sampler2D PackedMap;
uniform sampler2D EmissionColorMap;
uniform sampler2D MetallicMap;
uniform sampler2D RoughnessMap;
uniform sampler2D AOMap;
uniform sampler2D WindowGradientMap;

uniform bool EnableBaseColorMap;
uniform bool EnableNormalMap;
uniform bool EnableParallaxMap;
uniform bool EnableMetallicMap;
uniform bool EnableRoughnessMap;
uniform bool EnableEmissionColorMap;
uniform bool EnableAOMap;
uniform bool EnableAlphaTest;
uniform bool EnablePackedMap;
uniform bool EnableVertexBaseColor;

uniform int CastShadow;
uniform int ReceiveShadow;
uniform int CategoryLabel;
uniform int EnableGlobalWeather;
uniform int EnableDistanceDither;

//Float
uniform float EmissionIntensity;
uniform float VertexColorIntensity;

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
	vec2 base_uv = get_uv(UV);
	vec2 uvs = base_uv;

	//Parallax (no dedicated height map uniform, so PackedMap.a is assumed to store height)
	if (EnableParallaxMap) {
		vec3 view_dir = normalize(normalize(-VERTEX) * mat3(TANGENT, BINORMAL, NORMAL));
		float height = texture(PackedMap, base_uv).a;
		uvs = base_uv + view_dir.xy * (height - 0.5) * 0.04;
	}
	
	//Albedo
	vec4 base_col = vec4(1);
	base_col = texture(BaseColorMap, uvs);
	
	//Normal
	vec4 norm = vec4(1);
	if(EnableNormalMap)
		norm *= texture(NormalMap, uvs);
	
	//Packed
	vec4 packed = vec4(1);
	if(EnablePackedMap)
		packed *= texture(PackedMap, uvs);
	
	//Emission
	vec3 emission = vec3(1);
	if(EnableEmissionColorMap)
		emission *= (texture(EmissionColorMap, uvs).rgb * EmissionIntensity);

	// Window gradient acts as an emission mask, faking randomly-lit windows on a building facade
	float window_mask = texture(WindowGradientMap, uvs).r;
	emission *= window_mask * EmissionColor.rgb;

	//Metallic
	float metallic = 0.0;
    if (EnableMetallicMap) {
        metallic = texture(MetallicMap, uvs).r;
    }
	
	//AO
    float ao = 1.0;
    if (EnableAOMap)
        ao *= pow(texture(AOMap, uvs).r, 0.001);

	//Roughness
	float roughness = 1.0;
	if (EnableRoughnessMap)
		roughness = texture(RoughnessMap, uvs).r;

	float specular = 0.5;
	
	ALBEDO = base_col.rgb;
	if (EnableVertexBaseColor)
		ALBEDO *= COLOR.rgb * VertexColorIntensity;

	NORMAL_MAP = norm.rgb;
	SPECULAR = specular;
	EMISSION = emission;
	METALLIC = metallic;
	ROUGHNESS = roughness;
	AO = ao;

	//Alpha test / distance dither
	if (EnableAlphaTest) {
		ALPHA = base_col.a;
		ALPHA_SCISSOR_THRESHOLD = 0.5;
	}
	if (EnableDistanceDither > 0) {
		float dither = fract(sin(dot(FRAGCOORD.xy, vec2(12.9898, 78.233))) * 43758.5453);
		ALPHA = base_col.a;
		ALPHA_SCISSOR_THRESHOLD = dither;
	}
}
