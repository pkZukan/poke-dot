shader_type spatial;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D NormalMap : hint_normal;
uniform sampler2D RoughnessMap;
uniform sampler2D AOMap;
uniform sampler2D SSSMaskMap;

uniform bool EnableBaseColorMap;
uniform bool EnableNormalMap;
uniform bool EnableRoughnessMap;
uniform bool EnableAOMap;
uniform bool EnableSSSMaskMap;
uniform int NumMaterialLayer;
uniform bool EnableVertexSubLayer;
uniform bool EnableCustomLayer;
uniform int LightReceiveType;

void vertex() {
	//
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	ALBEDO = texture(BaseColorMap, uvs).rgb;
	ALPHA = 0.0f;
	NORMAL_MAP = texture(NormalMap, uvs).rgb;
}
