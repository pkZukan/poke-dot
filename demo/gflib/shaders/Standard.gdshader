shader_type spatial;

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

//Float
uniform float EmissionIntensity;

//Float4
uniform vec4 UVScaleOffset;
uniform vec4 EmissionColor;


const float PARALLAX_DEPTH = 0.04;
const float WIND_AMPLITUDE = 0.025;

vec2 get_uv(vec2 uv) 
{
	vec2 transformed = UVScaleOffset.xy * (uv - UVScaleOffset.zw);
	return vec2(transformed.x, 1.0 - transformed.y);
}

void vertex() 
{
	if (WindReceiverType != 0) 
	{
		float mask = 1.0;
		if (EnableWindMaskMap)
			mask = textureLod(WindMaskMap, get_uv(UV), 0.0).r;
		
		vec3 world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
		float sway = sin(TIME * 1.7 + dot(world_position.xz, vec2(0.7, 0.5)));
		VERTEX.x += sway * mask * WIND_AMPLITUDE;
	}
}

vec3 decode_normal(vec3 encoded) 
{
	vec2 xy = encoded.xy * 2.0 - 1.0;
	return normalize(vec3(xy, sqrt(max(1.0 - dot(xy, xy), 0.0001))));
}

// Whiteout blending preserves the neutral (0, 0, 1) detail normal.
vec3 blend_detail_normal(vec3 base, vec3 detail) 
{
	return normalize(vec3(base.xy + detail.xy, base.z * detail.z));
}

void fragment() 
{
	vec2 uvs = get_uv(UV);
	if (EnableParallaxMap) {
		vec3 view_tangent = VIEW * mat3(normalize(TANGENT), normalize(BINORMAL), normalize(NORMAL));
		float height = texture(ParallaxMap, uvs).r - 0.5;
		// UVs above are vertically flipped, so flip the tangent-space Y offset too.
		uvs -= vec2(view_tangent.x, -view_tangent.y) * height * PARALLAX_DEPTH
			/ max(abs(view_tangent.z), 0.2);
	}

	// Port enum: source 0 = texture, 1 = vertex color. Single-layer materials
	// must not depend on an unbound LayerMaskMap (which defaults to white).
	vec4 mask = vec4(0.0);
	if (NumMaterialLayer > 1 || NumDetailLayer > 0) 
	{
		if (LayerMaskSource == 1) 
		{
			mask = COLOR;
		} 
		else 
		{
			mask = texture(LayerMaskMap, uvs);
		}
	}
	// Swizzle 0 = RGBA; 1..4 replicate R/G/B/A respectively.
	if (LayerMaskSwizzle == 1)
		mask = vec4(mask.r);
	else if (LayerMaskSwizzle == 2)
		mask = vec4(mask.g);
	else if (LayerMaskSwizzle == 3)
		mask = vec4(mask.b);
	else if (LayerMaskSwizzle == 4)
		mask = vec4(mask.a);
	mask = clamp(mask, vec4(0.0), vec4(1.0));
	float layer_weight = NumMaterialLayer > 1 ? mask.r : 0.0;

	vec4 base = vec4(1.0);
	if (EnableBaseColorMap) {
		base = texture(BaseColorMap, uvs);
	}
	// Base-mask source 0 = One, 1 = base texture alpha.
	if (LayerBaseMaskSource == 1) 
		layer_weight *= base.a; 
	if (EnableBaseColorMap1 && NumMaterialLayer > 1) 
	{
		vec4 layer = texture(BaseColorMap1, uvs);
		// Blend 0 = Mix, 1 = Add, 2 = Multiply.
		if (LayerBlendType == 1) { base.rgb += layer.rgb * layer_weight; }
		else if (LayerBlendType == 2) { base.rgb *= mix(vec3(1.0), layer.rgb, layer_weight); }
		else { base = mix(base, layer, layer_weight); }
	}
	if (EnableVertexBaseColor) 
	{
		base *= COLOR;
	}
	if (EnableAlphaTest && base.a < 0.5) 
	{
		discard;
	}

	vec3 normal = vec3(0.0, 0.0, 1.0);
	if (EnableNormalMap)
		normal = decode_normal(texture(NormalMap, uvs).rgb); 
		
	if (EnableNormalMap1) 
	{
		vec3 layer_normal = decode_normal(texture(NormalMap1, uvs).rgb);
		if (NumDetailLayer > 0) 
		{
			normal = blend_detail_normal(normal, normalize(mix(vec3(0.0, 0.0, 1.0), layer_normal, mask.r)));
		} 
		else if (NumMaterialLayer > 1) 
		{
			normal = normalize(mix(normal, layer_normal, layer_weight));
		}
	}
	if (EnableNormalMap2) {
		vec3 layer_normal = decode_normal(texture(NormalMap2, uvs).rgb);
		if (NumDetailLayer > 1)
		{
			normal = blend_detail_normal(normal, normalize(mix(vec3(0.0, 0.0, 1.0), layer_normal, mask.g)));
		} 
		else if (NumMaterialLayer > 2) 
		{
			normal = normalize(mix(normal, layer_normal, mask.g));
		}
	}

	float metallic = 0.0;
	if (EnableMetallicMap)
		metallic = texture(MetallicMap, uvs).r;
		
	float roughness = 1.0;
	if (EnableRoughnessMap)
		roughness = texture(RoughnessMap, uvs).r; 
		
	if (EnableRoughnessMap1 && (NumMaterialLayer > 1 || NumDetailLayer > 0)) 
	{
		float weight = NumDetailLayer > 0 ? mask.r : layer_weight;
		roughness = mix(roughness, texture(RoughnessMap1, uvs).r, weight);
	}
	float ao = 1.0;
	if (EnableAOMap)
		ao = texture(AOMap, uvs).r;
	if (EnableAOMap1) 
	{
		float layer_ao = texture(AOMap1, uvs).r;
		if (NumDetailLayer > 0)
			ao *= mix(1.0, layer_ao, mask.r);
		else if (NumMaterialLayer > 1)
			ao = mix(ao, layer_ao, layer_weight);
	}
	if (EnableAOMap2) 
	{
		float layer_ao = texture(AOMap2, uvs).r;
		if (NumDetailLayer > 1)
			ao *= mix(1.0, layer_ao, mask.g);
		else if (NumMaterialLayer > 2)
			ao = mix(ao, layer_ao, mask.g);
	}

	vec3 emission = vec3(0.0);
	if (EnableEmissionColorMap) 
	{
		vec4 emission_sample = texture(EmissionColorMap, uvs);
		vec3 tint = EmissionColor.rgb;
		if (EnableLerpBaseColorEmission) 
		{
			tint = mix(base.rgb, tint, emission_sample.a); 
		}
		emission = emission_sample.rgb * tint * EmissionIntensity;
	}
	if (EnableLightMap) 
	{
		vec2 light_uv = NumRequiredUV > 1 ? vec2(UV2.x, 1.0 - UV2.y) : uvs;
		emission += base.rgb * texture(LightMap, light_uv).rgb * ao;
	}

	ALBEDO = base.rgb;
	NORMAL_MAP = normal * 0.5 + 0.5;
	METALLIC = clamp(metallic, 0.0, 1.0);
	ROUGHNESS = clamp(roughness, 0.0, 1.0);
	SPECULAR = 0.5;
	EMISSION = emission;
	AO = clamp(ao, 0.0, 1.0);
}
