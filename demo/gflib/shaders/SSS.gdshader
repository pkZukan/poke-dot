shader_type spatial;

render_mode cull_front;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D LayerMaskMap;
uniform sampler2D NormalMap : hint_normal;
uniform sampler2D NormalMap1 : hint_normal;
uniform sampler2D AOMap;

uniform bool EnableBaseColorMap;
uniform bool EnableNormalMap;
uniform bool EnableRoughnessMap;
uniform bool EnableAOMap;
uniform int NumMaterialLayer;
uniform bool EnableSSSMaskMaps;

void vertex() {
	//
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	float ao_factor = 1.0;
	if(EnableAOMap) ao_factor = texture(AOMap, uvs).r; 
	ALBEDO = texture(BaseColorMap, uvs).rgb * ao_factor;
	NORMAL_MAP = texture(NormalMap, uvs).rgb;
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
