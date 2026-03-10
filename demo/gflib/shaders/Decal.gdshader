shader_type spatial;

uniform sampler2D BaseColorMap : source_color;

uniform float VertexColorIntensity;
uniform float DiscardValue;
uniform float NormalHeight;
uniform float Metallic;
uniform float Roughness;
uniform float EmissionIntensity;

uniform vec4 UVScaleOffset;

void vertex() {
	//
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	vec4 albedo = texture(BaseColorMap, uvs);
	ALBEDO = albedo.rgb;
	ALPHA = albedo.a;
}